def sketch():
    #[step] Click 'Create sketch' toolbar button
    #[step] Select XY plane for sketching
    #[step] Sketch - Point
    #[step] Create point via clicking inside viewer 
    point_create_in_view((329, 255))
    #[step] Check 'Auxiliary' check-box
    #[step] Create point via clicking inside viewer
    point_create_in_view((429, 300), 1) 
    #[step] Finish creation of points via viewer
    #[step] Create point via direct input of coordinates. Use coordinates (50, 60)
    point_create((50, 60))
    #[step] Check 'Auxiliary' check-box
    #[step] Create point via direct input of coordinates. Use coordinates (150, 60)
    point_create((150, 60), 1)
    #[step] Finish creation of points
    #[step] Sketch - Line
    #[step] Create line via direct input of coordinates. Use coordinates (0, 0) for start point and (100, 100) - for end point
    line_create((0,0), (100, 100))
    #[step] Check 'Auxiliary' check-box
    #[step] Create line via direct input of coordinates. Use coordinates (100, 100) for start point and (100, 100) - for end point
    line_create((100, 100), (200, 200), 1)
    #[step] Create line via clicking inside viewer
    line_create_in_view((400, 200), (600, 150))
    #[step] Check 'Auxiliary' check-box
    #[step] Create line via clicking inside viewer
    line_create_in_view((600, 150), (500, 450), 1)
    #[step] Finish creation of lines
    #[step] Sketch - Circle
    #[step] Create circle via direct input of coordinates. Use coordinates (0, 0) for center and 200 - for radius
    circle_create((0, 0), 200)
    #[step] Check 'Auxiliary' check-box
    #[step] Create circle via direct input of coordinates. Use coordinates (0, 0) for center and 300 - for radius
    circle_create((0, 0), 300, 1)
    #[step] Create circle via clicking inside viewer
    circle_create_in_view((400, 200), (260, 0))
    #[step] Check 'Auxiliary' check-box
    #[step] Create circle via clicking inside viewer
    circle_create_in_view((400, 200), (300, 100), 1)
    #[step] Finish creation of circles    
    #[step] Sketch - Arc
    #[step] Create arc via direct input of coordinates. Use coordinates (0, 0) for center, (100, 50) - for start point and (80, 90) - for end point
    arc_create((0, 0), (100, 50), (80, 90))
    #[step] Check 'Auxiliary' check-box
    #[step] Create arc via direct input of coordinates. Use coordinates (0, 0) for center, (200, 50) - for start point and (80, 90) - for end point
    arc_create((0, 0), (200, 50), (80, 90), 1)
    #[step] Create arc via clicking inside viewer
    arc_create_in_view((218, 183), (102, 135), (128, 271))
    #[step] Check 'Auxiliary' check-box
    #[step] Create arc via clicking inside viewer
    arc_create_in_view((203, 185), (51, 134), (104, 282), 1)
    #[step] CLick 'Fit all' button
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    #[check] All objects have been created. Check that objects of both types are shown (main and auxiliary)
    test.vp("VP1")
    #[step] Finish sketching
    
    
def main():
    source(findFile("scripts", "common.py"))
    
    # [project] NewGEOM
    # [Scenario] Sketch_001
    #[Topic] Creation of objects in sketch
    #[Tested functionality] Creation of objects during sketching by 2 ways: direct input of coordinates and clicking in viewer. Creation of auxiliary objects 
    #[Summary description]
    #[Expected results] 
    
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
    # clickButton(waitForObject(":Arc.Auxiliary_QCheckBox"))

    sketch_create(help_points("XY_plane"), lambda: sketch())
    #[check] Check that only main objects are shown 
    test.vp("VP2")
    #[step] Close application without saving
    close_application_wo_saving()


