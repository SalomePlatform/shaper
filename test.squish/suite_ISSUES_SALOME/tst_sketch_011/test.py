
def main():
    #[project] NewGEOM
    #[Scenario] Sketch_010
    #[Topic] 'Horizontal' functionality
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
    open(DATA_PATH + "/for_sketch_010.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    # [step] Make second line horizontal
    horizontal((465, 173))
    
    #[step] Fit all
    fit_all()
    
    #[check] Check that second line became horizontal
    test.vp("VP1")
    
    #[step] Uncheck 'Show constraint' check-box
    clickButton(waitForObject(":Sketch.Show constraints_QCheckBox"))
    
    #[check] Check that icon disappears
    test.vp("VP2")
 
    #[step] Close application without saving
    close_application()
