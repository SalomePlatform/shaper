def main():
    # [project] NewGEOM
    # [Scenario] Extrusion_003
    # [Topic] 'Extrusion_cut' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_extrusion_001.hdf'
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

    # [step] Features - ExtrusionCut
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "ExtrusionCut"))

    # [step] Select the upper face for sketching
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 243, 171, 0, Qt.LeftButton)

    # [step] Click "Set plane view" in property panel [vp PLANE]
    clickButton(waitForObject(":Sketcher plane.Set plane view_QPushButton"))
    test.vp("PLANE")

    # [step] Draw closing contour, consisting from 4 lines
    lines_create_in_view([(83, 152), (85, 346), (669, 344), (669, 162), (83, 152)])

    # [check] Check that lines have been executed successfully [cp LINES]
    fit_all()
    test.vp("LINES")

    # [step] Ok
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    # [step] Define value of 'To size' parameter of extrusionCut, set it equal to 50
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), 50)
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "<Return>")

    # [check] Check that preview updated [cp PREVIEW_TO_SIZE]
    fit_all()
    test.vp("PREVIEW_TO_SIZE")

    # [step] Define value of 'From size' parameter of extrusionCut, set it equal to 50
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_2"), 50)
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_2"), "<Return>")

    # [check] Check that preview updated [cp PREVIEW_FROM_SIZE]
    fit_all()
    test.vp("PREVIEW_FROM_SIZE")

    # [step] Ok
    clickButton(waitForObject(":ExtrusionCut.property_panel_ok_QToolButton"))

    # [step] Click 'Reset view' toolbar button
    clickButton(waitForObject(":SALOME*.Reset_QToolButton_2"))
    mouseClick(waitForObject(":SALOME*_OCCViewer_ViewPort3d"), 10, 450, 0, Qt.LeftButton)  # close tool bar extension bar

    # [check] Check that extrusionCut operation has been executed successfully [vp EXTRUSION_CUT]
    test.vp("EXTRUSION_CUT")

    # [step] Close application without saving
    close_application()
