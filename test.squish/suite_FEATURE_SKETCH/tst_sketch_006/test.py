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
    #[Scenario] Sketch_006
    #[Topic] Parallel lines
    #[Tested functionality] 'Parallel' constraint
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_006.hdf'
    open(DATA_PATH + "/for_sketch_006.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Activate Part_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1 (Not loaded)", 51, 7, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1 (Not loaded)", 51, 7, 0)
    activateItem(waitForObjectItem(":_QMenu", "Activate"))

    #[step] Edit Sketch_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Part\\_1.Sketch\\_1", 54, 11, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Sketch\\_1", 54, 11, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' view button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))
    #[step] Make lines parallel
    parallel((77, 174), (580, 284))

    #[check] Check that lines are parallel
    test.vp("VP1")
    #[step] Select 'Parallel' icon near the line
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 103, 255, 0, Qt.LeftButton)
    #[check] Check that Input panel 'Parallel' appears, there are the names of lines in text boxes
    waitFor("object.exists(':Parallel.First line_QLineEdit')", 20000)
    test.compare(str(findObject(":Parallel.First line_QLineEdit").text), "SketchLine_1")
    waitFor("object.exists(':Parallel.Second line_QLineEdit')", 20000)
    test.compare(str(findObject(":Parallel.Second line_QLineEdit").text), "SketchLine_2")
    #[check] Check that lines are selected and highlighted in viewer
    test.vp("VP3")
    #[step] Confirm 'Parallel' operation
    clickButton(waitForObject(":Parallel.property_panel_ok_QToolButton"))
    #[step] Confirm edition of the sketch
    clickButton(waitForObject(":Sketch.property_panel_ok_QToolButton"))
    #[step] Close application
    close_application()
