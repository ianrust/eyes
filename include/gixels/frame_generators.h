#pragma once

#include <gixels/macro_frame_generator.h>

namespace gixels
{

class StaticGenerator : public MacroFrameGenerator
{
public:
    StaticGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, std::string window_name, float fps) :
        MacroFrameGenerator(window_name, fps)
    {
        storeMicroFrames(micro_path, micro_height);

        Mat frame_static_orig = imread(macro_path, CV_LOAD_IMAGE_COLOR);
        Mat frame_static_orig_hsv;
        cvtColor(frame_static_orig, frame_static_orig_hsv, CV_BGR2HSV);
        int height = macro_height;
        int width = frame_static_orig_hsv.size().width * float(macro_height) / float(frame_static_orig_hsv.size().height);
        macro_size = Size(width, height);
        current_macro_frame = std::make_shared<Mat>(macro_size, frame_static_orig_hsv.depth());
        resize(frame_static_orig_hsv, *current_macro_frame, current_macro_frame->size(), 0, 0, INTER_LANCZOS4);

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

    bool finishedDisplayLoop()
    {

        return (((micro_index % 360) <= 3) && (micro_index > 3));
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
    GifGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, std::string window_name, float fps) :
        MacroFrameGenerator(window_name, fps)
    {
        storeMicroFrames(micro_path, micro_height);

        path gif_path(macro_path);
        std::vector<path> v;
        std::copy(directory_iterator(gif_path), directory_iterator(), back_inserter(v));
        std::sort(v.begin(), v.end());
        for (std::vector<path>::const_iterator itr = v.begin(); itr != v.end(); ++itr)
        {
            std::string fn = itr->string();
            Mat orig = imread(fn, CV_LOAD_IMAGE_COLOR);
            Mat orig_hsv;
            cvtColor(orig, orig_hsv, CV_BGR2HSV);
            int width = orig_hsv.size().width * float(macro_height) / float(orig_hsv.size().height);
            int height = macro_height;
            macro_size = Size(width, height);
            Mat down_macro(macro_size, orig_hsv.depth());
            resize(orig_hsv, down_macro, down_macro.size(), 0, 0, INTER_LANCZOS4);
            macro_frames.push_back(down_macro);
        }
        
        current_macro_frame = std::make_shared<Mat>(macro_size, macro_frames[0].depth());

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

    bool finishedDisplayLoop()
    {
        return (((micro_index % 360) <= 3) && ((micro_index % macro_frames.size()) <= 3) && (micro_index > 3));
    }

protected:
    void setMacroFrame()
    {
        // cycle through
        *current_macro_frame = macro_frames[micro_index % macro_frames.size()];
        return;
    }

    std::vector<Mat> macro_frames;
};

class CamGenerator : public MacroFrameGenerator
{
public:
    CamGenerator(int cam_number, int macro_height, std::string micro_path, int micro_height, std::string window_name, float fps) :
        MacroFrameGenerator(window_name, fps)
    {
        storeMicroFrames(micro_path, micro_height);

        cam = VideoCapture(cam_number);
        Mat tmp_src;
        cam.read(tmp_src);

        int width = tmp_src.size().width * float(macro_height) / float(tmp_src.size().height);
        int height = macro_height;
        macro_size = Size(width, height);
        
        current_macro_frame = std::make_shared<Mat>(macro_size, tmp_src.depth());

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

    bool finishedDisplayLoop()
    {
        return true;
    }

protected:
    void setMacroFrame()
    {
        // read frame from cam
        Mat tmp, tmp_mirror, tmp_hsv;
        cam.read(tmp);
        cv::flip(tmp, tmp_mirror, 1);
        cvtColor(tmp_mirror, tmp_hsv, CV_BGR2HSV); 
        resize(tmp_hsv, *current_macro_frame, macro_size, 0, 0, INTER_LANCZOS4);
        return;
    }

    VideoCapture cam;
};

class VideoGenerator : public MacroFrameGenerator
{
public:
    VideoGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, std::string window_name, float fps) :
        MacroFrameGenerator(window_name, fps)
    {
        storeMicroFrames(micro_path, micro_height);

        cam = VideoCapture(macro_path);
        Mat tmp_src;
        cam.read(tmp_src);

        int width = tmp_src.size().width * float(macro_height) / float(tmp_src.size().height);
        int height = macro_height;
        macro_size = Size(width, height);
        
        current_macro_frame = std::make_shared<Mat>(macro_size, tmp_src.depth());

        // initilize the mapped frame;
        instantiateMappedFrame();
    }

    bool finishedDisplayLoop()
    {
        return true;
    }

protected:
    void setMacroFrame()
    {
        // read frame from video
        Mat tmp, tmp_hsv;
        cam.read(tmp);
        cvtColor(tmp, tmp_hsv, CV_BGR2HSV); 
        resize(tmp_hsv, *current_macro_frame, macro_size, 0, 0, INTER_LANCZOS4);
        return;
    }

    VideoCapture cam;
};

}