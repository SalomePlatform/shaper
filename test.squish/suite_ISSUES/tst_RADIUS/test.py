def sketch():
    circle_border = (229, 290)
    circle_create_in_view((334, 237), circle_border)
    radius_create(circle_border, (148, 97), 150)
    
    # check
    circle_border = (392, 179)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), circle_border[0], circle_border[1], 0, Qt.LeftButton)
    waitFor("object.exists(':Circle.CircleRadius_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox").text), "150")

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("GeomApp")

    point = (446, 297) # one of the construction planes
    sketch_create(point, lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
    clickButton(waitForObject(":Save current file.Discard_QPushButton"))
