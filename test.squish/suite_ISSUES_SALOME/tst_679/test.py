def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
    
    activate_newgeom()
    
    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "a")
    type(waitForObject(":Parameter_ExpressionEditor"), "3")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))
    
    clickButton(waitForObject(":SALOME*.ConstructionPoint_QToolButton"))
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "a")
    waitFor("object.exists(':Point.x_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Point.x_ModuleBase_ParamSpinBox").toolTip), "X coordinate")

    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "b")

    waitFor("object.exists(':Point_QFrame')", 20000)
    test.compare(str(findObject(":Point_QFrame").toolTip), "Attribute \"x\" invalidated by \"Model_AttributeValidator\" with error: name 'b' is not defined")
    waitFor("object.exists(':Point.x_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Point.x_ModuleBase_ParamSpinBox").toolTip), "X coordinate\nErrors:\nAttribute \"x\" invalidated by \"Model_AttributeValidator\" with error: name 'b' is not defined")

    close_application()
