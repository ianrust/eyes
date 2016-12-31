#include <yaml-cpp/yaml.h>

#include <iostream>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <stdio.h>

#include <gixels/frame_generators.h>

using namespace gixels;

// TODO 
// remove all absolute (/home/ian) paths
// simpler paths in yaml files
// crop images, don't resize
// fix ratio on single gixel things

int main( int argc, char** argv )
{
    std::string recipe_path(argv[1]);
    YAML::Node recipe = YAML::LoadFile(recipe_path);

    std::string mode = recipe["mode"].as<std::string>();

    std::string filename;
    bool save = argc > 2;
    if (save)
    {
        filename = std::string(argv[2]);
    }

    if (mode == "static")
    {

        StaticGenerator gen(recipe["macro_path"].as<std::string>(),
                            recipe["macro_height"].as<int>(),
                            recipe["micro_path"].as<std::string>(),
                            recipe["micro_height"].as<int>());
        if (!save)
        {
            gen.loop("static", recipe["fps"].as<float>());
        }
        else
        {
            gen.loopSave(filename, recipe["fps"].as<float>());
        }
    }
    else if (mode == "gif")
    {
        GifGenerator gen(recipe["macro_path"].as<std::string>(),
                         recipe["macro_height"].as<int>(),
                         recipe["micro_path"].as<std::string>(),
                         recipe["micro_height"].as<int>());
        if (!save)
        {
            gen.loop("animated geoff", recipe["fps"].as<float>());
        }
        else
        {
            gen.loopSave(filename, recipe["fps"].as<float>());
        }
    }
    else if (mode == "mirror")
    {
        CamGenerator gen(recipe["cam_number"].as<int>(),
                         recipe["macro_height"].as<int>(),
                         recipe["micro_path"].as<std::string>(),
                         recipe["micro_height"].as<int>());
        if (!save)
        {
            gen.loop("mirror", recipe["fps"].as<float>());
        }
        else
        {
            gen.loopSave(filename, recipe["fps"].as<float>());
        }
    }
    else if (mode == "video")
    {
        VideoGenerator gen(recipe["macro_path"].as<std::string>(),
                           recipe["macro_height"].as<int>(),
                           recipe["micro_path"].as<std::string>(),
                           recipe["micro_height"].as<int>());
        if (!save)
        {
            gen.loop(recipe["macro_path"].as<std::string>(), recipe["fps"].as<float>());
        }
        else
        {
            gen.loopSave(filename, recipe["fps"].as<float>());
        }
    }
    else
    {
        std::cerr << "unrecognized mode " << mode << std::endl;
        return 1;
    }

    return 0;
}