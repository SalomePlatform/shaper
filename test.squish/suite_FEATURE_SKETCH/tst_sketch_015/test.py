## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

def main():
    #[project] SHAPER
    #[Scenario] Sketch_015
    #[Topic] 'Fillet' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_015.hdf'
    open(DATA_PATH + "/for_sketch_015.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    #[step] Sketch - Fillet
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Fillet"))

    #[step] Select first line in viewer
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 99, 222, 0, Qt.LeftButton)

    #[step] Select first line in viewer
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 277, 479, 0, Qt.LeftButton)

    #[step] Input value equal to 50
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), "<Ctrl+A>")
    type(waitForObject(":Fillet.ConstraintValue_ModuleBase_ParamSpinBox"), 50)

    #[check] Check that preview is updated
    test.vp("VP1")

    #[step] Ok
    clickButton(waitForObject(":Fillet.property_panel_ok_QToolButton"))

    #[check] Check that fillet has been executed successfully
    test.vp("VP2")

    # [step] Close application without saving
    close_application()
