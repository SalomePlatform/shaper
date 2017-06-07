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
    #[Scenario] Sketch_009
    #[Topic] 'Equal' functionality
    #[Tested functionality]
    #[Summary description]
    #[Expected results]
    #[General comments]

    source(findFile("scripts", "common.py"))

    #[section] Application start
    #[step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    #[step] Open 'for_sketch_006.hdf'
    open(DATA_PATH + "/for_sketch_009.hdf")

    #[step] Activate SHAPER
    clickButton(waitForObject(":SALOME*.Shaper_QToolButton"))

    #[step] Edit 'Sketch_1'
    waitForObjectItem(":Object browser_XGUI_DataTree", "Sketch\\_1")
    clickItem(":Object browser_XGUI_DataTree", "Sketch\\_1", 43, 12, 0, Qt.LeftButton)
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Sketch\\_1", 43, 12, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))

    #[step] Click '+OZ' button
    clickButton(waitForObject(":SALOME*.+OZ_QToolButton"))

    # [check] Make lines equal
    equal((287, 392), (439, 322))

    # [step] Fit all
    clickButton(waitForObject(":SALOME*.Fit All_QToolButton"))

    #[check] Check that lines are equal
    test.vp("VP1")

    # [step] Select 'Equal' icon in viewer near any objects
    mouseClick(waitForObject(":SALOME*.3D View Operations_OCCViewer_ViewPort3d"), 451, 186, 0, Qt.LeftButton)

    #[check] Check that input panel 'Equal' appears
    waitFor("object.exists(':Equal.First object_QLineEdit')", 20000)
    test.compare(str(findObject(":Equal.First object_QLineEdit").text), "SketchLine_1")
    waitFor("object.exists(':Equal.Second object_QLineEdit')", 20000)
    test.compare(str(findObject(":Equal.Second object_QLineEdit").text), "SketchLine_2")

    #[step] Close application without saving
    close_application()
