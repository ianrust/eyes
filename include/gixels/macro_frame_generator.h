#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

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

    void loop(std::string window_name, float fps)
    {
        MatPtr frame_hsv;
        Mat frame;
        namedWindow(window_name, CV_WINDOW_NORMAL);
        while (true)
        {
            frame_hsv = getMappedFrame();
            cvtColor(*frame_hsv, frame, CV_HSV2BGR);
            setWindowProperty(window_name, CV_WND_PROP_FULLSCREEN, 1);
            imshow(window_name, frame);
            timedSleep(fps);
        }
    }

protected:

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

    std::chrono::time_point<std::chrono::system_clock> last_call;
};

}