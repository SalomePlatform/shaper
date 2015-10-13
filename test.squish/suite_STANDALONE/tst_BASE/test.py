def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
        
    #[step] Create parameter a=100
    parameter_create("a", "100")
    #[step] Create new parts
    part_create()
    #[step] Create sketch with circle center=(0,0) radius="a"
    sketch_create(help_points("XY_plane"), lambda: circle_create(0, 0, "a"))
    #[step] Crete extrusion with the circle and to_size="a"
    points = [(313, 336)] # circle
    extrusion_feature(points, "a")
    #[step] Close application
    close_application()