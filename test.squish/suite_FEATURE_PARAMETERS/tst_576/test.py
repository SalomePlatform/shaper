# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#[tested functionality] 1. Add parameter 2. Rename parameter

def main():
    source(findFile("scripts", "common.py"))

    startApplication("salome_run.sh")

    activate_SHAPER()

    clickButton(waitForObject(":SALOME*.Parameter_QToolButton"))
    type(waitForObject(":Parameter_QLineEdit"), "a")
    type(waitForObject(":Parameter_ExpressionEditor"), "3")
    type(waitForObject(":Parameter_ExpressionEditor"), "<Return>")
    clickButton(waitForObject(":Parameter.property_panel_ok_QToolButton"))

    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).a = 3", 10, 10, 0)
    activateItem(waitForObjectItem(":_QMenu", "Rename"))
    type(waitForObject(":_QExpandingLineEdit"), "b")
    type(waitForObject(":_QExpandingLineEdit"), "<Return>")

    test.compare(waitForObjectItem(":Object browser_XGUI_DataTree", "Parameters (1).b = 3").text, "b = 3")

    openItemContextMenu(waitForObject(":Object browser_XGUI_DataTree"), "Parameters (1).b = 3", 111, 5, 0)
    activateItem(waitForObjectItem(":_QMenu", "Edit..."))
    waitFor("object.exists(':Parameter_QLineEdit')", 20000)
    test.compare(str(findObject(":Parameter_QLineEdit").text), "b")
    waitFor("object.exists(':Parameter_ExpressionEditor')", 20000)
    test.compare(str(findObject(":Parameter_ExpressionEditor").plainText), "3")

    close_application()
