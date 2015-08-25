
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_009
    #[Topic] 'Equal' functionality 
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_006.hdf'
    open(DATA_PATH + "/for_sketch_009.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    # [check] Make lines equal
    equal((287, 392), (439, 322))
    
    # [step] Fit all
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    
    #[check] Check that lines are equal
    test.vp("VP1")
    
    #[step] Select 'Equal' icon in viewer near any objects
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 186, 0, Qt.LeftButton)
    
    #[check] Check that input panel 'Equal' appears
    test.vp("VP2")
    
    #[step] Close application without saving
    close_application()
