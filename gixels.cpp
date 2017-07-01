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

std::array<std::array<std::pair<std::shared_ptr<MacroFrameGenerator>, bool>, 255>, 6> gens_maps;


int animation_count = 0;

// macbook
int out_height = 1050;
int out_width = 1680;

// Ilya's Projector
// int out_height = 720;
// int out_width = 1280;

// Independent
// int out_height = 768;
// int out_width = 1024;

void placeGen(int map_index, KeyMap key_map)
{
    std::shared_ptr<MacroFrameGenerator> gen = generatorFromOption(key_map.recipe_path);

    int index = (int)key_map.key;
    int alt_index = (int)key_map.alt_key;
    gens_maps[map_index][index] = std::pair<std::shared_ptr<MacroFrameGenerator>, bool>(gen, false);
    gens_maps[map_index][alt_index] = std::pair<std::shared_ptr<MacroFrameGenerator>, bool>(gen, true);
    animation_count++;
    std::cout << "completed: " << key_map.recipe_path << std::endl;
    std::cout << "completed: " << animation_count << std::endl;
    Mat flicker_output(out_height, out_width, CV_8UC3, Scalar(0,0,0));
    imshow("window_name", flicker_output);
    waitKey(20);
    Mat blank_output(out_height, out_width, CV_8UC3, Scalar(255,0,0));
    char ch[200];
    sprintf(ch,"CH %d",animation_count);
    cv::putText(blank_output, ch, Point2f(40,80), FONT_HERSHEY_PLAIN, 5,  Scalar(250,255,250), 5);
    imshow("window_name", blank_output);
    waitKey(1);
}

// Freedom
// Beneath
// Kayne
// Secrets
// Year of Valor

