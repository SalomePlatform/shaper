def sketch_circle():
    circle_create((0, 0), 300)

def sketch_lines():
    clickButton(waitForObject(":SALOME*.Line_QToolButton"))
    points = [(647, 441), (419, 214), (725, 140), (578, 243), (647, 441)]
    for point in points:
        mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("salome_run.sh")
    set_defaults()
    
    activate_newgeom()

    sketch_create(help_points("XY_plane"), lambda: sketch_circle())
    sketch_create(help_points("XY_plane"), lambda: sketch_lines())
    test.vp("VP_SKETCHES")
    
    part_create()

    #[step] Create circle sketch
    extrusion((299, 159), 10, 0)
    #[step] Create V-shape sketch
    extrusion((642, 176), 10, 0)

    test.vp("VP_EXTRUSIONS")

    #[step] Create plane: inner right for V-shape
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Construction"))
    activateItem(waitForObjectItem(":_QMenu", "Plane"))
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 305, 263, -2, -165, 67108866, Qt.RightButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 651, 229, 0, Qt.LeftButton)
    clickButton(waitForObject(":Plane.property_panel_ok_QToolButton"))
    test.vp("VP_INNER_RIGHT_PLANE")
    
    #[step] Create plane: outer right for V-shape
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Construction"))
    activateItem(waitForObjectItem(":_QMenu", "Plane"))
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 467, 171, -6, 319, 67108866, Qt.RightButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 641, 209, 0, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 515, 402, -7, -305, 67108866, Qt.RightButton)
    clickButton(waitForObject(":Plane.property_panel_ok_QToolButton"))
    test.vp("VP_OUTER_RIGHT_PLANE")
    
    #[step] Activate Partition feature
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    activateItem(waitForObjectItem(":_QMenu", "Partition"))
    #[step] Select circle and V-shape
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 304, 293, 0, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Shift>")
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 541, 265, 33554432, Qt.LeftButton)
    #[step] Select inner right and outer right V-shape planes
    mouseClick(waitForObject(":Partition.Tool object_QListWidget"), 201, 90, 0, Qt.LeftButton)
    type(waitForObject(":Partition.Tool object_QListWidget"), "<Shift>")
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 667, 217, 33554432, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 655, 270, 8, 353, 67108866, Qt.RightButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Shift>")
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 639, 210, 33554432, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 567, 436, -11, -351, 67108866, Qt.RightButton)

    #[step] Apply Partition
    clickButton(waitForObject(":Partition.property_panel_ok_QToolButton"))
    
    test.vp("VP_PARTITION")

    close_application()
