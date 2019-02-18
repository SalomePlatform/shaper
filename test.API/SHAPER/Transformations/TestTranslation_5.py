# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

"""
Test case for Translation feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Boxes
Box_1 = model.addBox(Part_1_doc, 10, 20, 10)
Box_2 = model.addBox(Part_1_doc, 10, 20, 10)
Box_3 = model.addBox(Part_1_doc, 10, 20, 10)
Box_4 = model.addBox(Part_1_doc, 10, 20, 10)

# Sketches
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(47.23370043826552, 82.68004022993115, -80.41547074975036, -6.189635913624215)
SketchLine_2 = Sketch_1.addLine(-80.41547074975036, -6.189635913624215, 57.33252727276048, -26.79124265599387)
SketchLine_3 = Sketch_1.addLine(57.33252727276048, -26.79124265599387, 47.23370043826552, 82.68004022993115)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()

# Points
Point_1 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_2 = model.addPoint(Part_1_doc, 10, 10, 0)

# Vertex
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex")])

# Translations
Translation_99 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))
Translation_100 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_1"))
Translation_101 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("VERTEX", "InvalidName"), model.selection("VERTEX", "Point_1"))
Translation_102 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "InvalidName"))
Translation_103 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"))
Translation_104 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_2_1"))
Translation_105 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_4_1")], model.selection("VERTEX", "[Box_4_1/Back][Box_4_1/Left][Box_4_1/Bottom]"), model.selection("VERTEX", "[Box_4_1/Front][Box_4_1/Right][Box_4_1/Top]"))
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Translation_99, 1)
model.testNbSubResults(Translation_99, [0])
model.testNbSubShapes(Translation_99, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_99, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_99, model, Part_1_doc)

model.testNbResults(Translation_103, 1)
model.testNbSubResults(Translation_103, [0])
model.testNbSubShapes(Translation_103, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_103, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_103, model, Part_1_doc)

model.testNbResults(Translation_104, 1)
model.testNbSubResults(Translation_104, [0])
model.testNbSubShapes(Translation_104, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_104, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_104, model, Part_1_doc)

model.testNbResults(Translation_105, 1)
model.testNbSubResults(Translation_105, [0])
model.testNbSubShapes(Translation_105, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_105, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_105, model, Part_1_doc)


model.testNbResults(Translation_100, 0)
assert(Translation_100.feature().error() == 'The feature uses equal shapes.')

model.testNbResults(Translation_101, 0)
assert(Translation_101.feature().error() == 'Attribute "start_point" is not initialized.')

model.testNbResults(Translation_102, 0)
assert(Translation_102.feature().error() == 'Attribute "end_point" is not initialized.')
