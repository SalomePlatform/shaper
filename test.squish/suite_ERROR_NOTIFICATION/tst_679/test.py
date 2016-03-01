#[tested functionality] 1. Add parameter 2. Use wrong parameter name 3. Error notification in header 3. Error notification in widget

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
    
    activate_SHAPER()
    
    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "a")
    type(waitForObject(":Parameter_ExpressionEditor"), "3")
    type(waitForObject(":Parameter_ExpressionEditor"), "<Return>")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))
    
    clickButton(waitForObject(":SALOME*.ConstructionPoint_QToolButton"))
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "a")
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "<Return>")

    waitFor("object.exists(':Point.x_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Point.x_ModuleBase_ParamSpinBox").toolTip), "X coordinate")

    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "b")
    type(waitForObject(":Point.x_ModuleBase_ParamSpinBox"), "<Return>")

    waitFor("object.exists(':Point_QFrame')", 20000)
    test.compare(str(findObject(":Point_QFrame").toolTip), "ModelAPI_StateInvalidArgument")
    
    waitFor("object.exists(':Point.x_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Point.x_ModuleBase_ParamSpinBox").toolTip), "X coordinate\nErrors:\nx - Model_AttributeValidator: name 'b' is not defined")

    close_application()
