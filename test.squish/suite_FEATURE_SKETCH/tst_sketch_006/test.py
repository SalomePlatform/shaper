
def main():
    #[project] SHAPER
    #[Scenario] Sketch_006
    #[Topic] Parallel lines
    #[Tested functionality] 'Parallel' constraint
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_006.hdf'
    open(DATA_PATH + "/for_sketch_006.hdf")
    
    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.SHAPER_QToolButton"))
    
    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 51, 7, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 51, 7, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Edit Sketch_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1", 54, 11, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Sketch\\_1", 54, 11, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' view button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    #[step] Make lines parallel
    parallel((77, 174), (580, 284))

    #[check] Check that lines are parallel
    test.vp("VP1")
    #[step] Select 'Parallel' icon near the line
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 103, 255, 0, Qt.LeftButton)
    #[check] Check that Input panel 'Parallel' appears, there are the names of lines in text boxes
    waitFor("object.exists(':Parallel.First line_QLineEdit')", 20000)
    test.compare(str(findObject(":Parallel.First line_QLineEdit").text), "SketchLine_1")
    waitFor("object.exists(':Parallel.Second line_QLineEdit')", 20000)
    test.compare(str(findObject(":Parallel.Second line_QLineEdit").text), "SketchLine_2")
    #[check] Check that lines are selected and highlighted in viewer
    test.vp("VP3")
    #[step] Confirm 'Parallel' operation
    clickButton(waitForObject(":Parallel.property_panel_ok_QToolButton"))
    #[step] Confirm edition of the sketch
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    #[step] Close application
    close_application()
