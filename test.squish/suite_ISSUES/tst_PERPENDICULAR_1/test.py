def sketch():
    line_create((145, 205), (305, 118))
    line_create((221, 60), (195, 322))
    
    clickButton(waitForObject(":OpenParts*.Fit all_QToolButton"))
    
    perpendicular_create((323, 193),(246, 326))
    test.vp("VP1")

def main():
    source(findFile("scripts", "common.py"))
    
    startApplication("GeomApp")
    set_defaults()
    
    sketch_create(help_points("XY_plane"), lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
