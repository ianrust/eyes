import os

macros = [
    "resources/gifs/sunset",
    "resources/gifs/jelly",
    "resources/gifs/water",
    "resources/gifs/shroom",
    "resources/gifs/lake",
    "resources/gifs/electric",
    "resources/gifs/cubefractal",
    "resources/gifs/3dfractal",
    "resources/gifs/fractalsmoke",
    "resources/gifs/mandelbrot",
    "resources/gifs/shuttle",
    "resources/gifs/gridsunset",
    "resources/gifs/sunset1",
    "resources/gifs/cig",
    "resources/gifs/whispflower",
    "resources/gifs/smoketriangle",
    "resources/gifs/skullsmoke",
    "resources/gifs/skullexplode",
    "resources/gifs/skeleton",
    "resources/gifs/run",
    "resources/gifs/puff",
    "resources/gifs/pourskull",
    "resources/gifs/orbit",
    "resources/gifs/muppet",
    "resources/gifs/mouthsmoke1",
    "resources/gifs/mouthsmoke",
    "resources/gifs/monk",
    "resources/gifs/meteor",
    "resources/gifs/longtrail",
    "resources/gifs/layersmoke",
    "resources/gifs/hinden2",
    "resources/gifs/headsmoke",
    "resources/gifs/geomflower",
    "resources/gifs/explode",
    "resources/gifs/esplode",
    "resources/gifs/comet",
    "resources/gifs/burningman",
    "resources/gifs/bloom1",
    "resources/gifs/bloom",
    "resources/gifs/wave",
    "resources/gifs/eye/eye000.png",
    "resources/gifs/eye",
    "resources/gifs/nosmiling",
    "resources/gifs/kanyeheart",
    "resources/gifs/yeezy",
    "resources/gifs/bart",
    "resources/gifs/zoomeye",
    "resources/gifs/cube",
    "resources/gifs/crystal",
    "resources/gifs/circle",
    "resources/gifs/brokenheart",
    "resources/gifs/body",
    "resources/gifs/nazi",
    "resources/gifs/mithril",
    "resources/gifs/gridsunset",
    "resources/gifs/headsmoke",
    "resources/static/me_glasses.jpg",
]

micros = [
    "resources/gifs/explode",
    "resources/gifs/bloom1",
    "resources/gifs/lightbulb",
    "resources/gifs/geomflower",
    "resources/gifs/wave",
    "resources/gifs/swirl",
    "resources/remosaicsartwork",
    "resources/gifs/face2",
    "resources/gifs/smoke1",
    "resources/gifs/fastpaint",
    "resources/gifs/geom",
]

for macro in macros:
    for micro in micros:
        macro_name = macro.replace(".png","").replace(".jpg","").split("/")[-1]
        micro_name = micro.split("/")[-1]
        fn = "/Users/ian/Documents/DellBkp/eyes/resources/recipes/%s_%s.yaml" % (macro_name, micro_name)
        if os.path.exists(fn):
            pass
        else:
            t = "static" if ("png" in macro or "jpg" in macro) else "gif"
            text = ("mode : \"%s\"\n"
                    "macro_path : \"%s\"\n"
                    "macro_height : 140\n"
                    "micro_path : \"%s\"\n"
                    "micro_height : 15\n"
                    "fps: 30\n"
                    "grey_scale: 2.0") % (t, macro, micro)
            with open(fn, 'w') as f:
                print fn
                f.write(text)