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

void render(VideoWriter writer, const YAML::Node& recipe, float fps, bool save, std::string filename)
{
    std::string mode = recipe["mode"].as<std::string>();

    if (mode == "static")
    {
        StaticGenerator gen(recipe["macro_path"].as<std::string>(),
                            recipe["macro_height"].as<int>(),
                            recipe["micro_path"].as<std::string>(),
                            recipe["micro_height"].as<int>());
        if (!save)
        {
            gen.loop("static", fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, filename, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, filename, fps, 3.0);
            }
        }
    }
    else if (mode == "gif")
    {
        float fps_multiplier = (recipe["fps_multiplier"]) ? recipe["fps_multiplier"].as<float>() : 3.0;
        GifGenerator gen(recipe["macro_path"].as<std::string>(),
                         recipe["macro_height"].as<int>(),
                         recipe["micro_path"].as<std::string>(),
                         recipe["micro_height"].as<int>(),
                         fps_multiplier);
        if (!save)
        {
            gen.loop("animated geoff", fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, filename, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, filename, fps, 3.0);
            }
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
            gen.loop("mirror", fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, filename, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, filename, fps, 3.0);
            }
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
            gen.loop(recipe["macro_path"].as<std::string>(), fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, filename, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, filename, fps, 3.0);
            }
        }
    }
}

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

    float fps = recipe["fps"].as<float>();

    VideoWriter writer(filename, CV_FOURCC('M','P','E','G'), fps, Size(1920, 1200), true);

    if (mode == "sequence")
    {
        for (YAML::iterator it = recipe["sections"].begin(); it != recipe["sections"].end(); ++it)
        {
            const YAML::Node& sequence = *it;
            render(writer, sequence, fps, true, filename);
        }
    }
    else
    {
        render(writer, recipe, fps, save, filename);
    }

    return 0;
}