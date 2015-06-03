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
    
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
def circle_create(x, y, radius):
    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))
    type(waitForObject(":Center.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.X:_ModuleBase_ParamSpinBox"), x)
    type(waitForObject(":Center.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.Y:_ModuleBase_ParamSpinBox"), y)
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), radius)
    clickButton(waitForObject(":Circle.property_panel_ok_QToolButton"))
    
def extrusion_feature(points, to_size):
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.Extrusion_AppElements_Button"))    

    mouseClick(waitForObject(":Extrusion.Select a sketch face_QListWidget"), 10, 10, 0, Qt.LeftButton)
    for point in points:
        mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)
    
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), to_size)

    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))
