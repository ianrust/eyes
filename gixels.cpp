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

// TODO 
// make full screen
// make gif -> resource script
// gixel cacher
//   - detect # of frames
//   - allow multiple gifs
//   - closest by HSV
// allow different number of frames in Gif generator
// remove all absolute (/home/ian) paths
// simpler paths in yaml files


int main( int argc, char** argv )
{
    std::string recipe_path(argv[1]);
    YAML::Node recipe = YAML::LoadFile(recipe_path);

    std::string mode = recipe["mode"].as<std::string>();

    if (mode == "static")
    {
        StaticGenerator gen(recipe["macro_path"].as<std::string>(),
                            recipe["macro_height"].as<int>(),
                            recipe["micro_path"].as<std::string>(),
                            recipe["micro_height"].as<int>(),
                            recipe["num_micro_frames"].as<int>());
        gen.loop("static");
    }
    else if (mode == "gif")
    {
        GifGenerator gen(recipe["macro_path"].as<std::string>(),
                         recipe["macro_height"].as<int>(),
                         recipe["num_macro_frames"].as<int>(),
                         recipe["micro_path"].as<std::string>(),
                         recipe["micro_height"].as<int>(),
                         recipe["num_micro_frames"].as<int>());
        gen.loop("animated geoff");
    }
    else if (mode == "mirror")
    {
        CamGenerator gen(recipe["cam_number"].as<int>(),
                         recipe["macro_height"].as<int>(),
                         recipe["micro_path"].as<std::string>(),
                         recipe["micro_height"].as<int>(),
                         recipe["num_micro_frames"].as<int>());
        gen.loop("mirror");
    }
    else if (mode == "video")
    {
        VideoGenerator gen(recipe["macro_path"].as<std::string>(),
                           recipe["macro_height"].as<int>(),
                           recipe["micro_path"].as<std::string>(),
                           recipe["micro_height"].as<int>(),
                           recipe["num_micro_frames"].as<int>());
        gen.loop(recipe["macro_path"].as<std::string>());
    }
    else
    {
        std::cerr << "unrecognized mode " << mode << std::endl;
        return 1;
    }

    return 0;
}