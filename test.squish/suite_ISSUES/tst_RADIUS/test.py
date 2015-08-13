def sketch():
    circle_center = (300, 400)
    circle_border = (500, 400)
    radius_end_point = (100, 100)
    circle_create_in_view(circle_center, circle_border)
    radius_create(circle_border, radius_end_point, 150)
    
    # check
    circle_border = (200, 400)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), circle_border[0], circle_border[1], 0, Qt.LeftButton)
    waitFor("object.exists(':Circle.CircleRadius_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox").text), "150")

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()

    sketch_create(help_points("XY_plane"), lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
    clickButton(waitForObject(":Save current file.Discard_QPushButton"))
