#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

typedef std::shared_ptr<Mat> MatPtr;

enum MACRO_FRAME_TYPE
{
    STATIC = 0, //static
    GIF,
    CAM,
    VIDEO
};

class MacroFrameGenerator
{
public:
    MacroFrameGenerator(){}

    MatPtr getMappedFrame() // rgb micro frame mapped to pixels to current macro frame
    {
        incrementMicroIndex();

        setMacroFrame();

        for (uint i = 0; i < macro_size.width; i++)
        {
            for (uint j = 0; j < macro_size.height; j++)
            {
                fillPixel(i, j);
            }
        }

        return current_mapped_frame;
    }

protected:
    void instantiateMappedFrame()
    {
        // initalize macro frame
        setMacroFrame();
        Size mapped_size(macro_size.width*micro_size.width,
                         macro_size.height*micro_size.height);
        current_mapped_frame = std::make_shared<Mat>(mapped_size, CV_8UC3);
    }

    void incrementMicroIndex()
    {
        micro_index++;
    }

    virtual void setMacroFrame() = 0; // get macro frame that is greyscale
    
    void fillPixel(uint i, uint j)
    {
        Mat tmp_region = Mat(*current_mapped_frame, Rect(micro_size.width*i,
                                                         micro_size.height*j,
                                                         micro_size.width,
                                                         micro_size.height));
        micro_frames[getMicroIndex(i, j)].copyTo(tmp_region);
    }

    uint getMicroIndex(uint i, uint j)
    {
        float spread = micro_frames.size()/2;

        int val = current_macro_frame->at<uchar>(j,i);
        int shader = int(float(val)/float(256.0/spread)); // rel grescale, max of spread, min of 0
        uint index = (shader + micro_index) % micro_frames.size();

        return index;
    }

    void storeMicroFrames(std::string micro_path, int micro_height, int num_micro_frames)
    {
        // extract info from first frame
        Mat first_micro = imread("/home/ian/eyes/"+micro_path+"-"+std::to_string(0)+".png", CV_LOAD_IMAGE_GRAYSCALE);
        int micro_width = first_micro.size().width * float(micro_height) / float(first_micro.size().height);
        micro_size = Size(micro_width, micro_height);
        micro_aspect_ratio = float(micro_height)/float(micro_width);

        for (int i = 0; i < num_micro_frames; i++)
        {
            std::string fn = "/home/ian/eyes/"+micro_path+"-"+std::to_string(i)+".png";
            Mat down_eye(micro_size, CV_8UC3);
            Mat orig = imread(fn, CV_LOAD_IMAGE_COLOR);
            resize(orig, down_eye, down_eye.size(), 0, 0, INTER_LANCZOS4);
            micro_frames.push_back(down_eye);
        }

    }

    MatPtr current_mapped_frame; // rgb

    MatPtr current_macro_frame; // greyscale
    Size macro_size;

    std::vector<Mat> micro_frames; // rgb
    Size micro_size;
    float micro_aspect_ratio;
    uint micro_index = 0;
};

class StaticGenerator : public MacroFrameGenerator
{
public:
    StaticGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, int num_micro_frames)
    {
        storeMicroFrames(micro_path, micro_height, num_micro_frames);

        Mat frame_static_orig = imread(macro_path, CV_LOAD_IMAGE_GRAYSCALE);
        int width = frame_static_orig.size().width * float(macro_height) * micro_aspect_ratio / float(frame_static_orig.size().height);
        int height = macro_height;
        macro_size = Size(width, height);
        current_macro_frame = std::make_shared<Mat>(macro_size, CV_8U);
        resize(frame_static_orig, *current_macro_frame, current_macro_frame->size(), 0, 0, INTER_LANCZOS4);

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

protected:
    void setMacroFrame()
    {
        // no action, macro is static
        return;
    }
};

class GifGenerator : public MacroFrameGenerator
{
public:
    GifGenerator(std::string macro_path, int macro_height, int num_macro_frames, std::string micro_path, int micro_height, int num_micro_frames)
    {
        storeMicroFrames(micro_path, micro_height, num_micro_frames);

        for (int i = 0; i < num_macro_frames; i++)
        {
            std::string fn = "/home/ian/eyes/"+macro_path+"-"+std::to_string(i)+".png";
            Mat orig = imread(fn, CV_LOAD_IMAGE_GRAYSCALE);
            int width = orig.size().width * float(macro_height) * micro_aspect_ratio / float(orig.size().height);
            int height = macro_height;
            macro_size = Size(width, height);
            Mat down_macro(macro_size, CV_8U);
            resize(orig, down_macro, down_macro.size(), 0, 0, INTER_LANCZOS4);
            macro_frames.push_back(down_macro);
        }
        
        current_macro_frame = std::make_shared<Mat>(macro_size, CV_8U);

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

protected:
    void setMacroFrame()
    {
        // cycle through
        *current_macro_frame = macro_frames[micro_index % micro_frames.size()];
        return;
    }

    std::vector<Mat> macro_frames;
};

class CamGenerator : public MacroFrameGenerator
{
public:
    CamGenerator(int macro_height, std::string micro_path, int micro_height, int num_micro_frames)
    {
        storeMicroFrames(micro_path, micro_height, num_micro_frames);

        cam = VideoCapture(0);
        Mat tmp_src;
        cam.read(tmp_src);

        int width = tmp_src.size().width * float(macro_height) * micro_aspect_ratio / float(tmp_src.size().height);
        int height = macro_height;
        macro_size = Size(width, height);
        
        current_macro_frame = std::make_shared<Mat>(macro_size, CV_8U);

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

protected:
    void setMacroFrame()
    {
        // read frame from cam
        Mat tmp, tmp_mirror, tmp_gray;
        cam.read(tmp);
        cv::flip(tmp, tmp_mirror, 1);
        cvtColor(tmp_mirror, tmp_gray, CV_RGB2GRAY); 
        resize(tmp_gray, *current_macro_frame, macro_size, 0, 0, INTER_LANCZOS4);
        return;
    }

    VideoCapture cam;
};

//     std::vector<Mat> frame_loop;

/// Global variables
Mat src, dst, black_dst, eye_dst;
std::vector<Mat> frames;
std::vector<Mat> full_frames;
char* window_name = "eyes";

int main( int argc, char** argv )
{
    std::string macro_path(argv[1]);
    std::string micro_path(argv[2]);
    int num_frames;
    sscanf(argv[3], "%d", &num_frames);
    // StaticGenerator gen(macro_path, 80, micro_path, 20, num_frames);
    // GifGenerator gen(macro_path, 80, num_frames, micro_path, 20, num_frames);
    CamGenerator gen(50, micro_path, 35, num_frames);

    MatPtr frame;
    namedWindow(window_name, CV_WINDOW_AUTOSIZE);
    while (true)
    {
        frame = gen.getMappedFrame();
        imshow(window_name, *frame);
        waitKey(40);
    }

    return 0;
}