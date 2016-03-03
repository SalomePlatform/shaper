# -*- coding: utf-8 -*-

# A quick introduction to implementing scripts for BDD tests:
#
# This file contains snippets of script code to be executed as the .feature
# file is processed. See the section 'Behaviour Driven Testing' in the 'API
# Reference Manual' chapter of the Squish manual for a comprehensive reference.
#
# The decorators Given/When/Then/Step can be used to associate a script snippet
# with a pattern which is matched against the steps being executed. Optional
# table/multi-line string arguments of the step are passed via a mandatory
# 'context' parameter:
#
#   @When("I enter the text")
#   def whenTextEntered(context):
#      <code here>
#
# The pattern is a plain string without the leading keyword, but a couple of
# placeholders including |any|, |word| and |integer| are supported which can be
# used to extract arbitrary, alphanumeric and integer values resp. from the
# pattern; the extracted values are passed as additional arguments:
#
#   @Then("I get |integer| different names")
#   def namesReceived(context, numNames):
#      <code here>
#
# Instead of using a string with placeholders, a regular expression can be
# specified. In that case, make sure to set the (optional) 'regexp' argument
# to True.

import __builtin__

@Given("Launch application in salome mode")
def step(context):
    startApplication("salome_run.sh")
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))
    clickButton(waitForObject(":Activate module.New_QPushButton"))

