def main():
    # [project] SHAPER
    # [Scenario] Revolution_005
    # [Topic] 'RevolutionFuse by planes and offsets' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_revolution_005.hdf'
    open(DATA_PATH + "/for_revolution_005.hdf")

    # [step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all [vp INIT]
    fit_all()
    test.vp("INIT")

    # [step] Execute RevolutionFuse operation
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "RevolutionFuse"))

    # [step] Select sketch face
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 287, 236, 0, Qt.LeftButton)

    # [step] Click "Set plane view" in property panel [vp PLANE]
    clickButton(waitForObject(":Sketcher plane.Set plane view_QPushButton"))
    test.vp("PLANE")

    # [step] Create 2 circles on the plane [vp CIRCLES]
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Circle"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 400, 340, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 400, 370, 0, Qt.LeftButton)
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Circle"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 400, 130, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 400, 150, 0, Qt.LeftButton)
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    test.vp("CIRCLES")

    # [step] Approve sketching
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    # [step] Select axis for revolution
    mouseClick(waitForObject(":Revolution_QLineEdit_2"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 651, 219, 0, Qt.LeftButton)

    # [step] Click 'Reset view' toolbar button [vp RESET]
    clickButton(waitForObject(":SALOME*.Reset_QToolButton_2"))
    mouseClick(waitForObject(":SALOME*_OCCViewer_ViewPort3d"), 10, 450, 0, Qt.LeftButton)  # close tool bar extension bar

    fit_all()
    test.vp("RESET")

    # [step] Select revolution method 'By planes'
    snooze(1)
    clickButton(waitForObject(":Revolution_QToolButton_3"))

    # [step] Select face 'To' (biggest one)
    mouseClick(waitForObject(":To_QLineEdit_3"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 463, 171, 0, Qt.LeftButton)

    # [step] Fit all

    # [check] Check that preview is updated [vp PLANE_TO]
    fit_all()
    test.vp("PLANE_TO")

    # [step] Define offset for face 'To'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_3"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox_3"), 30)
    type(waitForObject(":To.to_offset_ModuleBase_ParamSpinBox_3"), "<Return>")

    # [check] Check that preview is updated [vp PLANE_TO]
    fit_all()
    test.vp("PLANE_TO_30")

    # [step] Select face 'From' (smallest one)
    mouseClick(waitForObject(":From_QLineEdit_3"), 10, 10, 0, Qt.LeftButton)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonPress, 504, 252, Qt.LeftButton, 1, 0)
    sendEvent("QMouseEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), QEvent.MouseButtonRelease, 504, 252, Qt.LeftButton, 0, 0)

    # [check] Check that preview is updated [vp PLANE_FROM]
    fit_all()
    test.vp("PLANE_FROM")

    # [step] Define offset for face 'From'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_4"), 38, 4, 0, Qt.LeftButton)
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox_3"), 45)
    type(waitForObject(":From.from_offset_ModuleBase_ParamSpinBox_3"), "<Return>")

    # [check] Check that preview is updated [vp PLANE_FROM_45]
    fit_all()
    test.vp("PLANE_FROM_45")

    # [step] Apply the feature
    clickButton(waitForObject(":RevolutionFuse.property_panel_ok_QToolButton"))

    # [step] Fit all
    fit_all()
    test.vp("REVOLUTION_FUSE")

    # [step] Close application without saving
    close_application()
