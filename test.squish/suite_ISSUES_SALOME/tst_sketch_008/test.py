def sketch():
    #[step] Click 'Create sketch' toolbar button
    #[step] Select XY plane for sketching 
    #[step] Create point
    point_create_in_view((340, 205))
    #[step] Create line
    line_create_in_view((274, 367), (554, 189))
    #[step] Make them coincident
    coincident((340, 205), (390, 293))
    #[check] Check that objects are coincident
    test.vp("VP1")
    #[step] Uncheck 'Show constraints' check-box
    clickButton(waitForObject(":Sketch.Show constraints_QCheckBox"))
    #[check] Check that 'Coincident' icon disappears
    test.vp("VP2")

def main():
    
    # [project] NewGEOM
    # [Scenario] Sketch_008
    #[Topic] 'Coincident' functionality
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
