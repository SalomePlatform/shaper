# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 207.1440202435477)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchCircle_2 = Sketch_1.addCircle(0, 0, 103.3027146966417)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_2_2r")], model.selection(), 410, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchCircle_3 = Sketch_2.addCircle(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1/To_Face]"))
SketchCircle_4 = Sketch_2.addCircle(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]"))
SketchArc_1 = Sketch_2.addArc(-76.31943784014528, 69.61892179185162, 22.73963059863011, 100.7688447087372, -98.26117835861398, -31.87776170424145, False)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchCircle_3.results()[1], SketchArc_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchCircle_3.results()[1], SketchArc_1.endPoint())
SketchConstraintTangent_1 = Sketch_2.setTangent(SketchCircle_4.results()[1], SketchArc_1.results()[1])
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchArc_1.center(), SketchCircle_3.results()[1])
SketchArc_2 = Sketch_2.addArc(0, 0, 22.73963059863011, 100.7688447087372, -98.26117835861152, -31.87776170424295, False)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchCircle_3.result(), SketchArc_2.center())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchArc_1.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchArc_1.results()[1], SketchArc_2.endPoint())
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchArc_1_2f-SketchArc_2_2r")], model.selection(), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_2_2r"), 0, model.selection(), 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face"))
SketchLine_1 = Sketch_3.addLine(72.49453964599654, -117.643151959956, 76.08424302289386, 111.5797586569786)
SketchArc_3 = Sketch_3.addArc(23.82028040531734, -147.9761102212422, 72.49453964599654, -117.643151959956, 29.20403695178628, -205.0750334594096, False)
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_1.startPoint(), SketchArc_3.startPoint())
SketchProjection_1 = Sketch_3.addProjection(model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchCircle_1_2&weak_name_2]"), False)
SketchArc_4 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchArc_3.endPoint(), SketchArc_4.results()[1])
model.do()
model.do()
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Sketch_3.feature()))
