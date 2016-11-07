#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <iostream>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

typedef std::shared_ptr<Mat> MatPtr;
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

    void loop(std::string window_name)
    {
        MatPtr frame;
        namedWindow(window_name, CV_WINDOW_AUTOSIZE);
        while (true)
        {
            frame = getMappedFrame();
            imshow(window_name, *frame);
            timedSleep(15);
        }
    }

protected:

    void timedSleep(float fps)
    {
        while (secondsSinceRelease() < 1/fps)
        {
            waitKey(1);
        }
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

    std::chrono::time_point<std::chrono::system_clock> last_call;
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

class VideoGenerator : public MacroFrameGenerator
{
public:
    VideoGenerator(std::string macro_path, int macro_height, std::string micro_path, int micro_height, int num_micro_frames)
    {
        storeMicroFrames(micro_path, micro_height, num_micro_frames);

        cam = VideoCapture(macro_path);
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
        // read frame from video
        Mat tmp, tmp_gray;
        cam.read(tmp);
        cvtColor(tmp, tmp_gray, CV_RGB2GRAY); 
        resize(tmp_gray, *current_macro_frame, macro_size, 0, 0, INTER_LANCZOS4);
        return;
    }

    VideoCapture cam;
};


int main( int argc, char** argv )
{
    std::string mode(argv[1]);
    std::string macro_path(argv[2]);
    std::string micro_path(argv[3]);
    int num_frames;
    sscanf(argv[4], "%d", &num_frames);

    if (mode == "static")
    {
        StaticGenerator gen(macro_path, 40, micro_path, 70, num_frames);
        gen.loop("static");
    }
    else if (mode == "gif")
    {
        GifGenerator gen(macro_path, 50, num_frames, micro_path, 40, num_frames);
        gen.loop("animated geoff");
    }
    else if (mode == "mirror")
    {
        CamGenerator gen(30, micro_path, 60, num_frames);
        gen.loop("mirror");
    }
    else if (mode == "video")
    {
        VideoGenerator gen(macro_path, 100, micro_path, 19, num_frames);
        gen.loop(macro_path);
    }
    else
    {
        std::cerr << "unrecognized mode " << mode << std::endl;
        return 1;
    }

    return 0;
}