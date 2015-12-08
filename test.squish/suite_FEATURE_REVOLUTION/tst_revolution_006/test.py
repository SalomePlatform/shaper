def main():
    # [project] NewGEOM
    # [Scenario] Revolution_006
    # [Topic] 'RevolutionCut by planes and offsets' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Open 'for_revolution_003.hdf'
    open(DATA_PATH + "/for_revolution_003.hdf")

    # [step] Activate NewGeom
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))

    # [step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 71, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 70, 9, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    # [step] Fit all [vp INIT]
    fit_all()
    test.vp("INIT")

    # TODO(spo): remove: workaround for the bug given RevolutionCut sketch created when select axis_object then apply is disabled
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1", 10, 10, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Sketch\\_1", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    # [step] Execute RevolutionFuse operation
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "RevolutionCut"))

    # [step] Select sketch face
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 287, 236, 0, Qt.LeftButton)

    # [step] Click "Set plane view" in property panel [vp PLANE]
    clickButton(waitForObject(":Sketcher plane.Set plane view_QPushButton"))
    test.vp("PLANE")

    # [step] Create 2 circles on the plane [vp CIRCLES]
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Circle"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 650, 350, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 690, 350, 0, Qt.LeftButton)
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Circle"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 650, 110, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 700, 110, 0, Qt.LeftButton)
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    test.vp("CIRCLES")

    # [step] Approve sketching
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))

    # [step] Select axis for revolution
    mouseClick(waitForObject(":Revolution_QLineEdit_3"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 750, 237, 0, Qt.LeftButton)

    # [step] Select method 'by planes and offsets'
    clickButton(waitForObject(":Revolution_QToolButton_5"))

    # [step] Select face 'To': front face
    mouseClick(waitForObject(":To_QLineEdit_4"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 334, 352, 0, Qt.LeftButton)

    # [step] Select face 'From': back face
    mouseClick(waitForObject(":From_QLineEdit_4"), 117, 10, 0, Qt.LeftButton)
    clickButton(waitForObject(":SALOME*.-OY_QToolButton"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 334, 352, 0, Qt.LeftButton)

    # [step] Apply the feature
    clickButton(waitForObject(":RevolutionCut.property_panel_ok_QToolButton"))

    # [step] Click 'Reset view' toolbar button
    clickButton(waitForObject(":SALOME*.Reset_QToolButton_2"))
    mouseClick(waitForObject(":SALOME*_OCCViewer_ViewPort3d"), 10, 450, 0, Qt.LeftButton)  # close tool bar extension bar

    # [check] Check that operation has been executed successfully
    fit_all()
    test.vp("REVOLUTION_CUT")

    # [step] Close application without saving
    close_application()
