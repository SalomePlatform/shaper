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

def sketch():
    line_create((0, 0), (100, 0))
    point_create((50, 50))

    point_1 = (412, 293) # point
    point_2 = (412, 324) # line
    annotaion_point = (262, 319)
    distance_create(point_1, point_2, annotaion_point, 100)

    mouseClick(waitForObject(":OpenParts*_AppElements_ViewPort"), annotaion_point[0], annotaion_point[1], 0, Qt.LeftButton)
    waitFor("object.exists(':Distance.First object_QLineEdit')", 20000)
    test.compare(str(findObject(":Distance.First object_QLineEdit").text), "SketchPoint_1/PointCoordinates")
    waitFor("object.exists(':Distance.Second object_QLineEdit')", 20000)
    test.compare(str(findObject(":Distance.Second object_QLineEdit").text), "SketchLine_1")
    waitFor("object.exists(':Distance.ConstraintValue_ModuleBase_ParamSpinBox')", 20000)
    test.compare(str(findObject(":Distance.ConstraintValue_ModuleBase_ParamSpinBox").text), "100")

def main():
    source(findFile("scripts", "common.py"))

    startApplication("linux_run.sh")
    set_defaults()

    sketch_create(help_points("XY_plane"), lambda: sketch())
