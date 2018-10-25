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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(40.12837458064153, 0, 0, 0)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 44.52500071883479)
SketchLine_3 = Sketch_1.addLine(0, 44.52500071883479, 40.12837458064153, 44.52500071883479)
SketchLine_4 = Sketch_1.addLine(40.12837458064153, 44.52500071883479, 40.12837458064153, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(0, 0, 24.97092001270093)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.center())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_1")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Wire_1_1")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Wire_2_1")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_5 = Sketch_2.addLine(49.62901186301215, 25.3130971701971, 39.15939470736273, 25.3130971701971)
SketchLine_6 = Sketch_2.addLine(39.15939470736273, 25.3130971701971, 39.15939470736273, 38.96586914453801)
SketchLine_7 = Sketch_2.addLine(39.15939470736273, 38.96586914453801, 49.62901186301215, 38.96586914453801)
SketchLine_8 = Sketch_2.addLine(49.62901186301215, 38.96586914453801, 49.62901186301215, 25.3130971701971)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r")], model.selection(), 50, 50, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(ExtrusionCut_1, 2)
model.testNbSubResults(ExtrusionCut_1, [0, 0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [10, 3])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [48, 6])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [96, 12])
model.testResultsVolumes(ExtrusionCut_1, [86396.175406624068273231387138367, 97946.509572227776516228914260864])

assert(model.checkPythonDump())
