def main():
    # [project] NewGEOM
    # [Scenario] XAO-export
    # [Topic] 'Exchange' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Activate NewGeom
    activate_newgeom()

    # [step] Create new Part
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "New part"))

    # [step] Import file 'exchange/xao/Box_1.brep'
    export_file_path = os.path.join(DATA_PATH, 'exchange', 'xao', 'Box_1.brep');

    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Import"))
    mouseClick(waitForObject(":property_panel_dock...._QPushButton"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":property_panel_dock.fileTypeCombo_QComboBox", "BREP files (*\\.BREP *\\.BRP)"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), export_file_path)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "<Return>")
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    # [step] Check that the body is visible in the view [vp BODY]
    fit_all()
    test.vp("BODY")

    # [step] Create Group feature
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    activateItem(waitForObjectItem(":_QMenu", "Group"))

    # [step] Select type: Solids
    mouseClick(waitForObjectItem(":property_panel_dock.Type_QComboBox", "Solids"), 10, 10, 0, Qt.LeftButton)

    # [step] Select the imported box [vp SELECTED_SOLIDS]
    mouseClick(waitForObject(":property_panel_dock.Selected objects:_QListWidget"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 310, 147, 0, Qt.LeftButton)
    test.vp("SELECTED_SOLIDS")

    # [step] Apply the feature
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    # [step] Rename the Group_1 result to: boite_1
    # rename feature
#     waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Group\\_1")
#     clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Group\\_1", 10, 10, 0, Qt.LeftButton)
#     openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Group\\_1", 49, 7, 0)
#     activateItem(waitForObjectItem(":_QMenu", "Rename"))
#     type(waitForObject(":_QExpandingLineEdit_2"), "boite_1")
#     type(waitForObject(":_QExpandingLineEdit_2"), "<Return>")
    # rename result
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (1)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (1)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (1).Group\\_1")
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Groups (1).Group\\_1", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Rename"))
    type(waitForObject(":_QExpandingLineEdit"), "boite_1")
    type(waitForObject(":_QExpandingLineEdit"), "<Return>")

    # [step] Create Group feature
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    activateItem(waitForObjectItem(":_QMenu", "Group"))

    # [step] Select type: Faces
    mouseClick(waitForObjectItem(":property_panel_dock.Type_QComboBox", "Faces"), 10, 10, 0, Qt.LeftButton)

    # [step] Select the two faces [vp SELECTED_FACES]
    mouseClick(waitForObject(":property_panel_dock.Selected objects:_QListWidget"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 469, 292, 0, Qt.LeftButton)
    type(waitForObject(":property_panel_dock.Selected objects:_QListWidget"), "<Shift>")
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 306, 296, 33554432, Qt.LeftButton)
    test.vp("SELECTED_FACES")

    # [step] Apply the feature
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    # [step] Check that the new Group result name is: Group_2
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (2).Group\\_2")

    # [step] Create Export feature
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Export"))

    # [step] Select export type: XAO
    mouseClick(waitForObjectItem(":property_panel_dock_QComboBox", "XAO"), 10, 10, 0, Qt.LeftButton)

    # [step] Set the file name 'Box-export.xao'
    export_file_path = os.path.join(TMP_DIR, 'Box-export.xao')
    clickButton(waitForObject(":property_panel_dock...._QPushButton"))
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_3"), 64, 7, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), export_file_path)
    clickButton(waitForObject(":property_panel_dock.Save_QPushButton"))

    # [step] Set author: me
    type(waitForObject(":property_panel_dock.Author_QLineEdit"), "me")

    # [step] Set geometry name: mygeom
    type(waitForObject(":property_panel_dock.Geometry name_QLineEdit"), "mygeom")
    type(waitForObject(":property_panel_dock.Geometry name_QLineEdit"), "<Return>")

    # [step] Apply the feature
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))
    snooze(1)

    # [step] Check that the exported file exists: 'exchange/xao/results/Box-export.xao'
    example_file_path = os.path.join(DATA_PATH, 'exchange', 'xao-examples', 'Box-export.xao')
    files = (export_file_path, example_file_path)
    if filecmp.cmp(*files):
        test.passes("File comparison of '%s' and '%s' passed" % files)
    else:
        test.fail("Files '%s' and '%s' are not equal" % files)

    # [section] Import

    # [step] Delete Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1", 10, 10, 0, Qt.LeftButton)
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Remove part"))

    # [step] Create new Part
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "New part"))

    # [step] Create Import feature
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Import"))

    # [step] Select the exported file
    mouseClick(waitForObject(":property_panel_dock...._QPushButton"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":property_panel_dock.fileTypeCombo_QComboBox", "XAO files (*\\.XAO)"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), export_file_path)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "<Return>")

    # [step] Apply the feature
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    # [step] Check that the body is visible in the view [vp BODY]
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Bodies (1).mygeom\\_1", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Show only"))
    fit_all()
    test.vp("BODY_IMPORT")

    # [step] In Object Browser check that the Import feature name is "mygeom"
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.mygeom")

    # [step] In Object Browser check new body: mygeom_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Bodies (1).mygeom\\_1")
    # [step] In Object Browser check new groups: boite_1, Group_2
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (2).boite\\_1")
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (2).Group\\_2")

    # [step] Check the group boite_1 is a solid [vp GROUP_1]
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Groups (2).boite\\_1", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Show only"))
    fit_all()
    test.vp("GROUP_1")

    # [step] Check the group Group_2 is a list of 2 faces [vp GROUP_2]
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Groups (2).Group\\_2", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Show only"))
    fit_all()
    test.vp("GROUP_2")

    # [step] Close application without saving
    close_application()
