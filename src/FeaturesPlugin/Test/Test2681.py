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
SketchLine_1 = Sketch_1.addLine(30.99434204882063, 26.80840312072785, -30.86519895695055, 26.80840312072785)
SketchLine_2 = Sketch_1.addLine(-30.86519895695055, 26.80840312072785, -30.86519895695055, -19.3714637805129)
SketchLine_3 = Sketch_1.addLine(-30.86519895695055, -19.3714637805129, 30.99434204882063, -19.3714637805129)
SketchLine_4 = Sketch_1.addLine(30.99434204882063, -19.3714637805129, 30.99434204882063, 26.80840312072785)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(-7.617093567666898, 26.80840312072785, -19.24336125977736, 26.80840312072785, 4.009174124443563, 26.80840312072785, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.endPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
model.do()
model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())
