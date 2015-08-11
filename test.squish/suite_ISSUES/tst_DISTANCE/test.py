def sketch():
    line_create((0, 0), (100, 0))
    point_create((50, 50))

    point_1 = (536, 273) # point
    point_2 = (543, 299) # line 
    annotaion_point = (392, 291)
    distance_create(point_1,point_2, annotaion_point, 100)
    
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), annotaion_point[0], annotaion_point[1], 0, Qt.LeftButton)
    waitFor("object.exists(':Distance.First object_QLineEdit')", 20000)
    test.compare(str(findObject(":Distance.First object_QLineEdit").text), "SketchPoint_1/PointCoordindates")
    waitFor("object.exists(':Distance.Second object_QLineEdit')", 20000)
    test.compare(str(findObject(":Distance.Second object_QLineEdit").text), "SketchLine_1")
    waitFor("object.exists(':Distance.ConstraintValue_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Distance.ConstraintValue_ModuleBase_ParamSpinBox").text), "100")

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("GeomApp")
    set_defaults()
    
    sketch_create(points["XY_plane"], lambda: sketch())
