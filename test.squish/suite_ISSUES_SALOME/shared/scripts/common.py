import os

testSettings.logScreenshotOnError = True
testSettings.logScreenshotOnFail = True
#RESULTS_PATH = "/dn48/newgeom/eso/sources/test.squish/shared/testresults/"
DATA_PATH = os.getenv('TEST_DATA_DIR')

g_points = {"XY_plane": (332, 250), "XZ_plane": (355, 207)} # one of the construction planes
def help_points(name):
    return g_points[name] 

def set_defaults():
    waitForObject(":SALOME*_STD_TabDesktop").resize(1024, 768)
    
def activate_newgeom():
    clickButton(waitForObject(":SALOME*.NewGeom_QToolButton"))
    clickButton(waitForObject(":Activate module.New_QPushButton"))
    
def close_application():
    sendEvent("QCloseEvent", waitForObject(":SALOME*_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Shutdown servers_QCheckBox"))
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
    clickButton(waitForObject(":Close active study.Close w/o saving_QPushButton"))

def close_application_wo_saving():
    sendEvent("QCloseEvent", waitForObject(":SALOME*_STD_TabDesktop"))
    clickButton(waitForObject(":Exit.Ok_QPushButton"))
    clickButton(waitForObject(":Close active study.Close w/o saving_QPushButton"))
    
def part_create():
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "New part"))
    
def sketch_create(point, actions):
    clickButton(waitForObject(":SALOME*.Sketch_QToolButton"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    
    actions()
    
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    
def point_create_in_view(point, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Point"))
    
    if aux==1:    
        clickButton(waitForObject(":Point.Auxiliary_QCheckBox"))
    
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    clickButton(waitForObject(":Point.property_panel_cancel_QToolButton"))
    
def point_create(point, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Point"))  
               
    if aux==1:    
        clickButton(waitForObject(":Point.Auxiliary_QCheckBox"))

    type(waitForObject(":Point.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.X _ModuleBase_ParamSpinBox"), point[0])

    type(waitForObject(":Point.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.Y _ModuleBase_ParamSpinBox"), point[1])

    clickButton(waitForObject(":Point.property_panel_ok_QToolButton"))
    
def line_create_in_view(start_point, end_point, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Line"))
    
    if aux==1:       
        clickButton(waitForObject(":Line.Auxiliary_QCheckBox"))
    
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), start_point[0], start_point[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), end_point[0], end_point[1], 0, Qt.LeftButton)
    
    clickButton(waitForObject(":Line.property_panel_cancel_QToolButton"))

def closing_line_create_in_view(start_point, end_point, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Line"))
    
    if aux==1:       
        clickButton(waitForObject(":Line.Auxiliary_QCheckBox"))
    
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), start_point[0], start_point[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), end_point[0], end_point[1], 0, Qt.LeftButton)
        
def line_create(start_point, end_point, aux=0): #Set aux=1 to create auxiliary line
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Line"))
    
    if aux==1:       
        clickButton(waitForObject(":Line.Auxiliary_QCheckBox"))        
        
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox"), start_point[0])
    
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox"),  start_point[1])
    
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox"), end_point[0])
    
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox"), end_point[1])

    clickButton(waitForObject(":Line.property_panel_ok_QToolButton"))
        
def circle_create_in_view(center, radius, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Circle"))
    
    if aux==1:
        clickButton(waitForObject(":Circle.Auxiliary_QCheckBox"))
        
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), center[0], center[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), radius[0], radius[1], 0, Qt.LeftButton)

    clickButton(waitForObject(":Circle.property_panel_cancel_QToolButton"))
    
def circle_create(center, radius, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Circle"))
    
    if aux==1:
        clickButton(waitForObject(":Circle.Auxiliary_QCheckBox"))
    
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox"), center[0])

    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox"), center[1])

    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), radius)

    clickButton(waitForObject(":Circle.property_panel_ok_QToolButton"))
    
