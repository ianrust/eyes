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

void render(VideoWriter writer, const YAML::Node& recipe, float fps, bool save)
{
    std::string mode = recipe["mode"].as<std::string>();

    float grey_scale = 0.5;
    if (recipe["grey_scale"])
    {
        grey_scale = recipe["grey_scale"].as<float>();
    }

    if (mode == "static")
    {
        StaticGenerator gen(recipe["macro_path"].as<std::string>(),
                            recipe["macro_height"].as<int>(),
                            recipe["micro_path"].as<std::string>(),
                            recipe["micro_height"].as<int>(),
                            grey_scale);
        if (!save)
        {
            gen.loop("static", fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, fps, 3.0);
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
                         fps_multiplier,
                         grey_scale);
        if (!save)
        {
            gen.loop("animated geoff", fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, fps, 3.0);
            }
        }
    }
    else if (mode == "mirror")
    {
        CamGenerator gen(recipe["cam_number"].as<int>(),
                         recipe["macro_height"].as<int>(),
                         recipe["micro_path"].as<std::string>(),
                         recipe["micro_height"].as<int>(),
                         grey_scale);
        if (!save)
        {
            gen.loop("mirror", fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, fps, 3.0);
            }
        }
    }
    else if (mode == "video")
    {
        VideoGenerator gen(recipe["macro_path"].as<std::string>(),
                           recipe["macro_height"].as<int>(),
                           recipe["micro_path"].as<std::string>(),
                           recipe["micro_height"].as<int>(),
                           grey_scale);
        if (!save)
        {
            gen.loop(recipe["macro_path"].as<std::string>(), fps);
        }
        else
        {
            if (recipe["duration"])
            {
                gen.loopSave(writer, fps, recipe["duration"].as<float>());
            }
            else
            {
                gen.loopSave(writer, fps, 3.0);
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
            render(writer, sequence, fps, true);
        }
    }
    else if (mode == "combos")
    {
        int total = recipe["macros"].size() * recipe["micros"].size();
        int count = 0;
        for (YAML::iterator macro_it = recipe["macros"].begin(); macro_it != recipe["macros"].end(); ++macro_it)
        {
            std::string macro_path = (*macro_it)["path"].as<std::string>();
            std::string macro_name = macro_path.substr(macro_path.find_last_of("/")+1);
            for (YAML::iterator micro_it = recipe["micros"].begin(); micro_it != recipe["micros"].end(); ++micro_it)
            {
                std::string micro_path = (*micro_it).as<std::string>();
                std::string micro_name = micro_path.substr(micro_path.find_last_of("/")+1);
                std::string name = macro_name + "_" + micro_name + ".avi";
                YAML::Node combo;

                combo["mode"] = (*macro_it)["mode"].as<std::string>();
                combo["macro_path"] = macro_path;
                combo["macro_height"] = recipe["macro_height"].as<int>();
                combo["micro_path"] = micro_path;
                combo["micro_height"] = recipe["micro_height"].as<int>();
                combo["duration"] = recipe["duration"].as<float>();
                combo["fps_multiplier"] = (*macro_it)["fps_multiplier"].as<float>();
                if ((*macro_it)["grey_scale"])
                {
                    combo["grey_scale"] = (*macro_it)["grey_scale"].as<float>();
                }

                VideoWriter combo_writer(name, CV_FOURCC('M','P','E','G'), fps, Size(1920, 1200), true);
                render(combo_writer, combo, fps, true);
                count++;
                std::cout << count << " / " << total << std::endl;
            }
        }
    }
    else
    {
        render(writer, recipe, fps, save);
    }

    return 0;
}