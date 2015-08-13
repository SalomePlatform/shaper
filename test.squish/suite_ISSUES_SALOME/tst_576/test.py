def main():
    startApplication("salome_run.sh")
    
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    clickButton(waitForObject(":Activate module.New_QPushButton"))
    
    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "a")
    type(waitForObject(":Parameter_ExpressionEditor"), "3")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))
    
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).a = 3", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Rename"))
    type(waitForObject(":_QExpandingLineEdit"), "b")
    type(waitForObject(":_QExpandingLineEdit"), "<Return>")
    
    waitFor("object.exists(':Parameters (1).b = 3_QModelIndex')", 20000)
    test.compare(findObject(":Parameters (1).b = 3_QModelIndex").text, "b = 3")
    
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).b = 3", 111, 5, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    waitFor("object.exists(':Parameter_QLineEdit')", 20000)
    test.compare(str(findObject(":Parameter_QLineEdit").text), "b")
    waitFor("object.exists(':Parameter_ExpressionEditor')", 20000)
    test.compare(str(findObject(":Parameter_ExpressionEditor").plainText), "3")

    sendEvent("QCloseEvent", waitForObject(":SALOME*_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Shutdown servers_QCheckBox"))
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
    clickButton(waitForObject(":Close active study.Close w/o saving_QPushButton"))
   
