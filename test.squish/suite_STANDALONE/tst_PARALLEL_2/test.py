def sketch():
    #[step] Create lines [vp VP_SKETCH]
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 123, 417, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 175, 132, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 477, 63, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 739, 397, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 123, 417, 0, Qt.LeftButton)
    test.vp("VP_SKETCH")
    
    #[step] Create 2 lines parallel
    parallel_create((355, 91), (445, 407))
    # [check] Check that lines are parallel [vp VP_PARALLEL_1]
    test.vp("VP_PARALLEL_1")
    
    #[step] Create 2 other lines parallel
    parallel_create((155, 225), (589, 216))
    #[check] Check that lines are parallel [vp VP_PARALLEL_2]
    test.vp("VP_PARALLEL_2")

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    sketch_create(help_points("XY_plane"), lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
