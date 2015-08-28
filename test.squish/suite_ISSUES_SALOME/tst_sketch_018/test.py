
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_018
    #[Topic] 'Tangency' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_018.hdf'
    open(DATA_PATH + "/for_sketch_018.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    
    #[step] Make 2 arcs tangent
    tangent((142, 237), (55, 223))
    
    #[step] Make arc and line tangent
    tangent((580, 343), (605, 403))

    #[step] Click Fit all button
    fit_all()
    
    #[check] Check that tangency has been executed successfully
    test.vp("VP1")

    # [step] Close application without saving
    close_application()
