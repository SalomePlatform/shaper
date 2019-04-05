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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 50)
SketchLine_2 = Sketch_1.addLine(0, 50, 50, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(50, 50, 50, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(50, 0, 0, 0)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchLine_1.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 50)
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_2.result(), SketchLine_4.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_1 = Sketch_2.addCircle(26.06240015364781, 18.07716886601709, 12.23427461205297)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), model.selection("FACE", "Extrusion_1_1/To_Face"), 0, model.selection(), 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_2_1")])
model.do()

Folder_1 = model.addFolder(Part_1_doc, Sketch_1, Extrusion_1)
Folder_1.setName("Box")
Folder_2 = model.addFolder(Part_1_doc, Sketch_2, Extrusion_2)
Folder_2.setName("Cylinder")

from GeomAPI import GeomAPI_Shape

# check result shape
model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Cut_1, [101488.717162133529200218617916107])

model.end()

assert(model.checkPythonDump())
