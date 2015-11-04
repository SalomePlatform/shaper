def sketch():
    #[step] Click 'Create sketch' toolbar button
    #[step] Select XY plane for sketching
    #[step] Sketch - Point
    #[step] Create point via clicking inside viewer 
    point_create_in_view((340, 205))
    #[step] Check 'Auxiliary' check-box
    #[step] Create point via clicking inside viewer
    point_create_in_view((489, 380), 1) 
    #[step] Finish creation of points via viewer
    #[step] Create point via direct input of coordinates. Use coordinates (50, 60)
    point_create((80, 160))
    #[step] Check 'Auxiliary' check-box
    #[step] Create point via direct input of coordinates. Use coordinates (150, 60)
    point_create((250, 50), 1)
    #[step] Fix positions of 2 points
    point_fixe((340, 205))
    point_fixe((504, 227))
    #[check] Check that 2 points have 'anchor' icons near themselves
    test.vp("VP1")
    #[step] Define distance between fixed and free points
    distance((340, 205), (490, 381), (201, 320))
    # [step] Change distance set
    change_distance((275, 407), 200)
    # [step] Check that distance has been modified, free point moved
    test.vp("VP3")

def main():
    
    # [project] NewGEOM
    # [Scenario] Sketch_005
    #[Topic] Fixed point
    #[Tested functionality] 
    #[Summary description]
    #[Expected results] 
    
    source(findFile("scripts", "common.py"))
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Activate NewGeom
    #[step]Click 'New' button
    activate_newgeom()
    
    #[section] Creation of sketch and objects inside it
    #[step] Part - New part
    part_create()

    sketch_create(help_points("XY_plane"), lambda: sketch())

    #[step] Close application without saving
    close_application()
