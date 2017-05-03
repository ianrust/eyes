#pragma once

#include <gixels/macro_frame_generator.h>

namespace gixels
{

class StaticGenerator : public MacroFrameGenerator
{
public:
    StaticGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, float grey_scale_)
    {
        storeMicroFrames(micro_path, micro_height, grey_scale_);

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
    GifGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, float fps_multiplier, float grey_scale_)
    {
        storeMicroFrames(micro_path, micro_height, grey_scale_);
        _fps_multiplier = fps_multiplier;

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

protected:
    void setMacroFrame()
    {
        // cycle through
        *current_macro_frame = macro_frames[int(float(micro_index)*_fps_multiplier) % macro_frames.size()];
        return;
    }

    std::vector<Mat> macro_frames;
    float _fps_multiplier;
};

class CamGenerator : public MacroFrameGenerator
{
public:
    CamGenerator(int cam_number, int macro_height, std::string micro_path, int micro_height, float grey_scale_)
    {
        storeMicroFrames(micro_path, micro_height, grey_scale_);

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
    VideoGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, float grey_scale_)
    {
        namedWindow("window_name", CV_WINDOW_NORMAL);
        std::cout << "b" << std::endl;
        storeMicroFrames(micro_path, micro_height, grey_scale_);
        std::cout << "b" << std::endl;

        cam = VideoCapture(macro_path);
        // cam.set(CV_CAP_PROP_FOURCC, )
        std::cout << macro_path << std::endl;
        Mat tmp_src;
        while (!cam.isOpened()) {
            std::cout << "blag" << std::endl;
        }
            imshow("window_name", tmp_src);
        std::cout << "b" << std::endl;

        int width = tmp_src.size().width * float(macro_height) / float(tmp_src.size().height);
        std::cout << "b" << std::endl;
        int height = macro_height;
        macro_size = Size(width, height);
        
        std::cout << "be" << std::endl;
        std::cout << tmp_src.depth() << std::endl;
        std::cout << tmp_src.size() << std::endl;
        std::cout << macro_size << std::endl;
        current_macro_frame = std::make_shared<Mat>(macro_size, tmp_src.depth());

        std::cout << "bi" << std::endl;
        // initilize the mapped frame;
        instantiateMappedFrame();
        std::cout << "b" << std::endl;
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