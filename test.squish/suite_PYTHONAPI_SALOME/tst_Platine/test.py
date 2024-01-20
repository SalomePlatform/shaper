# Copyright (C) 2014-2024  CEA, EDF
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

def set_parameter_value(name, value):
    openContextMenu(waitForObject(":Parameters.%s = _QModelIndex" % name), 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    mouseClick(waitForObject(":Parameter_ExpressionEditor"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":Parameter_ExpressionEditor"), "<Ctrl+A>")
    type(waitForObject(":Parameter_ExpressionEditor"), value)
    type(waitForObject(":Parameter_ExpressionEditor"), "<Return>")
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

def main():
    # [project] SHAPER
    # [Scenario] 'Platine' model created by PythonAPI
    # [Topic] PythonAPI functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [section] Application start
    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Activate Shaper module and create new file
    activate_SHAPER()

    # [step] In Python console type: import examples.Platine
    type(waitForObject(":Python Console_PyConsole_EnhEditor"), "import examples.Platine")
    type(waitForObject(":Python Console_PyConsole_EnhEditor"), "<Return>")

    # [step] Check that the model is correct [vp VP_PLATINE]
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    test.vp("VP_PLATINE")

    # [step] Check that the object browser contains all objects from the model [vp VP_TREE]
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (3)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Parameters (3)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Constructions (3)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Constructions (3)", -10, 10, 0, Qt.LeftButton)
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Bodies (1)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Bodies (1)", -10, 10, 0, Qt.LeftButton)
    # Tree is under modification, so it can not be checked by a picture
    # Moreover a mask for a text should be provided to check it on different platforms
    #test.vp("VP_TREE")

    # [step] Change parameter values: L = 64, E = 5, P = 80
    set_parameter_value("L", 64)
    set_parameter_value("E", 5)
    set_parameter_value("P", 80)
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    mouseMove(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 320, 240)
    test.vp("VP_PLATINE_E")

    # [step] Change parameter values: L = 90, E = 20, P = 80
    set_parameter_value("L", 90)
    set_parameter_value("E", 20)
    set_parameter_value("P", 80)
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    mouseMove(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 320, 240)
    test.vp("VP_PLATINE_L")

    # [step] Change parameter values: L = 64, E = 20, P = 120
    set_parameter_value("L", 64)
    set_parameter_value("E", 20)
    set_parameter_value("P", 120)
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))
    mouseMove(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 320, 240)
    test.vp("VP_PLATINE_P")

    # [step] Close application
    close_application()
