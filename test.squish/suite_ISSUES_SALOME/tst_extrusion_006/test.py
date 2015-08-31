def main():
    #[project] NewGEOM
    #[Scenario] Extrusion_006
    #[Topic] 'Extrusion_cut by planes' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_extrusion_006.hdf'
    open(DATA_PATH + "/for_extrusion_006.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Not loaded Part\\_1", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Fit all
    fit_all()
    
    #[step] Features - ExtrusionCut
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "ExtrusionCut"))
    
    #[step] Select the upper face for sketching
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 243, 171, 0, Qt.LeftButton)
    
    #[step] Draw closing contour, consisting from 4 lines
    line_create_in_view((83, 152), (85, 346))
    line_create_in_view((85, 346), (669, 344))
    line_create_in_view((669, 344), (669, 162))
    closing_line_create_in_view((669, 162), (83, 152))
    
    #[step] Ok
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
    #[step] Select 'By planes aand offsets' method 
    clickButton(waitForObject(":Extrusion_QToolButton_3"))
    mouseClick(waitForObject(":To_QLineEdit_5"), 46, 15, 0, Qt.LeftButton)
    
    #[step] Select Plane_1 as face 'To'
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 35, 130, 0, Qt.LeftButton)
    
    #[step] Ok
    clickButton(waitForObject(":ExtrusionCut.property_panel_ok_QToolButton"))
    
    #[step] Click 'Reset view' toolbar button    
    clickButton(waitForObject(":SALOME*.Reset_QToolButton"))
    
    #[check] Check that extrusionCut operation has been executed successfully
    test.vp("VP1")
  
    # [step] Close application without saving
    close_application()
