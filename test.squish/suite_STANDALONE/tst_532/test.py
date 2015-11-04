def sketch():
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 230, 140, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 128, 399, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 307, 317, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 473, 347, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 230, 140, 0, Qt.LeftButton)

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("linux_run.sh")
    set_defaults()
    
    sketch_create(help_points("XY_plane"), lambda: sketch())
    
    part_create()

    extrusion_feature([(266, 251)], 10) # on the sketch

    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Construction")
    clickButton(waitForObject(":Basic.Plane_AppElements_Button"))
    type(waitForObject(":OpenParts*_AppElements_ViewPort"), "<Control>")
    mouseDrag(waitForObject(":OpenParts*_AppElements_ViewPort"), 353, 364, -37, -171, 67108866, Qt.RightButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 274, 316, 0, Qt.LeftButton) # inner left face
    clickButton(waitForObject(":Plane.property_panel_ok_QToolButton"))

    test.vp("VP_EXTRUSION")
    
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Boolean.Partition_AppElements_Button"))
    mouseClick(waitForObject(":Partition.Main objects_QListWidget"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 227, 263, 0, Qt.LeftButton) # extrusion object
    mouseClick(waitForObject(":Partition.Tool object_QListWidget"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), 212, 344, 0, Qt.LeftButton) # construction plane
    clickButton(waitForObject(":Partition.property_panel_ok_QToolButton"))
    
    test.vp("VP_PARTITION")
