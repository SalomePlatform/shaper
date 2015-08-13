def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    parameter_create("a", "100")
    part_create()
    sketch_create(help_points("XY_plane"), lambda: circle_create(0, 0, "a"))

    points = [(313, 336)] # circle
    extrusion_feature(points, "a")
