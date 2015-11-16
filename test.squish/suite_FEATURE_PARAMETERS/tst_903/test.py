#[tested functionality] Add parameter
#[tested functionality] Use wrong parameter name
#[tested functionality] Error notification in header
#[tested functionality] Error notification in widget

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
   
    activate_newgeom()

    #[step] Click menu Part->Parameter    
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Parameter"))
    mouseClick(waitForObject(":Parameter_QLineEdit"), 79, 8, 0, Qt.LeftButton)
    #[step] Check that feature ToolTip is: Model_FeatureValidator: Attribute "expression" is not initialized.
    waitFor("object.exists(':Parameter_QFrame')", 20000)
    test.compare(str(findObject(":Parameter_QFrame").toolTip), "Model_FeatureValidator: Attribute \"expression\" is not initialized.")
    #[step] Check that name tooltip is: Errors:\nvariable - Parameters_VariableValidator: Incorrect variable name.
    waitFor("object.exists(':Parameter_QLineEdit')", 20000)
    test.compare(str(findObject(":Parameter_QLineEdit").toolTip), "Errors:\nvariable - Parameters_VariableValidator: Incorrect variable name.")
    #[step] Check that expression tooltip is: Errors:\nexpression - Parameters_ExpressionValidator: Expression is empty.
    waitFor("object.exists(':Parameter_ExpressionEditor')", 20000)
    test.compare(str(findObject(":Parameter_ExpressionEditor").toolTip), "Errors:\nexpression - Parameters_ExpressionValidator: Expression is empty.")

    #[step] Enter variable name 'a'    
    type(waitForObject(":Parameter_QLineEdit"), "a")
    mouseClick(waitForObject(":Parameter_ExpressionEditor"), 97, 31, 0, Qt.LeftButton)
    
    #[step] Enter variable expression '100+b'
    type(waitForObject(":Parameter_ExpressionEditor"), "100+b")
    type(waitForObject(":Parameter_ExpressionEditor"), "<Return>")
    
    #[step] Check that expression tooltip is: Errors:\nexpression - Parameters_ExpressionValidator: name 'b' is not defined
    waitFor("object.exists(':Parameter_ExpressionEditor')", 20000)
    test.compare(str(findObject(":Parameter_ExpressionEditor").toolTip), "Errors:\nexpression - Parameters_ExpressionValidator: name 'b' is not defined")
    #[step] Check that result message is: Error: unexpected EOF while parsing (<string>, line 0)
    waitFor("object.exists(':Parameter.Result_QLabel')", 20000)
    test.compare(str(findObject(":Parameter.Result_QLabel").text), "Error: name 'b' is not defined")

    #[step] Check that feature ToolTip is: expression - Parameters_ExpressionValidator: name 'b' is not defined
    waitFor("object.exists(':Parameter_QFrame')", 20000)
    test.compare(str(findObject(":Parameter_QFrame").toolTip), "expression - Parameters_ExpressionValidator: name 'b' is not defined")

    #[step] Check that apply button is red [vp VP_APPLY_RED]
    test.vp("VP_APPLY_RED")

    close_application()
