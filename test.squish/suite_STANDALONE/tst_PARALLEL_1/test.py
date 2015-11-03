def sketch():
    line_create((244, 279), (226, 282))
    line_create((200, 290), (250, 300))

    #fit all
    clickButton(waitForObject(":OpenParts*.Fit all_QToolButton"))
    
    #parallel
    clickButton(waitForObject(":Constraints.Parallel_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 308, 255, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 417, 440, 0, Qt.LeftButton)
    clickButton(waitForObject(":Constraints.Parallel_AppElements_Button"))
    test.vp("VP1")

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    sketch_create(help_points("XY_plane"), lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
    
