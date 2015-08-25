def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
   
    activate_newgeom()
    
    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "a")
    type(waitForObject(":Parameter_ExpressionEditor"), "3")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))
    
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).a = 3", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Rename"))
    type(waitForObject(":_QExpandingLineEdit"), "b")
    type(waitForObject(":_QExpandingLineEdit"), "<Return>")
    
    test.compare(waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).b = 3").text, "b = 3")
    
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).b = 3", 111, 5, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    waitFor("object.exists(':Parameter_QLineEdit')", 20000)
    test.compare(str(findObject(":Parameter_QLineEdit").text), "b")
    waitFor("object.exists(':Parameter_ExpressionEditor')", 20000)
    test.compare(str(findObject(":Parameter_ExpressionEditor").plainText), "3")

    close_application()
