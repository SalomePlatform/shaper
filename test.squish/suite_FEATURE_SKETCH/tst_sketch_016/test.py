
def main():
    #[project] SHAPER
    #[Scenario] Sketch_016
    #[Topic] 'Translation' functionality
    #[Tested functionality] 
    #[Summary description]
    #[Expected results]
    #[General comments]
    
    source(findFile("scripts", "common.py"))
    
    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    
    #[step] Open 'for_sketch_016.hdf'
    open(DATA_PATH + "/for_sketch_016.hdf")
    
    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.SHAPER_QToolButton"))
    
    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    
    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    
    #[step] Sketch - Translation
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    activateItem(waitForObjectItem(":Sketch_QMenu", "Translation"))
    mouseClick(waitForObject(":Translation.Segments:_QListWidget"), 115, 35, 0, Qt.LeftButton)

    #[step] Select all objects for translation
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Shift>")
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 90, 130, 33554432, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 440, 132, 33554432, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 573, 199, 33554432, Qt.LeftButton)
    
    #[step] Click in viewer to define start point
    mouseClick(waitForObject(":Start point.qt_spinbox_lineedit_QLineEdit"), 53, 7, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 112, 453, 0, Qt.LeftButton)
    
    #[step] Click in viewer to define end point, for this zoom out preview    
    mouseClick(waitForObject(":End point.qt_spinbox_lineedit_QLineEdit"), 154, 9, 0, Qt.LeftButton)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 434, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 434, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 434, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 434, -120, 0, 2)
    sendEvent("QWheelEvent", waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 434, -120, 0, 2)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 426, 256, 0, Qt.LeftButton)
    
    #[step] Define the number of copies
    mouseClick(waitForObject(":Translation.qt_spinbox_lineedit_QLineEdit"), 41, 9, 0, Qt.LeftButton)
    type(waitForObject(":Translation.MultiTranslationCopies_QSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Translation.MultiTranslationCopies_QSpinBox"), 3)


    #[step] Fit all 
    fit_all()
    
    #[check] that preview is updated
    test.vp("VP1")
    
    #[step] Ok
    clickButton(waitForObject(":Translation.property_panel_ok_QToolButton"))
    
    #[check] Check that translation has been executed successfully
    test.vp("VP2")
    
    # [step] Close application without saving
    close_application()
