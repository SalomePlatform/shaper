# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 80, 0, 150)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 150, 150, 150)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(150, 150, 150, 0)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(150, 0, 80, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintPerpendicular_2 = Sketch_1.setPerpendicular(SketchLine_4.result(), SketchLine_5.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_5.result())
SketchArc_1 = Sketch_1.addArc(0, 0, 80, 0, 0, 80, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_6).startPoint(), SketchArc_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 80)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 150)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection(), 200, 0)
Compound_1_objects = [model.selection("FACE", "Extrusion_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
Compound_2_objects = [model.selection("EDGE", "[Compound_1_1_2/Compound_1_1_2&Sketch_1/SketchLine_4][weak_name_4]"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "[Compound_1_1_3/Compound_1_1_3&Sketch_1/SketchLine_5][weak_name_4]")]
Compound_2 = model.addCompound(Part_1_doc, Compound_2_objects)
model.do()

model.testHaveNamingSubshapes(Compound_2,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
