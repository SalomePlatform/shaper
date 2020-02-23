# Copyright (C) 2020  CEA/DEN, EDF R&D
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
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(44.29784155360136, 17.09942588468031)
SketchArc_1 = Sketch_1.addArc(44.29784155360136, 17.09942588468031, 47.1668727423061, 12.27945348765633, 45.38299232715926, 22.60269052200972, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchArc_1.center())
SketchLine_1 = Sketch_1.addLine(42.5764228403785, 22.14892077680068, 39.70739165167375, 16.41085839939117)
SketchLine_2 = Sketch_1.addLine(39.70739165167375, 16.41085839939117, 43.03546783057126, 12.04993099255995)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(28.57555063949931, 12.96802097294547, 15.72229091410204, 12.96802097294547)
SketchLine_4 = Sketch_1.addLine(15.72229091410204, 12.96802097294547, 15.72229091410204, 21.46035329151154)
SketchLine_5 = Sketch_1.addLine(15.72229091410204, 21.46035329151154, 28.57555063949931, 21.46035329151154)
SketchLine_6 = Sketch_1.addLine(28.57555063949931, 21.46035329151154, 28.57555063949931, 12.96802097294547)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(-10.67279602198167, 14.78814178154371, -28.34602814440294, 14.78814178154371)
SketchLine_8 = Sketch_1.addLine(-28.34602814440294, 14.78814178154371, -28.34602814440294, 25.13271321305362)
SketchLine_9 = Sketch_1.addLine(-28.34602814440294, 25.13271321305362, -10.67279602198167, 25.13271321305362)
SketchLine_10 = Sketch_1.addLine(-10.67279602198167, 25.13271321305362, -10.67279602198167, 14.78814178154371)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_10.result())
SketchLine_11 = Sketch_1.addLine(-17.67323212242127, 25.13271321305362, -21.80463703415611, 14.78814178154371)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_11.startPoint(), SketchLine_9.result())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_7.result())
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchArc_1")], False)
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchArc_1_2")], False)
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")], False)
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r")])
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_10r-SketchLine_9r-SketchLine_11f-SketchLine_7r"), model.selection("FACE", "Sketch_1/Face-SketchLine_11r-SketchLine_9r-SketchLine_8r-SketchLine_7r")])
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 50)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_2_1"), model.selection("SOLID", "Cylinder_1_1")], keepSubResults = True)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OY"), 50)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Translation_2_1")], model.selection("EDGE", "PartSet/OZ"), 30, 3)
Compound_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("EDGE", "Edge_1_1"), model.selection("WIRE", "Wire_1_1"), model.selection("FACE", "Face_1_1"), model.selection("SHELL", "Shell_1_1"), model.selection("SOLID", "Translation_1_1"), model.selection("COMPSOLID", "Partition_1_1"), model.selection("COMPOUND", "AngularCopy_1_1")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
model.end()

TOLERANCE = 1.e-7
COPIES = 4

NB_SOLIDS = 7
NB_FACES = 44
NB_EDGES = 177
NB_VERTICES = 355
VOLUME = 5589.0486226

# collect reference data
REFERENCE = []
for ind in range(0, Compound_1.result().numberOfSubs()):
    p = Compound_1.result().subResult(ind).resultSubShapePair()[1].middlePoint()
    REFERENCE.append(p)

def assertResult(theFeature, theNbMoved):
    model.testNbResults(theFeature, 1)
    model.testNbSubResults(theFeature, [8])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.SOLID, [NB_SOLIDS])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.FACE, [NB_FACES])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.EDGE, [NB_EDGES])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.VERTEX, [NB_VERTICES])
    model.testResultsVolumes(theFeature, [VOLUME])

    for ind in range(0, theFeature.result().numberOfSubs()):
        ref = GeomAPI_Pnt(REFERENCE[ind].x(), REFERENCE[ind].y(), REFERENCE[ind].z())
        if ind < theNbMoved:
            ref.setY(0)
            ref.setZ(0)
        midPoint = theFeature.result().subResult(ind).resultSubShapePair()[0].shape().middlePoint()
        assert(midPoint.distance(ref) < TOLERANCE), "Sub-result {}; actual ({}, {}, {}) != expected ({}, {}, {})".format(ind, midPoint.x(), midPoint.y(), midPoint.z(), ref.x(), ref.y(), ref.z())


model.begin()
MultiRotation_1 = model.addMultiRotation(Part_1_doc, [model.selection("VERTEX", "Compound_1_1_1")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
NB_VERTICES += COPIES - 1
assertResult(MultiRotation_1, 1)

MultiRotation_2 = model.addMultiRotation(Part_1_doc, [model.selection("EDGE", "AngularCopy_2_1_2")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
c = GeomAlgoAPI_CompoundBuilder.compound([Edge_1.defaultResult().shape()])
REFERENCE[1] = c.middlePoint()
NB_EDGES += COPIES - 1
NB_VERTICES += 2 * (COPIES - 1)
assertResult(MultiRotation_2, 2)

MultiRotation_3 = model.addMultiRotation(Part_1_doc, [model.selection("WIRE", "AngularCopy_3_1_3")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
c = GeomAlgoAPI_CompoundBuilder.compound([Wire_1.defaultResult().shape()])
REFERENCE[2] = c.middlePoint()
NB_EDGES += 2 * (COPIES - 1)
NB_VERTICES += 4 * (COPIES - 1)
assertResult(MultiRotation_3, 3)

MultiRotation_4 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "AngularCopy_4_1_4")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
NB_FACES += COPIES - 1
NB_EDGES += 4 * (COPIES - 1)
NB_VERTICES += 8 * (COPIES - 1)
assertResult(MultiRotation_4, 4)

MultiRotation_5 = model.addMultiRotation(Part_1_doc, [model.selection("SHELL", "AngularCopy_5_1_5")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
NB_FACES += 2 * (COPIES - 1)
NB_EDGES += 8 * (COPIES - 1)
NB_VERTICES += 16 * (COPIES - 1)
assertResult(MultiRotation_5, 5)

MultiRotation_6 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "AngularCopy_6_1_6")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
NB_SOLIDS += COPIES - 1
NB_FACES += 6 * (COPIES - 1)
NB_EDGES += 24 * (COPIES - 1)
NB_VERTICES += 48 * (COPIES - 1)
VOLUME += 1000 * (COPIES - 1)
assertResult(MultiRotation_6, 6)

MultiRotation_7 = model.addMultiRotation(Part_1_doc, [model.selection("COMPSOLID", "AngularCopy_7_1_7")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
NB_SOLIDS += 3 * (COPIES - 1)
NB_FACES += 17 * (COPIES - 1)
NB_EDGES += 66 * (COPIES - 1)
NB_VERTICES += 132 * (COPIES - 1)
VOLUME += 1589.0486226 * (COPIES - 1)
assertResult(MultiRotation_7, 7)

MultiRotation_8 = model.addMultiRotation(Part_1_doc, [model.selection("COMPOUND", "AngularCopy_8_1_8")], model.selection("EDGE", "PartSet/OX"), COPIES, keepSubResults = True)
NB_SOLIDS += 3 * (COPIES - 1)
NB_FACES += 18 * (COPIES - 1)
NB_EDGES += 72 * (COPIES - 1)
NB_VERTICES += 144 * (COPIES - 1)
VOLUME += 3000 * (COPIES - 1)
assertResult(MultiRotation_8, 8)

model.end()

assert(model.checkPythonDump())
