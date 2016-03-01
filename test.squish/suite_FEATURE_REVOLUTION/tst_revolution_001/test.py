def main():
    # [project] SHAPER
    # [Scenario] Revolution_001
    # [Topic] 'Revolution by angles' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_revolution_001.hdf'
    open(DATA_PATH + "/for_revolution_001.hdf")

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

    # [step] Execute revolution by angles
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "Revolution"))

    # [step] Click '+OY' button
    clickButton(waitForObject(":SALOME*.+OY_QToolButton"))

    # [step] Fit all [vp +OY]
    fit_all()
    test.vp("+OY")

    # [step] Select sketch for revolution
    mouseClick(waitForObject(":Revolution.Select a sketch face_QListWidget"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 431, 180, 0, Qt.LeftButton)
    # [step] Select vector
    mouseClick(waitForObject(":Revolution_QLineEdit"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 311, 282, 0, Qt.LeftButton)

    # [step] Fit all [vp AFTER_SELECTION]
    fit_all()
    test.vp("AFTER_SELECTION")

    # [step] Click 'Reset view' toolbar button
    clickButton(waitForObject(":SALOME*.Reset_QToolButton_2"))
    mouseClick(waitForObject(":SALOME*_OCCViewer_ViewPort3d"), 10, 450, 0, Qt.LeftButton)  # close tool bar extension bar

    test.vp("AFTER_SELECTION_RESET")

    # [step] Input angle 'To'
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), 90)

    # [step] Input angle 'From'
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), 120)
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), "<Return>")

    # [check] Check that preview is updated [cp AFTER_ANGLE_90_120]
    fit_all()
    test.vp("AFTER_ANGLE_90_120")

    # [step] Clear the angle 'To'
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.to_angle_ModuleBase_ParamSpinBox"), "<Delete>")

    # [step] Input new angle 'From': 360
    mouseClick(waitForObject(":Revolution.qt_spinbox_lineedit_QLineEdit_2"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), 360)
    type(waitForObject(":Revolution.from_angle_ModuleBase_ParamSpinBox"), "<Return>")

    # [step] Apply the feature
    clickButton(waitForObject(":Revolution.property_panel_ok_QToolButton"))

    # [check] Check that operation has been executed successfully [cp AFTER_ANGLE_360]
    fit_all()
    test.vp("AFTER_ANGLE_360")

    # [step] Close application without saving
    close_application()
