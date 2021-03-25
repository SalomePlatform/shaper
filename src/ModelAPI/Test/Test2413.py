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
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 50)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchArc_1 = Sketch_1.addArc(0, 0, 0, 50, 50, 0, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.center())
SketchLine_3 = Sketch_1.addLine(0, 0, 50, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_4 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.result())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.endPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 50)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3f-SketchArc_1_2f-SketchLine_1r")], model.selection(), 100, 0)
model.do()
Solid_1_objects = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3")]
Solid_1 = model.addSolid(Part_1_doc, Solid_1_objects)
# check that resulting build-solid feature is invalid: only one not-closed face is used
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Solid_1.feature()) == False)
ModelAPI_Session.get().abortOperation()
# another try: to make a solid with a face inside
model.begin()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"), 70, True)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_5 = Sketch_2.addLine(14.9039069695087, 8.923908092675951, 26.86805702948195, 11.13714035991706)
SketchLine_6 = Sketch_2.addLine(26.86805702948195, 11.13714035991706, 18.50962242738747, 32.13572332177004)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_9")
SketchLine_7 = Sketch_2.addLine(18.50962242738747, 32.13572332177004, 14.9039069695087, 8.923908092675951)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_10")
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_7.endPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_5r-SketchLine_6f-SketchLine_7f")])
Solid_1_objects = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Extrusion_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Face_1_1")]
Solid_1 = model.addSolid(Part_1_doc, Solid_1_objects)
model.do()
model.end()

assert(model.checkPythonDump())
