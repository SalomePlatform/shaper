def parameter_create(name, expression):
    clickButton(waitForObject(":Parameters.Parameter_AppElements_Button"))
    type(waitForObject(":Parameter_QLineEdit"), name)
    type(waitForObject(":Parameter_ExpressionEditor"), expression)
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

def part_create():
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Part")
    clickButton(waitForObject(":Operations.New part_AppElements_Button"))
    
def sketch_create(point, actions):
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Sketch")
    clickButton(waitForObject(":Basic.Sketch_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)
    
    actions()
    
    clickButton(waitForObject(":Sketch.Ok_QToolButton"))
    
def point_create(point):
    clickButton(waitForObject(":Basic.Point_AppElements_Button"))

    type(waitForObject(":Point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.X:_ModuleBase_ParamSpinBox"), point[0])
    type(waitForObject(":Point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.Y:_ModuleBase_ParamSpinBox"), point[1])

    clickButton(waitForObject(":Point.property_panel_ok_QToolButton"))       
    
def line_create(start_point, end_point):
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))

    type(waitForObject(":Start point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.X:_ModuleBase_ParamSpinBox"), start_point[0])
    type(waitForObject(":Start point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y:_ModuleBase_ParamSpinBox"), start_point[1])

    type(waitForObject(":End point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.X:_ModuleBase_ParamSpinBox"), end_point[0])
    type(waitForObject(":End point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.Y:_ModuleBase_ParamSpinBox"), end_point[1])

    clickButton(waitForObject(":Line.property_panel_ok_QToolButton"))    
    
def circle_create(x, y, radius):
    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))
    
    type(waitForObject(":Center.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.X:_ModuleBase_ParamSpinBox"), x)
    type(waitForObject(":Center.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.Y:_ModuleBase_ParamSpinBox"), y)
    
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), radius)
    
    clickButton(waitForObject(":Circle.property_panel_ok_QToolButton"))
    
def distance_create(point_1, point_2, annotaion_point, distance):
    clickButton(waitForObject(":Constraints.Distance_AppElements_Button"))
    
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_2[0], point_2[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), annotaion_point[0], annotaion_point[1], 0, Qt.LeftButton) # move annotation

    type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":_ModuleBase_ParamSpinBox"), distance)
    type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Return>")

    clickButton(waitForObject(":Constraints.Distance_AppElements_Button"))    
    
def extrusion_feature(points, to_size):
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.Extrusion_AppElements_Button"))    

    mouseClick(waitForObject(":Extrusion.Select a sketch face_QListWidget"), 10, 10, 0, Qt.LeftButton)
    for point in points:
        mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)
    
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), to_size)

    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))
