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

std::shared_ptr<MacroFrameGenerator>
generatorFromOption(std::string option_path)
{
    YAML::Node recipe = YAML::LoadFile(option_path);
    std::string mode = recipe["mode"].as<std::string>();

    if (mode == "static")
    {
        return std::make_shared<StaticGenerator>(
            recipe["macro_path"].as<std::string>(),
            recipe["macro_height"].as<int>(),
            recipe["micro_path"].as<std::string>(),
            recipe["micro_height"].as<int>(),
            recipe["macro_path"].as<std::string>(),
            recipe["fps"].as<float>()
        );
    }
    else if (mode == "gif")
    {
        return std::make_shared<GifGenerator>(
            recipe["macro_path"].as<std::string>(),
            recipe["macro_height"].as<int>(),
            recipe["micro_path"].as<std::string>(),
            recipe["micro_height"].as<int>(),
            recipe["macro_path"].as<std::string>(),
            recipe["fps"].as<float>()
        );
    }
    else if (mode == "mirror")
    {
        return std::make_shared<CamGenerator>(
            recipe["cam_number"].as<int>(),
            recipe["macro_height"].as<int>(),
            recipe["micro_path"].as<std::string>(),
            recipe["micro_height"].as<int>(),
            recipe["macro_path"].as<std::string>(),
            recipe["fps"].as<float>()
        );
    }
    else if (mode == "video")
    {
        
        return std::make_shared<VideoGenerator>(
            recipe["macro_path"].as<std::string>(),
            recipe["macro_height"].as<int>(),
            recipe["micro_path"].as<std::string>(),
            recipe["micro_height"].as<int>(),
            recipe["macro_path"].as<std::string>(),
            recipe["fps"].as<float>()
        );
    }
    else
    {
        std::cerr << "unrecognized mode " << mode << std::endl;
        return NULL;
    }
}

int main( int argc, char** argv )
{
    std::string recipe_path(argv[1]);

    std::string filename;
    bool save = argc > 2;
    if (save)
    {
        filename = std::string(argv[2]);
    }

    std::vector<std::string> options = {
        "resources/recipes/eyes.yaml",
        "resources/recipes/sunbulb.yaml",
        "resources/recipes/geodude.yaml",
        "resources/recipes/mithrilswirl.yaml",
    };

    int map[255];
    for (int i = 0; i < 255; i++)
    {
        map[i] = 0;
    }
    map[100] = 0;
    map[101] = 1;
    map[102] = 2;
    map[103] = 3;


    std::vector<std::shared_ptr<MacroFrameGenerator>> gens;
    for (std::string& option : options) {
        gens.push_back(generatorFromOption(option));
        std::cout << int(100*gens.size()/options.size()) << "\%" << std::endl;
    }

    size_t index = 0;

    int i = 0;

    namedWindow("window_name", CV_WINDOW_NORMAL);
    setWindowProperty("window_name", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    std::shared_ptr<Mat> display;

    while (true) 
    {
        display = gens[i]->getCachedFrame();

        imshow("window_name", *display);

        int key = gens[i]->timedSleepResponsive();

        if (key != 255)
        {
            i = map[key];
        }
    }

    return 0;
}