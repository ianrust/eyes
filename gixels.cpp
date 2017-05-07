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

    float grey_scale = 2.0;
    if (recipe["grey_scale"])
    {
        grey_scale = recipe["grey_scale"].as<float>();
    }
    if (mode == "static")
    {
        return std::make_shared<StaticGenerator>(
            recipe["macro_path"].as<std::string>(),
            recipe["macro_height"].as<int>(),
            recipe["micro_path"].as<std::string>(),
            recipe["micro_height"].as<int>(),
            recipe["macro_path"].as<std::string>(),
            recipe["fps"].as<float>(),
            grey_scale
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
            recipe["fps"].as<float>(),
            grey_scale
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
            recipe["fps"].as<float>(),
            grey_scale
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
            recipe["fps"].as<float>(),
            grey_scale
        );
    }
    else
    {
        std::cerr << "unrecognized mode " << mode << std::endl;
        return NULL;
    }
}

struct KeyMap
{
    std::string recipe_path;
    char key;
    KeyMap(std::string recipe_path_, char key_) :
        recipe_path(recipe_path_),
        key(key_)
    {}
};

std::array<std::shared_ptr<MacroFrameGenerator>, 255> gens_map;

void placeGen(KeyMap key_map)
{
    int index = (int)key_map.key;
    gens_map[index] = generatorFromOption(key_map.recipe_path);
}

// Freedom
// Beneath
// Kayne
// Secrets
// Year of Valor

int main( int argc, char** argv )
{
    std::string recipe_path(argv[1]);

    std::string filename;
    bool save = argc > 2;
    if (save)
    {
        filename = std::string(argv[2]);
    }

    // std::vector<KeyMap> key_maps = {
    //     KeyMap("resources/recipes/esplode_lightbulb.yaml", '`'),
    // };

    std::vector<KeyMap> key_maps = {
        // intro
        KeyMap("resources/recipes/eye_remosaicsartwork.yaml", ','),
    
        // Freedom
        KeyMap("resources/recipes/sunset1_explode.yaml", '1'),
        KeyMap("resources/recipes/orbit_explode.yaml", '2'),
        KeyMap("resources/recipes/shuttle_geom.yaml", '3'),
        KeyMap("resources/recipes/longtrail_smoke1.yaml", '4'),
        KeyMap("resources/recipes/crystal_geom.yaml", '5'),
        KeyMap("resources/recipes/mithril_wave.yaml", '6'),
        KeyMap("resources/recipes/wave_wave.yaml", '7'),
        KeyMap("resources/recipes/puff_fastpaint.yaml", '8'),
    
        // break
        KeyMap("resources/recipes/sunbulb.yaml", '9'),
    
        // Beneath
        KeyMap("resources/recipes/jelly_explode.yaml", '0'),
        KeyMap("resources/recipes/shroom_geomflower.yaml", '-'),
        KeyMap("resources/recipes/shroom_face2.yaml", '='),
        KeyMap("resources/recipes/whispflower_smoke1.yaml", 'q'),
        KeyMap("resources/recipes/electric_lightbulb.yaml", 'w'),
        KeyMap("resources/recipes/explode_explode.yaml", 'e'),
        KeyMap("resources/recipes/fractalsmoke_smoke1.yaml", 'r'),
        KeyMap("resources/recipes/run_bloom1.yaml", 't'),
        KeyMap("resources/recipes/explode_explode.yaml", 'y'),
    
        // break
        KeyMap("resources/recipes/yeezy_smoke1.yaml", 'u'),
    
        // kanye gifs
        KeyMap("resources/recipes/nosmiling_explode.yaml", 'i'),
        KeyMap("resources/recipes/kanyeheart_face2.yaml", 'o'),
        KeyMap("resources/recipes/headsmoke_wave.yaml", 'p'),
        KeyMap("resources/recipes/bart_geomflower.yaml", '['),
        KeyMap("resources/recipes/gridsunset_geomflower.yaml", ']'),
        KeyMap("resources/recipes/cig_wave.yaml", '\\'),
        KeyMap("resources/recipes/me_glasses_smoke1.yaml", 'a'),
        KeyMap("resources/recipes/muppet_face2.yaml", 's'),
        KeyMap("resources/recipes/kanyeheart_smoke1.yaml", 'd'),
    
        // break
        KeyMap("resources/recipes/comet_lightbulb.yaml", 'd'),
    
        // Secrets
        KeyMap("resources/recipes/body_smoke1.yaml", 'f'),
        KeyMap("resources/recipes/cube_smoke1.yaml", 'g'),
        KeyMap("resources/recipes/pourskull_bloom1.yaml", 'h'),
        KeyMap("resources/recipes/skullexplode_lightbulb.yaml", 'j'),
        KeyMap("resources/recipes/burningman_lightbulb.yaml", 'k'),
        KeyMap("resources/recipes/burningman_explode.yaml", 'l'),
        KeyMap("resources/recipes/3dfractal_geomflower.yaml", ';'),
        KeyMap("resources/recipes/mandelbrot_lightbulb.yaml", '\''),
        KeyMap("resources/recipes/skeleton_face2.yaml", 'z'),
        KeyMap("resources/recipes/nazi_face2.yaml", 'x'),
    
        // break
        KeyMap("resources/recipes/longtrail_lightbulb.yaml", 'c'),
    
        // Year of Valor
        KeyMap("resources/recipes/geomflower_remosaicsartwork.yaml", 'v'),
        KeyMap("resources/recipes/cubefractal_fastpaint.yaml", 'b'),
        KeyMap("resources/recipes/lake_lightbulb.yaml", 'n'),
        KeyMap("resources/recipes/monk_explode.yaml", 'm'),
        KeyMap("resources/recipes/mouthsmoke_smoke1.yaml", ','),
        KeyMap("resources/recipes/mouthsmoke1_smoke1.yaml", '.'),
    
        // outro
        KeyMap("resources/recipes/water_lightbulb.yaml", '/'),
    };

    std::shared_ptr<MacroFrameGenerator> default_gen = generatorFromOption("resources/recipes/eye_remosaicsartwork.yaml");
    for (int i = 0; i < 255; i++)
    {
        gens_map[i] = default_gen;
    }

    int count = 0;
    for (KeyMap& key_map : key_maps)
    {
        std::cout << key_map.recipe_path << std::endl;
        placeGen(key_map);
        count++;
        std::cout << int(100*count/key_maps.size()) << "\%" << std::endl;
    }

    // int i = 0;

    // namedWindow("window_name", CV_WINDOW_NORMAL);
    // setWindowProperty("window_name", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    // std::shared_ptr<Mat> display;

    // while (true) 
    // {
    //     display = gens[i]->getCachedFrame();

    //     imshow("window_name", *display);

    //     int key = gens[i]->timedSleepResponsive();

    //     if (key != 255)
    //     {
    //         i = map[key];
    //         std::cout << key << std::endl;
    //     }
    // }

    int key = '`';

    while (true)
    {
        key = gens_map[key]->loop();
    }

    return 0;
}