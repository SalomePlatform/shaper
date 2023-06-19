# Copyright (C) 2014-2023  CEA, EDF
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

def sketch():
    circle_create_in_view((302, 221), (436, 287))

def main():
    #[project] SHAPER
    #[Scenario] Sketch_003
    #[Topic]
    #[Tested functionality] Saving and opening studies
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()
    #[section] Creation of 3D model
    #[step] Activate SHAPER
    #[step]Click 'New' button

    activate_SHAPER()
    #[step] Create new part
    part_create()
    #[step] Create sketch, consisting from one circle
    sketch_create(help_points("XZ_plane"), lambda: sketch())

    #[step] Execute extrusion. Use parameters to size = 100 and from size = 50
    extrusion((182, 140), 100, 50)

    #[step] Rotate obtained model
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 425, 422, 20, -23, 67108865, Qt.LeftButton)
    type(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), "<Control>")
    mouseDrag(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 446, 411, -13, -137, 67108866, Qt.RightButton)
    test.vp("VP1")

    #[section] Save file
    #[step] File - Save as
    #[step] 'Save as' dialog appears
    #[step] Type name of file
    #[step] Click 'Save' button
    save("RESULTS_PATH + sketch_003")
    if object.exists(":Warning.Yes_QPushButton"):
        clickButton(waitForObject(":Warning.Yes_QPushButton"))

    #[step] Close document
    clickButton(waitForObject(":SALOME*.Close_QToolButton"))

    #[step] File - Open
    #[step} Open just saved file
    open("RESULTS_PATH + sketch_003.hdf")
    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 50, 11, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 50, 11, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    #[check] Check that file was loaded correct
    test.vp("VP2")
    #[step] Close application
    close_application()
