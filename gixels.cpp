#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <stdio.h>

#include <gixels/frame_generators.h>

using namespace boost::filesystem;
using namespace gixels;

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