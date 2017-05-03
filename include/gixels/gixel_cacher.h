#pragma once

#include <array>

#include <boost/filesystem.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace boost::filesystem;
using namespace cv;

namespace gixels
{

typedef std::shared_ptr<Mat> MatPtr;

class Gixel
{
public:
    Gixel(const path& p, Size micro_size, bool square, float grey_scale_)
    {
        std::vector<path> v;
        std::copy(directory_iterator(p), directory_iterator(), back_inserter(v));
        std::sort(v.begin(), v.end());
        for (std::vector<path>::const_iterator itr = v.begin(); itr != v.end(); ++itr)
        {
            std::string fn = itr->string();
            Mat orig = imread(fn, CV_LOAD_IMAGE_COLOR);
            Mat down(micro_size, orig.depth());
            resize(orig, down, down.size(), 0, 0, INTER_LANCZOS4);
            Mat down_hsv;
            cvtColor(down, down_hsv, CV_BGR2HSV);
            frames.push_back(down_hsv);
        }

        storeRepresentativeHue();

        grey_scale = grey_scale_;
    }
    
    void fillPixel(MatPtr& current_mapped_frame,
                   const MatPtr& current_macro_frame,
                   uint micro_index,
                   uint i,
                   uint j)
    {
        Size micro_size = frames[0].size();

        Mat tmp_region = Mat(*current_mapped_frame, Rect(micro_size.width*i,
                                                         micro_size.height*j,
                                                         micro_size.width,
                                                         micro_size.height));

        frames[getMicroIndex(current_macro_frame, micro_index, i, j)].copyTo(tmp_region);
    }

    float distanceFromHue(const Vec3b& pixel)
    {
        return distanceFromHue(pixel(0));
    }

    float distanceFromHue(float hue)
    {
        float d1 = fabs(hue - representative_hue);
        float d2 = fabs((hue+360) - representative_hue);
        float d3 = fabs(hue - (representative_hue+360));

        return std::min(std::min(d1, d2), d3);
    }

    float getRepresentativeHue()
    {
        return representative_hue;
    }

    std::vector<Mat> getFrames()
    {
        return frames;
    }

    void merge(const std::shared_ptr<Gixel>& other_gixel)
    {
        std::vector<Mat> other_frames = other_gixel->getFrames();

        frames.insert(frames.begin(), other_frames.begin(), other_frames.end());
    }

private:

    uint getMicroIndex(const MatPtr& current_macro_frame, uint micro_index, uint i, uint j)
    {
        float spread = frames.size()*grey_scale;
        float val = current_macro_frame->at<Vec3b>(j,i)(2);
        int shader = int(val/float(256.0/spread)); // rel value, max of spread, min of 0
        uint index = (shader + micro_index) % frames.size();

        return index;
    }

    void storeRepresentativeHue()
    {
        double h_x = 0;
        double h_y = 0;
        for (const Mat& frame : frames)
        {
            for (uint i = 0; i < frame.size().width; i++)
            {
                for (uint j = 0; j < frame.size().height; j++)
                {
                    int val = ((int)frame.at<Vec3b>(j,i)(0)) % 180;
                    double h = val*2.0/180.0*M_PI;
                    h_x += cos(h);
                    h_y += sin(h);
                }
            }
        }
        representative_hue = atan2(h_y, h_x);
        if (representative_hue < 0)
        {
            representative_hue += 2*M_PI;
        }
        representative_hue *= 180/M_PI;
    }

    std::vector<Mat> frames;
    float representative_hue;
    float grey_scale;
};

typedef std::shared_ptr<Gixel> GixelPtr;

bool gixelCompare(GixelPtr g1, GixelPtr g2)
{
    return (g1->getRepresentativeHue() < g2->getRepresentativeHue());
}

class GixelCacher
{
public:
    GixelCacher(){}
    GixelCacher(std::string p, Size micro_size, float grey_scale_)
    {
        // go through the directory and find all folders, then add those folders as a Gixel
        std::vector<GixelPtr> loaded_gixels;
        path gixel_dir(p);
        directory_iterator end_itr;
        bool found_subs = false;
        for (directory_iterator itr(gixel_dir);
             itr != end_itr;
             ++itr)
        {
            if (is_directory(itr->status()))
            {
                GixelPtr new_gixel(new Gixel(itr->path(), micro_size, true, grey_scale_));
                loaded_gixels.push_back(new_gixel);
                found_subs = true;
            }
        }
        // just fill with the path if there is only a directory of frames being pointed to
        if (!found_subs)
        {
            GixelPtr new_gixel(new Gixel(gixel_dir, micro_size, true, grey_scale_));
            loaded_gixels.push_back(new_gixel);
        }

        // Then sort the vector of gixels
        std::sort(loaded_gixels.begin(), loaded_gixels.end(), gixelCompare);

        // Then store the voronoi region array
        for (int h = 0; h < gixels.size(); h++)
        {
            GixelPtr closest_gixel;
            float least_hue_dist = -1;
            std::vector<GixelPtr> coincident_gixels;
            int closest_index = 0;
            int index = 0;
            for (GixelPtr g : loaded_gixels)
            {
                float distance = g->distanceFromHue(float(h));
                if ((least_hue_dist < 0 ||
                     distance < least_hue_dist))
                {
                    gixels[h] = g;
                    least_hue_dist = distance;
                    closest_index = index;
                }
                index++;
            }

            index = 0;
            for (GixelPtr g : loaded_gixels)
            {
                float distance = g->distanceFromHue(float(h));
                if (distance < 1 && index != closest_index)
                {
                    coincident_gixels.push_back(g);
                }
                index++;
            }

            for (GixelPtr c_g : coincident_gixels)
            {
                gixels[h]->merge(c_g);
            }
        }
    }

    void fillFrame(MatPtr& mapped_frame, const MatPtr& macro_frame, uint micro_index)
    {
        for (uint i = 0; i < macro_frame->size().width; i++)
        {
            for (uint j = 0; j < macro_frame->size().height; j++)
            {
                // find the micro frame with the closest distance to the macro frame in voronoi region
                int hue_index = round((int(macro_frame->at<Vec3b>(j,i)(micro_index))%180)*2);
                hue_index = (hue_index + micro_index) % 360;
                GixelPtr closest_gixel = gixels[hue_index];

                // then set it
                closest_gixel->fillPixel(mapped_frame, macro_frame, micro_index, i, j);
            }
        }
    }

    int loopNumber(int micro_index)
    {
        return int(floor(float(micro_index) / 360.0));
    }

private:
    std::array<GixelPtr, 360> gixels; // cached array of frames, evenly spaced in voronoi regions, 360 entries
    Size micro_size;
    float micro_aspect_ratio;
};

}
