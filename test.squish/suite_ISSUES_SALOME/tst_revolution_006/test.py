def main():
    #[project] NewGEOM
    #[Scenario] Revolution_006
    #[Topic] 'RevolutionCut by planes and offsets' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_revolution_003.hdf'
    open(DATA_PATH + "/for_revolution_003.hdf")
    
    #[step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    
    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Not loaded Part\\_1", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Not loaded Part\\_1", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))
    
    #[step] Fit all
    fit_all()
    
    #[step] Execute RevolutionFuse operation
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "RevolutionCut"))
    
    #[step] Select sketch face
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 287, 236, 0, Qt.LeftButton)

    # [step] Create 2 circles
    circle_create_in_view((650, 364), (671, 392))
    circle_create_in_view((641, 114), (664, 141))
    
    #[step] Approve sketching
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
    # [step] Select axis for revolution
    mouseClick(waitForObject(":Revolution_QLineEdit_3"), 103, 11, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 750, 237, 0, Qt.LeftButton)
    
    #[step] Select method 'by planes and offsets'
    clickButton(waitForObject(":Revolution_QToolButton_5"))
    
    #[step] Select face 'To'
    mouseClick(waitForObject(":To_QLineEdit_4"), 82, 14, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 334, 352, 0, Qt.LeftButton)
    
    #[step] Select face 'From'
    mouseClick(waitForObject(":From_QLineEdit_4"), 117, 10, 0, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 507, 436, -473, -110, 67108866, Qt.RightButton)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonPress, 359, 293, Qt.LeftButton, 1, 0)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonRelease, 359, 293, Qt.LeftButton, 0, 0)
    
    #[step] Ok
    clickButton(waitForObject(":RevolutionCut.property_panel_ok_QToolButton"))
    
    #[step] Rotate the model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 85, 45, 376, 233, 67108866, Qt.RightButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 202, 241, 75, -75, 67108866, Qt.RightButton)

    #[step] Fit all
    fit_all()
    
    #[check] Check that operation has been executed successfully    
    test.vp("VP1")

    # [step] Close application without saving
    close_application()
