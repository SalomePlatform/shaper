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

def sketch():
    circle_center = (300, 400)
    circle_border = (500, 400)
    radius_end_point = (100, 100)
    circle_create_in_view(circle_center, circle_border)
    radius_create(circle_border, radius_end_point, 150)

    # check
    circle_border = (200, 400)
    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), circle_border[0], circle_border[1], 0, Qt.LeftButton)
    waitFor("object.exists(':Circle.CircleRadius_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Circle.CircleRadius_ModuleBase_ParamSpinBox").text), "150")

def main():
    source(findFile("scripts", "common.py"))

    startApplication("linux_run.sh")
    set_defaults()

    sketch_create(help_points("XY_plane"), lambda: sketch())

    sendEvent("QCloseEvent", waitForObject(":OpenParts*_AppElements_MainWindow"))
    clickButton(waitForObject(":Save current file.Discard_QPushButton"))
