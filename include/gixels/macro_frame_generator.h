#pragma once

#include <chrono>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <gixels/gixel_cacher.h>

using namespace cv;

namespace gixels
{

typedef std::chrono::high_resolution_clock Clock;

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
    MacroFrameGenerator(std::string window_name_, float fps_, float grey_scale_) :
        window_name(window_name_),
        fps(fps_),
        grey_scale(grey_scale_)
    {}

    void fillCachedFrames()
    {
        MatPtr frame_hsv;
        Mat frame, frame_scaled;
        int out_height = 1050;
        int out_width = 1680;
        float output_aspect = float(out_width)/float(out_height);
        while (!finishedDisplayLoop())
        {
            Mat output = Mat(out_height, out_width, CV_8UC3, Scalar(0,0,0));
            frame_hsv = getMappedFrame();
            cvtColor(*frame_hsv, frame, CV_HSV2BGR);

            float frame_height = frame.size().height;
            float frame_width = frame.size().width;
            float frame_aspect = frame_width/frame_height;

            if (frame_aspect < output_aspect)
            {
                int scale_width = int(frame_aspect*out_height);
                resize(frame, frame_scaled, Size(scale_width, out_height));
                frame_scaled.copyTo(output(Rect(out_width/2-scale_width/2, 0, scale_width, out_height)));
            }
            else
            {
                int scale_height = int(out_width/frame_aspect);
                resize(frame, frame_scaled, Size(out_width, scale_height));
                frame_scaled.copyTo(output(Rect(0, out_height/2-scale_height/2, out_width, scale_height)));
            }

            cached_frames.push_back(std::make_shared<Mat>(output));
        }
    }

    std::shared_ptr<Mat> getCachedFrame()
    {
        static int loop_number = 0;

        loop_number++;
        if (loop_number >= cached_frames.size())
        {
            loop_number = 0;
        }

        return cached_frames[loop_number];
    }

    int loop(bool color_shift)
    {
        MatPtr frame_hsv;
        Mat frame, frame_scaled, frame_rotated;
        int out_height = 1050;
        int out_width = 1680;
        float output_aspect = float(out_width)/float(out_height);
        Mat output(out_height, out_width, CV_8UC3, Scalar(0,0,0));
        namedWindow("window_name", CV_WINDOW_NORMAL);
        setWindowProperty("window_name", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
        while (true)
        {
            frame_hsv = getMappedFrame();
            if (color_shift)
            {
                frame_rotated = *frame_hsv + Scalar((micro_index*4)%255,(micro_index*6)%255,0);
                cvtColor(frame_rotated, frame, CV_HSV2BGR);
            }
            else
            {
                cvtColor(*frame_hsv, frame, CV_HSV2BGR);
            }

            float frame_height = frame.size().height;
            float frame_width = frame.size().width;
            float frame_aspect = frame_width/frame_height;

            if (frame_aspect < output_aspect)
            {
                int scale_width = int(frame_aspect*out_height);
                resize(frame, frame_scaled, Size(scale_width, out_height));
                frame_scaled.copyTo(output(Rect(out_width/2-scale_width/2, 0, scale_width, out_height)));
            }
            else
            {
                int scale_height = int(out_width/frame_aspect);
                resize(frame, frame_scaled, Size(out_width, scale_height));
                frame_scaled.copyTo(output(Rect(0, out_height/2-scale_height/2, out_width, scale_height)));
            }

            imshow("window_name", output);
            int key_pressed = timedSleepResponsive();

            if (key_pressed != 255)
            {
                micro_index = 0;
                return key_pressed;
            }
        }
    }

    int timedSleepResponsive()
    {
        int wait = std::max(1, int(1000*(1/fps - secondsSinceRelease())));
        int key = waitKey(wait);
        last_call = Clock::now();
        return key;
    }

protected:

    MatPtr getMappedFrame(bool& done)
    {
        MatPtr return_frame = getMappedFrame();
        done = gixel_cacher.loopNumber(micro_index) == 1;
        return return_frame;
    }

    MatPtr getMappedFrame() // rgb micro frame mapped to pixels to current macro frame
    {
        incrementMicroIndex();

        setMacroFrame();

        // fill from gixel cacher
        gixel_cacher.fillFrame(current_mapped_frame,
                               current_macro_frame,
                               micro_index);

        return current_mapped_frame;
    }

    void timedSleep(float fps)
    {
        int wait = std::max(1, int(1000*(1/fps - secondsSinceRelease())));
        int key = waitKey(wait);
        last_call = Clock::now();
    }

    float secondsSinceRelease()
    {
        auto now = Clock::now();
        long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_call).count();
        float s = float(ns) * 1e-9;
        return s;
    }

    void instantiateMappedFrame()
    {
        // initalize macro frame
        setMacroFrame();
        Size mapped_size(macro_size.width*micro_size.width,
                         macro_size.height*micro_size.height);
        current_mapped_frame = std::make_shared<Mat>(mapped_size, CV_8UC3);
        last_call = Clock::now();

        fillCachedFrames();
    }

    void incrementMicroIndex()
    {
        micro_index+=1;
    }

    virtual void setMacroFrame() = 0; // get macro frame that is greyscale
    virtual bool finishedDisplayLoop() = 0;

    void storeMicroFrames(std::string micro_path, int micro_height, float grey_scale)
    {
        micro_size = Size(micro_height, micro_height);
        gixel_cacher = GixelCacher(micro_path, micro_size, grey_scale);
    }

    MatPtr current_mapped_frame; // hsv

    MatPtr current_macro_frame; // hsv
    Size macro_size;

    std::vector<std::shared_ptr<Mat>> cached_frames;

    GixelCacher gixel_cacher; // hsv
    Size micro_size;
    uint micro_index = 0;

    std::string window_name;
    float fps;
    float grey_scale;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_call;
};

}