def arc_create(center, start_point, end_point, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Arc"))
    
    if aux==1:
        clickButton(waitForObject(":Arc.Auxiliary_QCheckBox"))

    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Center.X _ModuleBase_ParamSpinBox_2"), center[0])
    
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Center.Y _ModuleBase_ParamSpinBox_2"), center[1])
    
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Start point.X _ModuleBase_ParamSpinBox_2"), start_point[0])
    
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y _ModuleBase_ParamSpinBox_2"), start_point[1])

    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":End point.X _ModuleBase_ParamSpinBox_2"), end_point[0])
    
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":End point.Y _ModuleBase_ParamSpinBox_2"), end_point[1])
    
    clickButton(waitForObject(":Arc.property_panel_ok_QToolButton"))    
       
def arc_create_in_view(center, start_point, end_point, aux=0):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Arc"))
               
    if aux==1:
        clickButton(waitForObject(":Arc.Auxiliary_QCheckBox"))
        
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), center[0], center[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), start_point[0], start_point[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), end_point[0], end_point[1], 0, Qt.LeftButton)
    
    clickButton(waitForObject(":Arc.property_panel_cancel_QToolButton"))
    
def save(filename):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "File"))
    mouseClick(waitForObjectItem(":_QMenu", "Save As..."))
    
    type(waitForObject(":fileNameEdit_QLineEdit"), filename)
    clickButton(waitForObject(":Save File.Save_QPushButton"))
    
def extrusion(point, to_size, from_size):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Features"))
    mouseClick(waitForObjectItem(":_QMenu", "Extrusion"))
    
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), to_size)
    
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), from_size)
    
    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))
    
def open(filename):
    clickButton(waitForObject(":SALOME*.Open_QToolButton"))
    waitForObject(":fileNameEdit_QLineEdit_2").setText(filename)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "<Return>")

def point_fixe(point):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Fixed"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    clickButton(waitForObject(":Fixed.property_panel_cancel_QToolButton"))
    
    
def distance(start_point, end_point, annotation_point):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Distance"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), start_point[0], start_point[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), end_point[0], end_point[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), annotation_point[0], annotation_point[1], 0, Qt.LeftButton)
    
    #if length!=0:
        #type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
        #type(waitForObject(":_ModuleBase_ParamSpinBox"), length)
           
    clickButton(waitForObject(":Distance.property_panel_cancel_QToolButton"))
    
def change_distance(point, value):
    doubleClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    type(waitForObject(":Distance.ConstraintValue_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Distance.ConstraintValue_ModuleBase_ParamSpinBox"), value)
    clickButton(waitForObject(":Distance.property_panel_ok_QToolButton"))    
    
def parallel(point_1, point_2):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Parallel"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_2[0], point_2[1], 0, Qt.LeftButton)
    clickButton(waitForObject(":Parallel.property_panel_cancel_QToolButton_2"))
    
def perpendicular(point_1, point_2):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Perpendicular"))
    
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_2[0], point_2[1], 0, Qt.LeftButton) 
    
    clickButton(waitForObject(":Perpendicular.property_panel_cancel_QToolButton"))
    
def coincident(point_1, point_2):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Coincident"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_2[0], point_2[1], 0, Qt.LeftButton) 
    clickButton(waitForObject(":Coincident.property_panel_cancel_QToolButton"))
    
def equal(point_1, point_2):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Equal"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point_2[0], point_2[1], 0, Qt.LeftButton) 
    clickButton(waitForObject(":Equal.property_panel_cancel_QToolButton"))

def vertical(point):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Vertical"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    clickButton(waitForObject(":Vertical.property_panel_cancel_QToolButton"))
    
def horizontal(point):
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Horizontal"))
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), point[0], point[1], 0, Qt.LeftButton)
    clickButton(waitForObject(":Horizontal.property_panel_cancel_QToolButton"))
    
def fit_all():
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    

        


    
    
    
    
    