int main( int argc, char** argv )
{

    namedWindow("window_name", CV_WINDOW_NORMAL);
    setWindowProperty("window_name", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    // default image
    std::shared_ptr<MacroFrameGenerator> default_gen = generatorFromOption("resources/recipes/eye_remosaicsartwork.yaml");
    for (int i = 0; i < gens_maps.size(); i++)
    {
        for (int j = 0; j < 255; j++)
        {
            gens_maps[i][j] = std::pair<std::shared_ptr<MacroFrameGenerator>, bool>(default_gen, false);
        }
    }

    // std::vector<KeyMap> key_maps = {
    //     // intro
    //     KeyMap("resources/recipes/eye_remosaicsartwork.yaml", ',', '~'),
    
    //     // Freedom
    //     KeyMap("resources/recipes/sunset1_explode.yaml", '1', '!'),
    //     KeyMap("resources/recipes/orbit_explode.yaml", '2', '@'),
    //     KeyMap("resources/recipes/shuttle_geom.yaml", '3', '#'),
    //     KeyMap("resources/recipes/longtrail_smoke1.yaml", '4', '$'),
    //     KeyMap("resources/recipes/crystal_geom.yaml", '5', '%'),
    //     KeyMap("resources/recipes/mithril_wave.yaml", '6', '^'),
    //     KeyMap("resources/recipes/wave_wave.yaml", '7', '&'),
    //     KeyMap("resources/recipes/puff_fastpaint.yaml", '8', '*'),
    
    //     // break
    //     KeyMap("resources/recipes/sunbulb.yaml", '9', '('),
    
    //     // Beneath
    //     KeyMap("resources/recipes/jelly_explode.yaml", '0', ')'),
    //     KeyMap("resources/recipes/shroom_geomflower.yaml", '-', '-'),
    //     KeyMap("resources/recipes/shroom_face2.yaml", '=', '+'),
    //     KeyMap("resources/recipes/whispflower_smoke1.yaml", 'q', 'Q'),
    //     KeyMap("resources/recipes/electric_lightbulb.yaml", 'w', 'W'),
    //     KeyMap("resources/recipes/explode_explode.yaml", 'e', 'E'),
    //     KeyMap("resources/recipes/fractalsmoke_smoke1.yaml", 'r', 'R'),
    //     KeyMap("resources/recipes/run_bloom1.yaml", 't', 'T'),
    //     KeyMap("resources/recipes/explode_explode.yaml", 'y', 'Y'),
    
    //     // break
    //     KeyMap("resources/recipes/yeezy_smoke1.yaml", 'u', 'U'),
    
    //     // kanye gifs
    //     KeyMap("resources/recipes/nosmiling_explode.yaml", 'i', 'I'),
    //     KeyMap("resources/recipes/kanyeheart_face2.yaml", 'o', 'O'),
    //     KeyMap("resources/recipes/headsmoke_wave.yaml", 'p', 'P'),
    //     KeyMap("resources/recipes/bart_geomflower.yaml", '[', '{'),
    //     KeyMap("resources/recipes/gridsunset_geomflower.yaml", ']', '}'),
    //     KeyMap("resources/recipes/cig_wave.yaml", '\\', '|'),
    //     KeyMap("resources/recipes/me_glasses_smoke1.yaml", 'a', 'A'),
    //     KeyMap("resources/recipes/kanyeheart_smoke1.yaml", 's', 'S'),
    
    //     // break
    //     KeyMap("resources/recipes/comet_lightbulb.yaml", 'd', 'D'),
    
    //     // Secrets
    //     KeyMap("resources/recipes/body_smoke1.yaml", 'f', 'F'),
    //     KeyMap("resources/recipes/cube_smoke1.yaml", 'g', 'G'),
    //     KeyMap("resources/recipes/pourskull_bloom1.yaml", 'h', 'H'),
    //     KeyMap("resources/recipes/skullexplode_lightbulb.yaml", 'j', 'J'),
    //     KeyMap("resources/recipes/burningman_lightbulb.yaml", 'k', 'K'),
    //     KeyMap("resources/recipes/burningman_explode.yaml", 'l', 'L'),
    //     KeyMap("resources/recipes/3dfractal_geomflower.yaml", ';', ':'),
    //     KeyMap("resources/recipes/mandelbrot_lightbulb.yaml", '\'', '\"'),
    //     KeyMap("resources/recipes/skeleton_face2.yaml", 'z', 'Z'),
    //     KeyMap("resources/recipes/nazi_face2.yaml", 'x', 'X'),
    
    //     // break
    //     KeyMap("resources/recipes/longtrail_lightbulb.yaml", 'c', 'C'),
    
    //     // Year of Valor
    //     KeyMap("resources/recipes/geomflower_remosaicsartwork.yaml", 'v', 'V'),
    //     KeyMap("resources/recipes/cubefractal_fastpaint.yaml", 'b', 'B'),
    //     KeyMap("resources/recipes/lake_lightbulb.yaml", 'n', 'N'),
    //     KeyMap("resources/recipes/monk_explode.yaml", 'm', 'M'),
    //     KeyMap("resources/recipes/mouthsmoke_smoke1.yaml", ',', '<'),
    //     KeyMap("resources/recipes/mouthsmoke1_smoke1.yaml", '.', '>'),
    
    //     // outro
    //     KeyMap("resources/recipes/water_lightbulb.yaml", '/', '?'),
    // };

    // int count = 0;
    // for (KeyMap& key_map : key_maps)
    // {
    //     std::cout << key_map.recipe_path << std::endl;
    //     placeGen(key_map);
    //     count++;
    //     std::cout << int(100*count/key_maps.size()) << "\%" << std::endl;
    // }

    // Chromo, freedom, year of valor, Kayne, beneath , secrets
    
    // TODO - Chromo
    placeGen(0, KeyMap("resources/recipes/tea_geom.yaml", 'q', 'Q'));
    placeGen(0, KeyMap("resources/recipes/tunnel_explode.yaml", 'w', 'W'));
    placeGen(0, KeyMap("resources/recipes/rhombus_face2.yaml", 'e', 'E'));
    placeGen(0, KeyMap("resources/recipes/sin_smoke1.yaml", 'r', 'R'));
    placeGen(0, KeyMap("resources/recipes/spin_geom.yaml", 't', 'T'));
    placeGen(0, KeyMap("resources/recipes/facefold_explode.yaml", 'y', 'Y'));
    placeGen(0, KeyMap("resources/recipes/psych_geom.yaml", 'u', 'U'));
    placeGen(0, KeyMap("resources/recipes/faceswirl_bloom1.yaml", 'i', 'I'));
    placeGen(0, KeyMap("resources/recipes/thinker_lightbulb.yaml", 'o', 'O'));

    // Freedom
    placeGen(1, KeyMap("resources/recipes/sunset1_explode.yaml", 'q', 'Q'));
    placeGen(1, KeyMap("resources/recipes/orbit_explode.yaml", 'w', 'W'));
    placeGen(1, KeyMap("resources/recipes/shuttle_geom.yaml", 'e', 'E'));
    placeGen(1, KeyMap("resources/recipes/longtrail_smoke1.yaml", 'r', 'R'));
    placeGen(1, KeyMap("resources/recipes/crystal_geom.yaml", 't', 'T'));
    placeGen(1, KeyMap("resources/recipes/mithril_wave.yaml", 'y', 'Y'));
    placeGen(1, KeyMap("resources/recipes/wave_wave.yaml", 'u', 'U'));
    placeGen(1, KeyMap("resources/recipes/puff_fastpaint.yaml", 'i', 'I'));
    
    // Year of Valor
    placeGen(2, KeyMap("resources/recipes/longtrail_lightbulb.yaml", 'q', 'Q'));
    placeGen(2, KeyMap("resources/recipes/geomflower_remosaicsartwork.yaml", 'w', 'W'));
    placeGen(2, KeyMap("resources/recipes/cubefractal_fastpaint.yaml", 'e', 'E'));
    placeGen(2, KeyMap("resources/recipes/lake_lightbulb.yaml", 'r', 'R'));
    placeGen(2, KeyMap("resources/recipes/monk_explode.yaml", 't', 'T'));
    placeGen(2, KeyMap("resources/recipes/mouthsmoke_smoke1.yaml", 'y', 'Y'));
    placeGen(2, KeyMap("resources/recipes/mouthsmoke1_smoke1.yaml", 'u', 'U'));

    // Kanye
    placeGen(3, KeyMap("resources/recipes/yeezy_smoke1.yaml", 'q', 'Q'));
    placeGen(3, KeyMap("resources/recipes/nosmiling_explode.yaml", 'w', 'W'));
    placeGen(3, KeyMap("resources/recipes/kanyeheart_face2.yaml", 'e', 'E'));
    placeGen(3, KeyMap("resources/recipes/headsmoke_wave.yaml", 'r', 'R'));
    placeGen(3, KeyMap("resources/recipes/bart_geomflower.yaml", 't', 'T'));
    placeGen(3, KeyMap("resources/recipes/gridsunset_geomflower.yaml", 'y', 'Y'));
    placeGen(3, KeyMap("resources/recipes/cig_wave.yaml", 'u', 'U'));
    placeGen(3, KeyMap("resources/recipes/me_glasses_smoke1.yaml", 'i', 'I'));
    placeGen(3, KeyMap("resources/recipes/kanyeheart_smoke1.yaml", 'o', 'O'));

    // Beneath
    placeGen(4, KeyMap("resources/recipes/sunbulb.yaml", 'q', 'Q'));
    placeGen(4, KeyMap("resources/recipes/jelly_explode.yaml", 'w', 'W'));
    placeGen(4, KeyMap("resources/recipes/shroom_geomflower.yaml", 'e', 'E'));
    placeGen(4, KeyMap("resources/recipes/shroom_face2.yaml", 'r', 'R'));
    placeGen(4, KeyMap("resources/recipes/whispflower_smoke1.yaml", 't', 'T'));
    placeGen(4, KeyMap("resources/recipes/electric_lightbulb.yaml", 'y', 'Y'));
    placeGen(4, KeyMap("resources/recipes/explode_explode.yaml", 'u', 'U'));
    placeGen(4, KeyMap("resources/recipes/fractalsmoke_smoke1.yaml", 'i', 'I'));
    placeGen(4, KeyMap("resources/recipes/run_bloom1.yaml", 'o', 'O'));
    placeGen(4, KeyMap("resources/recipes/explode_explode.yaml", 'p', 'P'));
    
    // Secrets
    placeGen(5, KeyMap("resources/recipes/comet_lightbulb.yaml", 'q', 'Q'));
    placeGen(5, KeyMap("resources/recipes/body_smoke1.yaml", 'w', 'W'));
    placeGen(5, KeyMap("resources/recipes/cube_smoke1.yaml", 'e', 'E'));
    placeGen(5, KeyMap("resources/recipes/pourskull_bloom1.yaml", 'r', 'R'));
    placeGen(5, KeyMap("resources/recipes/skullexplode_lightbulb.yaml", 't', 'T'));
    placeGen(5, KeyMap("resources/recipes/burningman_lightbulb.yaml", 'y', 'Y'));
    placeGen(5, KeyMap("resources/recipes/burningman_explode.yaml", 'u', 'U'));
    placeGen(5, KeyMap("resources/recipes/3dfractal_geomflower.yaml", 'i', 'I'));
    placeGen(5, KeyMap("resources/recipes/mandelbrot_lightbulb.yaml", 'o', 'O'));
    placeGen(5, KeyMap("resources/recipes/skeleton_face2.yaml", 'p', 'P'));
    placeGen(5, KeyMap("resources/recipes/nazi_face2.yaml", '[', '{'));


    int key = '`';
    int song = 0;

    while (true)
    {
        int new_key = gens_maps[song][key].first->loop(gens_maps[song][key].second, out_height, out_width);
        if (new_key < 0)
        {
            new_key = 0;
        }
        if (new_key > 255)
        {
            new_key = 255;
        }
        switch (new_key)
        {
            case '1': song = 0;
                      key = 'q';
                      break;
            case '2': song = 1;
                      key = 'q';
                      break;
            case '3': song = 2;
                      key = 'q';
                      break;
            case '4': song = 3;
                      key = 'q';
                      break;
            case '5': song = 4;
                      key = 'q';
                      break;
            case '6': song = 5;
                      key = 'q';
                      break;
            default: key = new_key;
        }
    }

    return 0;
}