## Copyright (C) 2017  CEA/DEN, EDF R&D
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


from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 14.26163338647443, -11.82948087533604, 14.26163338647443, 11.82948087533609, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchLine_1 = Sketch_1.addLine(14.26163338647443, -11.82948087533604, 14.26163338647443, 11.82948087533609)
SketchLine_1.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchArc_1_2")])
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OY"), 200, 0)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("FACE", "PartSet/XOZ"), model.selection("FACE", "Revolution_1_1")])
Union_1 = model.addUnion(Part_1_doc, [model.selection("FACE", "Partition_1_1_1"), model.selection("FACE", "Partition_1_1_2")])
model.do()

model.checkResult(Union_1,model,1,[0],[0],[1],[4],[8])
model.testHaveNamingSubshapes(Union_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
