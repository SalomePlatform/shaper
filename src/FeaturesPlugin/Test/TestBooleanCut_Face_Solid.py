# Copyright (C) 2014-2024  CEA, EDF
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
SketchCircle_1 = Sketch_1.addCircle(-8.850181926352695, 1.062021831162306, 46.73432400996293)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_1 = Sketch_2.addLine(64.95207814695526, 55.29739830292169, -8.272875875352462, 55.29739830292169)
SketchLine_2 = Sketch_2.addLine(-8.272875875352462, 55.29739830292169, -8.272875875352462, -5.036618941767999)
SketchLine_3 = Sketch_2.addLine(-8.272875875352462, -5.036618941767999, 64.95207814695526, -5.036618941767999)
SketchLine_4 = Sketch_2.addLine(64.95207814695526, -5.036618941767999, 64.95207814695526, 55.29739830292169)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 10, 10)
Cut_1 = model.addCut(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
Group_1_objects = [model.selection("VERTEX", "Cut_1_1/Generated_Vertex&Sketch_2/SketchLine_3&Sketch_1/SketchCircle_1_2"), model.selection("FACE", "Cut_1_1"), model.selection("FACE", "Cut_1_1"), model.selection("VERTEX", "Cut_1_1/Generated_Vertex&Face_1_1/Face_1_1")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("FACE", "Cut_1_1"), model.selection("EDGE", "Cut_1_1/Generated_Edge&Sketch_2/SketchLine_3&Face_1_1/Face_1_1"), model.selection("VERTEX", "Cut_1_1/Generated_Vertex&Face_1_1/Face_1_1"), model.selection("FACE", "Cut_1_1")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3 = model.addGroup(Part_1_doc, [model.selection("FACE", "Cut_1_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [6])

assert(model.checkPythonDump())
