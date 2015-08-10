def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("GeomApp")
    parameter_create("a", "100")
    part_create()
    point = (446, 297) # one of the construction planes
    sketch_create(point, lambda: circle_create(0, 0, "a"))
    points = [(489, 319)] # circle
    extrusion_feature(points, "a")
