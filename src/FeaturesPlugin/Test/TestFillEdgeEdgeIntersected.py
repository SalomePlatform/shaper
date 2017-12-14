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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-54.08747895500726, -7.310347750362856, 54.27191756168357, 63.68373962264153)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_2 = Sketch_2.addLine(-42.46271611030479, 53.71965718432511, 14.83075791001449, -51.73354862119014)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_1")])
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/Edge-SketchLine_2")])
Boolean_1 = model.addFill(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], [model.selection("EDGE", "Edge_2_1")])
model.do()

model.checkBooleansResult(Boolean_1,model,1,[2],[0],[0],[2],[4])
model.testHaveNamingSubshapes(Boolean_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
