# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(-77.19749092035903, 24.18794374198263, -26.50243221408208, 23.76942369170098)
SketchLine_2 = Sketch_1.addLine(-52.21940522238806, 23.98173370989996, -68.62380444441784, 24.11716249058297)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_1.result())
SketchLine_3 = Sketch_1.addLine(9.658082674231901, 24.38959180621407, 76.00931510902234, 24.79203653255435)
SketchLine_4 = Sketch_1.addLine(31.27467463593981, 24.52070441885453, 55.13167389667986, 24.6654059290823)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_3.result())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_3")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_2_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_2_1")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [4])

assert(model.checkPythonDump())
