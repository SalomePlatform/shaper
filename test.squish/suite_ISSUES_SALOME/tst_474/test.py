def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
    
    create_new_document()
    
    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "aa")
    type(waitForObject(":Parameter_ExpressionEditor"), "4")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

    clickButton(waitForObject(":SALOME*.New part_QToolButton"))
    
    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "bb")
    type(waitForObject(":Parameter_ExpressionEditor"), "aa+1")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).aa = 4", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Delete"))
    waitFor("object.exists(':Delete features._QLabel')", 20000)
    test.compare(str(findObject(":Delete features._QLabel").text), "Selected features are used in the following features: bb.These features will be deleted.\nWould you like to continue?")
    clickButton(waitForObject(":Delete features.No_QPushButton"))
    
    close_application()
