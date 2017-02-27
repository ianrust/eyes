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
    MacroFrameGenerator(){}

    void loopSave(VideoWriter& writer, std::string file_name, float fps, float duration)
    {
        MatPtr frame_hsv;
        Mat frame, frame_scaled;
        bool done = false;
        int frame_count  = 0;

        int out_height = 1200;
        int out_width = 1920;

        float output_aspect = float(out_width)/float(out_height);
        Mat output(out_height, out_width, CV_8UC3, Scalar(0,0,0));

        while (frame_count < duration*fps)
        {
            frame_hsv = getMappedFrame(done);
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

            writer.write(output);
            frame_count++;
        }

    }

    void loop(std::string window_name, float fps)
    {
        MatPtr frame_hsv;
        Mat frame, frame_scaled;
        namedWindow(window_name, CV_WINDOW_NORMAL);

        // projector
        int out_height = 1200;
        int out_width = 1920;
        // retina
        // int out_height = 1050;
        // int out_width = 1680;

        float output_aspect = float(out_width)/float(out_height);
        Mat output(out_height, out_width, CV_8UC3, Scalar(0,0,0));
        while (true)
        {
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

            imshow(window_name, output);
            setWindowProperty(window_name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
            timedSleep(fps);
        }
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
        waitKey(wait);
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
    }

    void incrementMicroIndex()
    {
        micro_index++;
    }

    virtual void setMacroFrame() = 0; // get macro frame that is greyscale

    void storeMicroFrames(std::string micro_path, int micro_height)
    {
        micro_size = Size(micro_height, micro_height);
        gixel_cacher = GixelCacher(micro_path, micro_size);
    }

    MatPtr current_mapped_frame; // hsv

    MatPtr current_macro_frame; // hsv
    Size macro_size;

    GixelCacher gixel_cacher; // hsv
    Size micro_size;
    uint micro_index = 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_call;
};

}