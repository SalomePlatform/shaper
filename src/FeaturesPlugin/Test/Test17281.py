# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, -106.704990237, 59.3180779141, 313.27249047)
Point_2.setName("Point_30")
Point_2.result().setName("Point_30")
Point_3 = model.addPoint(Part_1_doc, -101.376950979, 62.2815769566, 321)
Point_3.setName("Point_31")
Point_3.result().setName("Point_31")
Point_4 = model.addPoint(Part_1_doc, -100.200106232, 69.3301090863, 329.729339172)
Point_4.setName("Point_32")
Point_4.result().setName("Point_32")
Point_5 = model.addPoint(Part_1_doc, -71.22799999999999, 49.284, 328.95)
Point_5.setName("Point_33")
Point_5.result().setName("Point_33")
Point_6 = model.addPoint(Part_1_doc, -74.8174225795, 47.3826420932, 326)
Point_6.setName("Point_34")
Point_6.result().setName("Point_34")
Point_7 = model.addPoint(Part_1_doc, -82.6510873528, 45.946338858, 320.314271667)
Point_7.setName("Point_35")
Point_7.result().setName("Point_35")
Polyline_1_objects = [model.selection("VERTEX", "Point_30"), model.selection("VERTEX", "Point_31"), model.selection("VERTEX", "Point_32"), model.selection("VERTEX", "Point_33"), model.selection("VERTEX", "Point_34"), model.selection("VERTEX", "Point_35")]
Polyline_1 = model.addPolyline3D(Part_1_doc, Polyline_1_objects, True)
Polyline_1.setName("Polyline_8")
Polyline_1.result().setName("Polyline_8_1")
Polyline_2 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_31"), model.selection("VERTEX", "Point_34")], False)
Polyline_2.setName("Polyline_11")
Polyline_2.result().setName("Polyline_11_1")
Point_8 = model.addPoint(Part_1_doc, model.selection("EDGE", "Polyline_11_1/Generated_Edge&Point_31/Point_31"), 0.5, True, False)
Point_8.setName("Point_36")
Point_8.result().setName("Point_36")
Scale_1 = model.addScale(Part_1_doc, [model.selection("WIRE", "Polyline_8_1")] , model.selection("VERTEX", "Point_36"), 1.05)
Scale_1.setName("Scale_4")
Scale_1.result().setName("Scale_4_1")
Polyline_3 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "[Scale_4_1/ME:Scaled&Point_33/Point_33]e[Scale_4_1/ME:Scaled&Point_35/Point_35]e"), model.selection("VERTEX", "[Scale_4_1/ME:Scaled&Point_30/Point_30]e[Scale_4_1/ME:Scaled&Point_32/Point_32]e")], False)
Polyline_3.setName("Polyline_10")
Polyline_3.result().setName("Polyline_10_1")
Recover_1 = model.addRecover(Part_1_doc, Polyline_3, [Scale_1.result()])
Recover_1.setName("Recover_10")
Recover_1.result().setName("Recover_10_1")
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("EDGE", "Recover_10_1/Modified_Edge&Point_31/Point_31"), model.selection("EDGE", "Polyline_10_1/Generated_Edge&weak_name_1")], model.selection("EDGE", "Recover_10_1/Modified_Edge&Point_33/Point_33"), [])
Pipe_1.setName("Pipe_3")
Pipe_1.result().setName("Pipe_3_1")
Recover_2 = model.addRecover(Part_1_doc, Pipe_1, [Recover_1.result()])
Recover_2.setName("Recover_11")
Recover_2.result().setName("Recover_11_1")
Pipe_2 = model.addPipe(Part_1_doc, [model.selection("EDGE", "Recover_11_1/Modified_Edge&Point_32/Point_32"), model.selection("EDGE", "Recover_11_1/Modified_Edge&Point_35/Point_35")], model.selection("EDGE", "Pipe_3_1/From_Edge_1"), [])
Pipe_2.setName("Pipe_4")
Pipe_2.result().setName("Pipe_4_1")
model.end()

assert(Pipe_2.feature().error() == "")

from GeomAPI import *

model.testNbResults(Pipe_2, 1)
model.testNbSubResults(Pipe_2, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Pipe_2, [280.91366299542733])

assert(model.checkPythonDump())
