def main():
    # [project] NewGEOM
    # [Scenario] Extrusion_004
    # [Topic] 'Extrusion_Fuse by sizes' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_extrusion_003.hdf'
    open(DATA_PATH + "/for_extrusion_003.hdf")

    # [step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all
    fit_all()
    test.vp("INIT")

    # [step] Features - ExtrusionFuse
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "ExtrusionFuse"))

    # [step] Select the upper face for sketching
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 243, 171, 0, Qt.LeftButton)

    # [step] Click "Set plane view" in property panel [vp PLANE]
    clickButton(waitForObject(":Sketcher plane.Set plane view_QPushButton"))
    test.vp("PLANE")

    # [step] Draw close contour, consisting from 4 lines
    lines_create_in_view([(83, 152), (85, 346), (669, 344), (669, 162), (83, 152)])

    # [check] Check that lines have been executed successfully [cp LINES]
    fit_all()
    test.vp("LINES")

    # [step] Ok
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    # [step] Define value of 'To size' parameter of ExtrusionFuse, set it equal to 100
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_3"), 100)
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_3"), "<Return>")

    # [check] Check that preview updated [cp PREVIEW_TO_SIZE]
    fit_all()
    test.vp("PREVIEW_TO_SIZE")

    # [step] Define value of 'From size' parameter of ExtrusionFuse, set it equal to 150
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), 150)
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "<Return>")

    # [check] Check that preview updated [cp PREVIEW_FROM_SIZE]
    fit_all()
    test.vp("PREVIEW_FROM_SIZE")

    # [step] Ok
    clickButton(waitForObject(":ExtrusionFuse.property_panel_ok_QToolButton"))

    # [step] Click 'Reset view' toolbar button
    need_expand = not object.exists(":SALOME*.Reset_QToolButton")
    if need_expand:
        clickButton(waitForObject(":SALOME*.qt_toolbar_ext_button_QToolBarExtension_3"))

    clickButton(waitForObject(":SALOME*.Reset_QToolButton"))

    if need_expand:
        clickButton(waitForObject(":SALOME*.qt_toolbar_ext_button_QToolBarExtension_3"))

    # [check] Check that extrusionCut operation has been executed successfully [vp EXTRUSION_FUSE]
    test.vp("EXTRUSION_FUSE")

    # [step] Close application without saving
    close_application()
