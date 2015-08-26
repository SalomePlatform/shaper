
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_007
    #[Topic] Perpendicular lines
    #[Tested functionality] 'Perpendicular' constraint
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_006.hdf'
    open(DATA_PATH + "/for_sketch_007.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1", 51, 7, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1", 51, 7, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Edit Sketch_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1", 54, 11, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Sketch\\_1", 54, 11, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' view button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    #[step] Make lines perpendicular    
    perpendicular((331, 155), (386, 169))
    
    #[step] Fit all
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    
    #[check] that lines are perpendicular
    test.vp("VP1")
    
    #[step] Select the 'perpendicular' icon near the line
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 372, 263, 0, Qt.LeftButton)
    
    #[check] Check that Input panel 'Perpendicular' appears, there are the names of lines in text boxes
    waitFor("object.exists(':Perpendicular.First line_QLineEdit')", 20000)
    test.compare(str(findObject(":Perpendicular.First line_QLineEdit").text), "SketchLine_1")
    waitFor("object.exists(':Perpendicular.Second line_QLineEdit')", 20000)
    test.compare(str(findObject(":Perpendicular.Second line_QLineEdit").text), "SketchLine_2")
    
    # [check] Check that lines are selected and highlighted in viewer
    test.vp("VP3")
    
    #[step] Confirm 'Perpendicular' operation
    clickButton(waitForObject(":Perpendicular.property_panel_ok_QToolButton"))
    
    #[step] Confirm edition of the sketch
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
    #[step] Close application
    clickButton(waitForObject(":SALOME*.Close_QToolButton"))
    clickButton(waitForObject(":Close active study.Close w/o saving_QPushButton"))
    sendEvent("QCloseEvent", waitForObject(":SALOME*_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
        
