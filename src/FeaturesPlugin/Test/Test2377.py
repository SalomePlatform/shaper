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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-1.039451114922807, 35.51457975986278, -38.45969125214407, -21.30874785591767)
SketchLine_2 = Sketch_1.addLine(-38.45969125214407, -21.30874785591767, 46.08233276157804, -21.30874785591767)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(46.08233276157804, -21.30874785591767, -1.039451114922807, 35.51457975986278)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f")], model.selection(), 100, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchCircle_1 = Sketch_2.addCircle(-10.74792845018279, -51.46099937127247, 20.55678647017868)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection("EDGE", "PartSet/OX"), model.selection("FACE", "PartSet/YOZ"), 0, model.selection(), 0)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Extrusion_2, 1)
model.testNbSubResults(Extrusion_2, [0])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Extrusion_2, [25446.012037604283250402659177780])

assert(model.checkPythonDump())
