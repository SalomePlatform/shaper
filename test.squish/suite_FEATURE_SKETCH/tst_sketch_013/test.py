
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_013
    #[Topic] 'Radius' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_013.hdf'
    open(DATA_PATH + "/for_sketch_013.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    
    #[step] Define radius for circle
    radius((270, 252), (205, 224))
    
    #[step] Define radius for arc
    radius((718, 168), (677, 203))
    
    #[check] Check that constraints have been set successfully
    test.vp("VP1")
    
    #[step] Change radius of the circle to 150: select constraint and type '150' in 'Radius' field
    change_radius((192, 228), 150)
    
    #[step] Change radius of the arc to 100: select constraint and type '100' in 'Radius' field
    change_radius((674, 200), 100)
    
    #[step] Click 'Fit all' button
    fit_all()
    
    #[check] Check that radiuses have been changed successfully
    test.vp("VP2")

    #[step] Close application without saving
    close_application()
