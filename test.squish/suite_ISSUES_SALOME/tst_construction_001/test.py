def main():
    #[project] NewGEOM
    #[Scenario] Construction_001
    #[Topic] 'Construction of point, axis and plane' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_construction_001.hdf'
    open(DATA_PATH + "/for_construction_001.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Not loaded Part\\_1", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Create plane basing on upper face of the box
    plane((283, 127), 100) 

    #[step] Fit all
    fit_all()

    #[check] Check that plane has been created
    test.vp("VP1")
    
    point(-50, -50, 125)
    point(50, 50, 125)
    
    axis((168, 138), (583, 137))

    test.vp("VP2")
    
    # [step] Close application without saving
    close_application()