@When("Create parameter a=30")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Parameter"))
    mouseClick(waitForObject(":Parameter_QLineEdit"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Parameter_QLineEdit"), "a")
    mouseClick(waitForObject(":Parameter_ExpressionEditor"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Parameter_ExpressionEditor"), "30")
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

@When("Create part")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "New part"))

@When("Create parameter a=a+2")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Parameter"))
    mouseClick(waitForObject(":Parameter_QLineEdit"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Parameter_QLineEdit"), "a")
    mouseClick(waitForObject(":Parameter_ExpressionEditor"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Parameter_ExpressionEditor"), "a+2")
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

@Then("the parameter in the part should exist")
def step(context):
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (1)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (1)", -10, 10, 0, Qt.LeftButton)
    waitFor("object.exists(':Parameters (1).a = 32_QModelIndex')", 20000)
    test.compare(findObject(":Parameters (1).a = 32_QModelIndex").text, "a = 32")

@When("Create sketch")
def step(context):
    clickButton(waitForObject(":SALOME*.Sketch_QToolButton"))
    clickButton(waitForObject(":SALOME*.-OZ_QToolButton"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 247, 126, 0, Qt.LeftButton)
    clickButton(waitForObject(":Sketcher plane.Set plane view_QPushButton"))

@Then("Check that Point does not allow using expressions")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Point"))
    mouseClick(waitForObject(":Point.qt_spinbox_lineedit_QLineEdit"), 41, 6, 0, Qt.LeftButton)
    type(waitForObject(":Point.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.X _ModuleBase_ParamSpinBox"), "a")
    mouseClick(waitForObject(":Point.qt_spinbox_lineedit_QLineEdit_2"), 40, 9, 0, Qt.LeftButton)
    type(waitForObject(":Point.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.Y _ModuleBase_ParamSpinBox"), "a")
    waitFor("object.exists(':Point.qt_spinbox_lineedit_QLineEdit')", 20000)
    test.compare(str(findObject(":Point.qt_spinbox_lineedit_QLineEdit").text), "0")
    waitFor("object.exists(':Point.qt_spinbox_lineedit_QLineEdit_2')", 20000)
    test.compare(str(findObject(":Point.qt_spinbox_lineedit_QLineEdit_2").text), "0")
    clickButton(waitForObject(":Point.property_panel_cancel_QToolButton"))

@Then("Check that Line does not allow using expressions")
def step(context):
    clickButton(waitForObject(":SALOME*.Line_QToolButton"))
    mouseClick(waitForObject(":Start point.qt_spinbox_lineedit_QLineEdit_2"), 89, 9, 0, Qt.LeftButton)
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox"), "a")
    mouseClick(waitForObject(":Start point.qt_spinbox_lineedit_QLineEdit_3"), 72, 10, 0, Qt.LeftButton)
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox"), "a")
    mouseClick(waitForObject(":End point.qt_spinbox_lineedit_QLineEdit_2"), 62, 8, 0, Qt.LeftButton)
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox"), "a")
    mouseClick(waitForObject(":End point.qt_spinbox_lineedit_QLineEdit_3"), 59, 6, 0, Qt.LeftButton)
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox"), "a")
    waitFor("object.exists(':Start point.qt_spinbox_lineedit_QLineEdit_2')", 20000)
    test.compare(str(findObject(":Start point.qt_spinbox_lineedit_QLineEdit_2").text), "0")
    waitFor("object.exists(':Start point.qt_spinbox_lineedit_QLineEdit_3')", 20000)
    test.compare(str(findObject(":Start point.qt_spinbox_lineedit_QLineEdit_3").text), "0")
    waitFor("object.exists(':End point.qt_spinbox_lineedit_QLineEdit_2')", 20000)
    test.compare(str(findObject(":End point.qt_spinbox_lineedit_QLineEdit_2").text), "0")
    waitFor("object.exists(':End point.qt_spinbox_lineedit_QLineEdit_3')", 20000)
    test.compare(str(findObject(":End point.qt_spinbox_lineedit_QLineEdit_3").text), "0")
    clickButton(waitForObject(":Line.property_panel_cancel_QToolButton"))

@Then("Check that Circle does not allow using expressions")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Circle"))
    mouseClick(waitForObject(":Center.qt_spinbox_lineedit_QLineEdit"), 90, 4, 0, Qt.LeftButton)
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox"), "a")
    mouseClick(waitForObject(":Center.qt_spinbox_lineedit_QLineEdit_2"), 71, 8, 0, Qt.LeftButton)
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox"), "a")
    mouseClick(waitForObject(":Circle.qt_spinbox_lineedit_QLineEdit"), 58, 6, 0, Qt.LeftButton)
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), "a")
    waitFor("object.exists(':Center.qt_spinbox_lineedit_QLineEdit')", 20000)
    test.compare(str(findObject(":Center.qt_spinbox_lineedit_QLineEdit").text), "0")
    waitFor("object.exists(':Center.qt_spinbox_lineedit_QLineEdit_2')", 20000)
    test.compare(str(findObject(":Center.qt_spinbox_lineedit_QLineEdit_2").text), "0")
    waitFor("object.exists(':Circle.qt_spinbox_lineedit_QLineEdit')", 20000)
    test.compare(str(findObject(":Circle.qt_spinbox_lineedit_QLineEdit").text), "0")
    clickButton(waitForObject(":Circle.property_panel_cancel_QToolButton"))

@Then("Check that Arc does not allow using expressions")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Arc"))
    mouseClick(waitForObject(":Center.qt_spinbox_lineedit_QLineEdit_3"), 156, 6, 0, Qt.LeftButton)
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox_2"), "A")
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox_2"), "a")
    mouseClick(waitForObject(":Arc.Center_QGroupBox"), 135, 41, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Center.qt_spinbox_lineedit_QLineEdit_4"), 115, 3, 0, Qt.LeftButton)
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox_2"), "a")
    mouseClick(waitForObject(":Start point.qt_spinbox_lineedit_QLineEdit_4"), 103, 9, 0, Qt.LeftButton)
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox_2"), "a")
    mouseClick(waitForObject(":Start point.qt_spinbox_lineedit_QLineEdit_5"), 99, 8, 0, Qt.LeftButton)
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox_2"), "a")
    mouseClick(waitForObject(":End point.qt_spinbox_lineedit_QLineEdit_4"), 99, 1, 0, Qt.LeftButton)
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox_2"), "a")
    mouseClick(waitForObject(":End point.qt_spinbox_lineedit_QLineEdit_5"), 99, 1, 0, Qt.LeftButton)
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox_2"), "a")
    waitFor("object.exists(':Center.qt_spinbox_lineedit_QLineEdit_3')", 20000)
    test.compare(str(findObject(":Center.qt_spinbox_lineedit_QLineEdit_3").text), "0")
    waitFor("object.exists(':Center.qt_spinbox_lineedit_QLineEdit_4')", 20000)
    test.compare(str(findObject(":Center.qt_spinbox_lineedit_QLineEdit_4").text), "0")
    waitFor("object.exists(':Start point.qt_spinbox_lineedit_QLineEdit_4')", 20000)
    test.compare(str(findObject(":Start point.qt_spinbox_lineedit_QLineEdit_4").text), "0")
    waitFor("object.exists(':Start point.qt_spinbox_lineedit_QLineEdit_5')", 20000)
    test.compare(str(findObject(":Start point.qt_spinbox_lineedit_QLineEdit_5").text), "0")
    waitFor("object.exists(':End point.qt_spinbox_lineedit_QLineEdit_4')", 20000)
    test.compare(str(findObject(":End point.qt_spinbox_lineedit_QLineEdit_4").text), "0")
    waitFor("object.exists(':End point.qt_spinbox_lineedit_QLineEdit_5')", 20000)
    test.compare(str(findObject(":End point.qt_spinbox_lineedit_QLineEdit_5").text), "0")
    clickButton(waitForObject(":Arc.property_panel_cancel_QToolButton"))

@Then("Check that Fillet does not allow using expressions")
def step(context):
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Line"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 131, 282, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 209, 98, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 283, 275, 0, Qt.LeftButton)
    #TODO_1 clickButton(waitForObject(":Line.property_panel_cancel_QToolButton"))
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Fillet"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 211, 99, 0, Qt.LeftButton)
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "<Keypad_1>")
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "<Keypad_0>")
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "<Keypad_Enter>")
    waitFor("object.exists(':Fillet.qt_spinbox_lineedit_QLineEdit')", 20000)
    test.compare(str(findObject(":Fillet.qt_spinbox_lineedit_QLineEdit").text), "10")
    mouseClick(waitForObject(":Fillet.qt_spinbox_lineedit_QLineEdit"), 48, 6, 0, Qt.LeftButton)
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "a")
    waitFor("object.exists(':Fillet.qt_spinbox_lineedit_QLineEdit')", 20000)
    test.compare(str(findObject(":Fillet.qt_spinbox_lineedit_QLineEdit").text), "10")
    clickButton(waitForObject(":Fillet.property_panel_cancel_QToolButton"))
