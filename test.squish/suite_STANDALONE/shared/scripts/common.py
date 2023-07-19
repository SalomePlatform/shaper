# Copyright (C) 2015-2023  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
testSettings.logScreenshotOnError = True
testSettings.logScreenshotOnFail = True

g_points = {"XY_plane": (320, 320)} # one of the construction planes
def help_points(name):
    return g_points[name]

def set_defaults():
    waitForObject(":OpenParts*_AppElements_MainWindow").resize(1024, 768)

def close_application():
    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
    if object.exists(":Save current file.Discard_QPushButton"):
        clickButton(waitForObject(":Save current file.Discard_QPushButton"))

def parameter_create(name, expression):
    clickButton(waitForObject(":Parameters.Parameter_AppElements_Button"))
    type(waitForObject(":Parameter_QLineEdit"), name)
    type(waitForObject(":Parameter_ExpressionEditor"), expression)
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

def part_create():
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Part")
    clickButton(waitForObject(":Operations.New part_AppElements_Button"))

def sketch_create(point, actions):
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Sketch")
    clickButton(waitForObject(":Basic.Sketch_AppElements_Button"))
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)

    actions()

    clickButton(waitForObject(":Sketch.Ok_QToolButton"))

def point_create(point):
    clickButton(waitForObject(":Basic.Point_AppElements_Button"))

    type(waitForObject(":Point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.X:_ModuleBase_ParamSpinBox"), point[0])
    type(waitForObject(":Point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Point.Y:_ModuleBase_ParamSpinBox"), point[1])

    clickButton(waitForObject(":Point.property_panel_ok_QToolButton"))

def line_create(start_point, end_point):
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))

    type(waitForObject(":Start point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.X:_ModuleBase_ParamSpinBox"), start_point[0])
    type(waitForObject(":Start point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y:_ModuleBase_ParamSpinBox"), start_point[1])

    type(waitForObject(":End point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.X:_ModuleBase_ParamSpinBox"), end_point[0])
    type(waitForObject(":End point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.Y:_ModuleBase_ParamSpinBox"), end_point[1])

    clickButton(waitForObject(":Line.property_panel_ok_QToolButton"))

def line_create_in_view(start_point, end_point):
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), start_point[0], start_point[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), end_point[0], end_point[1], 0, Qt.LeftButton)

def auxiliary_line_create(start_point, end_point):
    clickButton(waitForObject(":Basic.Line_AppElements_Button"))
    clickButton(waitForObject(":Line.Auxiliary_QCheckBox"))

    type(waitForObject(":Start point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.X:_ModuleBase_ParamSpinBox"), start_point[0])
    type(waitForObject(":Start point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Start point.Y:_ModuleBase_ParamSpinBox"), start_point[1])

    type(waitForObject(":End point.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.X:_ModuleBase_ParamSpinBox"), end_point[0])
    type(waitForObject(":End point.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":End point.Y:_ModuleBase_ParamSpinBox"), end_point[1])

    #clickButton(waitForObject(":Line.property_panel_ok_QToolButton"))

def circle_create(x, y, radius):
    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))

    type(waitForObject(":Center.X:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.X:_ModuleBase_ParamSpinBox"), x)
    type(waitForObject(":Center.Y:_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Center.Y:_ModuleBase_ParamSpinBox"), y)

    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox"), radius)

    clickButton(waitForObject(":Circle.property_panel_ok_QToolButton"))

def circle_create_in_view(point_1, point_2):
    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_2[0], point_2[1], 0, Qt.LeftButton)

    clickButton(waitForObject(":Basic.Circle_AppElements_Button"))

def distance_create(point_1, point_2, annotaion_point, distance):
    clickButton(waitForObject(":Constraints.Distance_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_2[0], point_2[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), annotaion_point[0], annotaion_point[1], 0, Qt.LeftButton) # move annotation

    type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":_ModuleBase_ParamSpinBox"), distance)
    type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Keypad_Enter>")

    clickButton(waitForObject(":Constraints.Distance_AppElements_Button"))

def parallel_create(point_1, point_2):
    clickButton(waitForObject(":Constraints.Parallel_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_2[0], point_2[1], 0, Qt.LeftButton)

    clickButton(waitForObject(":Constraints.Parallel_AppElements_Button"))

def perpendicular_create(point_1, point_2):
    clickButton(waitForObject(":Constraints.Perpendicular_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_2[0], point_2[1], 0, Qt.LeftButton)

    clickButton(waitForObject(":Perpendicular.property_panel_cancel_QToolButton"))

def horizontal_create(point_1):
    clickButton(waitForObject(":Constraints.Horizontal_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)

    clickButton(waitForObject(":Constraints.Horizontal_AppElements_Button"))

def vertical_create(point_1):
    cclickButton(waitForObject(":Constraints.Vertical_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)

    clickButton(waitForObject(":Constraints.Vertical_AppElements_Button"))

def radius_create(point_1, point_2, radius):
    clickButton(waitForObject(":Constraints.Radius_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_1[0], point_1[1], 0, Qt.LeftButton)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point_2[0], point_2[1], 0, Qt.LeftButton)
    type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":_ModuleBase_ParamSpinBox"), radius)
    type(waitForObject(":_ModuleBase_ParamSpinBox"), "<Keypad_Enter>")

    clickButton(waitForObject(":Constraints.Radius_AppElements_Button"))

def part_create():
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Part")
    clickButton(waitForObject(":Operations.New part_AppElements_Button"))

def extrusion_feature(points, to_size=0, from_size=0):
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.Extrusion_AppElements_Button"))

    mouseClick(waitForObject(":Extrusion.Select a sketch face_QListWidget"), 10, 10, 0, Qt.LeftButton)
    for point in points:
        mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)

    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":to_size_ModuleBase_ParamSpinBox"), to_size)

    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox"), from_size)

    clickButton(waitForObject(":Extrusion.property_panel_ok_QToolButton"))

def extrusion_cut_by_sizes_feature(point, actions, to_size, from_size):
    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.ExtrusionCut_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)

    actions()

    clickButton(waitForObject(":Sketch.Ok_QToolButton"))

    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox"), to_size)

    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_2"), from_size)

    clickButton(waitForObject(":ExtrusionCut.property_panel_ok_QToolButton"))
    #clickButton(waitForObject(":Sketch.property_panel_cancel_QToolButton"))

def extrusion_fuse_by_sizes_feature(point, actions, to_size, from_size):

    clickTab(waitForObject(":General.qt_tabwidget_tabbar_QTabBar"), "Features")
    clickButton(waitForObject(":Extrusion.ExtrusionFuse_AppElements_Button"))

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), point[0], point[1], 0, Qt.LeftButton)

    actions()

    clickButton(waitForObject(":Sketch.Ok_QToolButton"))

    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.to_size_ModuleBase_ParamSpinBox_2"), to_size)

    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), "<Ctrl+A>")
    type(waitForObject(":Extrusion.from_size_ModuleBase_ParamSpinBox_3"), from_size)

    clickButton(waitForObject(":ExtrusionFuse.property_panel_ok_QToolButton"))
    #clickButton(waitForObject(":Sketch.property_panel_cancel_QToolButton"))


