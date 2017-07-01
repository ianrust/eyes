import os

macros = [
    "resources/gifs/facefold",
    "resources/gifs/faceswirl",
    "resources/gifs/psych",
    "resources/gifs/rhombus",
    "resources/gifs/sin",
    "resources/gifs/spin",
    "resources/gifs/tea",
    "resources/gifs/thinker",
    "resources/gifs/tunnel",
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