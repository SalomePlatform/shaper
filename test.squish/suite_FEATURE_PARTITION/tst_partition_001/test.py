def main():
    #[project] NewGEOM
    #[Scenario] Partition_001
    #[Topic] 'Partition' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_partition_001.hdf'
    open(DATA_PATH + "/for_partition_001.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Fit all
    test.vp("VP_AFTER_LOAD")
    fit_all()
    test.vp("VP_FITALL")
    partition([(291, 188)], (178, 205))
    test.vp("VP_PARTITION_1")
    partition([(275, 173), (255, 323)], (272, 94))
    test.vp("VP_PARTITION_2")
    
    #[step] Hide one of parts obtained in the viewer: select it, call context menu - Hide
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 291, 198, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 291, 198, 0, Qt.RightButton)
    activateItem(waitForObjectItem(":_QtxMenu", "Hide"))
    
    #[check] Check that partition has been executed successfully
    test.vp("VP_HIDE")

    # [step] Close application without saving
    close_application()
