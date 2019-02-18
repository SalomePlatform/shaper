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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-65.75962883650391, 1.812165175943577, 20.84344758927215)
SketchLine_1 = Sketch_1.addLine(-64.98081644495703, 22.64105757725738, -64.58019600110421, -18.99788637597517)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
SketchCircle_2 = Sketch_1.addCircle(62.53870966583226, 1.604010602289747, 23.75523331537361)
SketchLine_2 = Sketch_1.addLine(63.75890236666707, 25.3278854890081, 63.76050419996562, -22.11978184293701)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_2.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(-32.47294132465983, 2.220594155120791, 24.592292084386)
SketchLine_3 = Sketch_2.addLine(-57.05151402373072, 3.041930894979393, -7.893787575150295, 1.41683366733466)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchCircle_3.results()[1])
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchCircle_3.results()[1])
SketchCircle_4 = Sketch_2.addCircle(107.0005131452338, 2.231606536656211, 32.56614591098422)
SketchLine_4 = Sketch_2.addLine(74.45658844157124, 3.434446666454839, 139.5031967571015, 0.1995097076777023)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchCircle_4.results()[1])
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchCircle_4.results()[1])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 20, 20)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_2_2"), model.selection("COMPSOLID", "Extrusion_2_1")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [22])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [84])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [168])

assert(model.checkPythonDump())
