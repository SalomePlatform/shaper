def sketch():
    #[step] Click 'Create sketch' toolbar button
    #[step] Select XY plane for sketching
    #[step] Create random closed contour
    line_create_in_view((101, 183),(265, 107))
    line_create_in_view((265, 107), (520, 192))
    line_create_in_view((520, 192), (443, 411))
    closing_line_create_in_view((443, 411), (101, 183))
    
    #[step] Create one more closed contour inside previous, not parallel to it
    line_create_in_view((219, 198),(269, 166))
    line_create_in_view((269, 166), (451, 230))
    line_create_in_view((451, 230), (410, 334))
    closing_line_create_in_view((410, 334), (219, 199))
    
    #[check] Check that lines are not parallel and there are no icons near lines
    test.vp("VP1")
    #[step] Make according lines parallel
    parallel((394, 151), (361, 199))
    parallel((201, 137), (244, 182))
    parallel((245, 278), (292, 249))
    parallel((430, 286), (482, 299))
    # [check] Check that lines are parallel and there are icons near lines
    test.vp("VP2")

def main():
    # [project] NewGEOM
    #[Scenario] Sketch_002
    #[Topic] Parallel objects
    #[Tested functionality]Parallelism between main lines
    
    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Activate NewGeom
    #[step]Click 'New' button
    activate_newgeom()
    #[section] Creation of main lines and make them parallel
    
    sketch_create(help_points("XY_plane"), lambda: sketch())
    #[step] Save study
    save("RESULTS_PATH + sketch_002")
    if object.exists(":Warning.Yes_QPushButton"):
        clickButton(waitForObject(":Warning.Yes_QPushButton"))
    ##[step] Close application
    sendEvent("QCloseEvent", waitForObject(":SALOME*_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Shutdown servers_QCheckBox"))
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
