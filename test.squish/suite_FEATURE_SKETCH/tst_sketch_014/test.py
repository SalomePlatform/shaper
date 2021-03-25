# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

def main():
    #[project] SHAPER
    #[Scenario] Sketch_014
    #[Topic] 'Mirror' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_014.hdf'
    open(DATA_PATH + "/for_sketch_014.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    #[step] Sketch - Mirror
    mouseClick(waitForObjectItem(":SALOME*_QMenuBar", "Sketch"))
    mouseClick(waitForObjectItem(":Sketch_QMenu", "Mirror"))

    #[step] Select long line as mirror line
    mouseClick(waitForObject(":Mirror.Mirror line_QLineEdit"), 122, 5, 0, Qt.LeftButton)
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 256, 193, 0, Qt.LeftButton)

    #[step] Select other objects as objects for mirror
    mouseClick(waitForObject(":Mirror.Segments:_QListWidget"), 147, 69, 0, Qt.LeftButton)
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 45, 277, 689, 219, 1, Qt.LeftButton)

    #[step] Ok
    clickButton(waitForObject(":Mirror.property_panel_ok_QToolButton"))

    #[step] Click Fit all button
    fit_all()

    #[check] Check that mirroring has been executed successfully
    test.vp("VP1")

    # [step] Close application without saving
    close_application()
