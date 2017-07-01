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
    char alt_key;
    KeyMap(std::string recipe_path_, char key_, char alt_key_) :
        recipe_path(recipe_path_),
        key(key_),
        alt_key(alt_key_)
    {}
};

// array of gen/boolean of whether we should color shift

std::array<std::pair<std::shared_ptr<MacroFrameGenerator>, bool>, 255> gens_map;

void placeGen(KeyMap key_map)
{
    std::shared_ptr<MacroFrameGenerator> gen = generatorFromOption(key_map.recipe_path);

    int index = (int)key_map.key;
    int alt_index = (int)key_map.alt_key;
    gens_map[index] = std::pair<std::shared_ptr<MacroFrameGenerator>, bool>(gen, false);
    gens_map[alt_index] = std::pair<std::shared_ptr<MacroFrameGenerator>, bool>(gen, true);
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
    //     KeyMap("resources/recipes/esplode_lightbulb.yaml", '`', '~'),
    // };

    std::vector<KeyMap> key_maps = {
        // intro
        KeyMap("resources/recipes/eye_remosaicsartwork.yaml", ',', '~'),
    
        // Freedom
        KeyMap("resources/recipes/sunset1_explode.yaml", '1', '!'),
        KeyMap("resources/recipes/orbit_explode.yaml", '2', '@'),
        KeyMap("resources/recipes/shuttle_geom.yaml", '3', '#'),
        KeyMap("resources/recipes/longtrail_smoke1.yaml", '4', '$'),
        KeyMap("resources/recipes/crystal_geom.yaml", '5', '%'),
        KeyMap("resources/recipes/mithril_wave.yaml", '6', '^'),
        KeyMap("resources/recipes/wave_wave.yaml", '7', '&'),
        KeyMap("resources/recipes/puff_fastpaint.yaml", '8', '*'),
    
        // break
        KeyMap("resources/recipes/sunbulb.yaml", '9', '('),
    
        // Beneath
        KeyMap("resources/recipes/jelly_explode.yaml", '0', ')'),
        KeyMap("resources/recipes/shroom_geomflower.yaml", '-', '-'),
        KeyMap("resources/recipes/shroom_face2.yaml", '=', '+'),
        KeyMap("resources/recipes/whispflower_smoke1.yaml", 'q', 'Q'),
        KeyMap("resources/recipes/electric_lightbulb.yaml", 'w', 'W'),
        KeyMap("resources/recipes/explode_explode.yaml", 'e', 'E'),
        KeyMap("resources/recipes/fractalsmoke_smoke1.yaml", 'r', 'R'),
        KeyMap("resources/recipes/run_bloom1.yaml", 't', 'T'),
        KeyMap("resources/recipes/explode_explode.yaml", 'y', 'Y'),
    
        // break
        KeyMap("resources/recipes/yeezy_smoke1.yaml", 'u', 'U'),
    
        // kanye gifs
        KeyMap("resources/recipes/nosmiling_explode.yaml", 'i', 'I'),
        KeyMap("resources/recipes/kanyeheart_face2.yaml", 'o', 'O'),
        KeyMap("resources/recipes/headsmoke_wave.yaml", 'p', 'P'),
        KeyMap("resources/recipes/bart_geomflower.yaml", '[', '{'),
        KeyMap("resources/recipes/gridsunset_geomflower.yaml", ']', '}'),
        KeyMap("resources/recipes/cig_wave.yaml", '\\', '|'),
        KeyMap("resources/recipes/me_glasses_smoke1.yaml", 'a', 'A'),
        KeyMap("resources/recipes/kanyeheart_smoke1.yaml", 's', 'S'),
    
        // break
        KeyMap("resources/recipes/comet_lightbulb.yaml", 'd', 'D'),
    
        // Secrets
        KeyMap("resources/recipes/body_smoke1.yaml", 'f', 'F'),
        KeyMap("resources/recipes/cube_smoke1.yaml", 'g', 'G'),
        KeyMap("resources/recipes/pourskull_bloom1.yaml", 'h', 'H'),
        KeyMap("resources/recipes/skullexplode_lightbulb.yaml", 'j', 'J'),
        KeyMap("resources/recipes/burningman_lightbulb.yaml", 'k', 'K'),
        KeyMap("resources/recipes/burningman_explode.yaml", 'l', 'L'),
        KeyMap("resources/recipes/3dfractal_geomflower.yaml", ';', ':'),
        KeyMap("resources/recipes/mandelbrot_lightbulb.yaml", '\'', '\"'),
        KeyMap("resources/recipes/skeleton_face2.yaml", 'z', 'Z'),
        KeyMap("resources/recipes/nazi_face2.yaml", 'x', 'X'),
    
        // break
        KeyMap("resources/recipes/longtrail_lightbulb.yaml", 'c', 'C'),
    
        // Year of Valor
        KeyMap("resources/recipes/geomflower_remosaicsartwork.yaml", 'v', 'V'),
        KeyMap("resources/recipes/cubefractal_fastpaint.yaml", 'b', 'B'),
        KeyMap("resources/recipes/lake_lightbulb.yaml", 'n', 'N'),
        KeyMap("resources/recipes/monk_explode.yaml", 'm', 'M'),
        KeyMap("resources/recipes/mouthsmoke_smoke1.yaml", ',', '<'),
        KeyMap("resources/recipes/mouthsmoke1_smoke1.yaml", '.', '>'),
    
        // outro
        KeyMap("resources/recipes/water_lightbulb.yaml", '/', '?'),
    };

    std::shared_ptr<MacroFrameGenerator> default_gen = generatorFromOption("resources/recipes/eye_remosaicsartwork.yaml");
    for (int i = 0; i < 255; i++)
    {
        gens_map[i] = std::pair<std::shared_ptr<MacroFrameGenerator>, bool>(default_gen, false);
    }

    // int count = 0;
    // for (KeyMap& key_map : key_maps)
    // {
    //     std::cout << key_map.recipe_path << std::endl;
    //     placeGen(key_map);
    //     count++;
    //     std::cout << int(100*count/key_maps.size()) << "\%" << std::endl;
    // }

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
        key = gens_map[key].first->loop(gens_map[key].second);
    }

    return 0;
}