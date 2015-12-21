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
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
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
