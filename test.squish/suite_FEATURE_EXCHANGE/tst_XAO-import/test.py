# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    # [project] NewGEOM
    # [Scenario] XAO-import
    # [Topic] 'Exchange' functionality
    # [Tested functionality]
    # [Summary description]
    # [Expected results]
    # [General comments]

    source(findFile("scripts", "common.py"))

    # [step] Launch SALOME
    startApplication("salome_run.sh")

    set_defaults()

    # [step] Activate NewGeom
    activate_newgeom()

    # [step] Create new Part
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "New part"))

    # [step] Create Import feature
    activateItem(waitForObjectItem(":SALOME*_QMenuBar", "Part"))
    activateItem(waitForObjectItem(":Part_QMenu", "Import"))

    # [step] Select file 'exchange/xao/test.xao' and apply the feature
    file_path = os.path.join(DATA_PATH, 'exchange', 'xao', 'test.xao');
    mouseClick(waitForObject(":property_panel_dock...._QPushButton"), 10, 10, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":property_panel_dock.fileTypeCombo_QComboBox", "XAO files (*\\.XAO)"), 10, 10, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), file_path)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "<Return>")
    clickButton(waitForObject(":Boolean.property_panel_ok_QToolButton"))

    # [step] Check that the body is visible in the view [vp BODY]
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Bodies (1).mygeom\\_1", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Show only"))
    fit_all()
    test.vp("BODY")

    # [step] In Object Browser check that the Import feature name is "mygeom"
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.mygeom")

    # [step] In Object Browser check new body: mygeom_1
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Bodies (1).mygeom\\_1")
    # [step] In Object Browser check new groups: boite_1, Group_2
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (2).boite\\_1")
    waitForObjectItem(":Object browser_XGUI_DataTree", "Part\\_1.Groups (2).Group\\_2")

    # [step] Check the group boite_1 is a solid [vp GROUP_1]
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Groups (2).boite\\_1", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Show only"))
    fit_all()
    test.vp("GROUP_1")

    # [step] Check the group Group_2 is a list of 2 faces [vp GROUP_2]
    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Part\\_1.Groups (2).Group\\_2", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Show only"))
    fit_all()
    test.vp("GROUP_2")

    # [step] Close application without saving
    close_application()
