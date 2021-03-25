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
h_marche=18
model.addParameter(Part_1_doc, "h_marche", str(h_marche))
model.addParameter(Part_1_doc, "angle", "35")
nb_marches=25
h_totale=nb_marches*(h_marche+1)

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(13.86513942972648, 15.13114452817127, 113.484609238901, 23.84671880293715)
SketchLine_2 = Sketch_1.addLine(124.230417176185, 14, 124.230417176185, 0)
SketchLine_3 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.result())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchArc_1 = Sketch_1.addArc(15.1889430421138, 0, 0, 0, 13.86513942972648, 15.13114452817127, True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.result(), SketchArc_1.center())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(114.3460855070856, 14, 124.230417176185, 14, 113.484609238901, 23.84671880293715, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_1.endPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_2.result())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_1.result())
SketchConstraintMirror_1_objects = [SketchArc_1.results()[1], SketchLine_1.result(), SketchArc_2.results()[1], SketchLine_2.result()]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_3.result(), SketchConstraintMirror_1_objects)
[SketchArc_3, SketchLine_4, SketchArc_4, SketchLine_5] = SketchConstraintMirror_1.mirrored()
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_1.result(), SketchLine_3.result(), 5)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 100)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 14)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2f-SketchArc_3_2f-SketchLine_4f-SketchArc_4_2f-SketchLine_5f-SketchLine_2r-SketchArc_2_2f-SketchLine_1r")], model.selection(), 5, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchArc_1"), True)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_2.addCircle(15.1889430421138, 0, 9.469961851384218)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
model.do()
# 1st step :
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2f")], model.selection(), h_totale, 0)
Axis_4 = model.addAxis(Part_1_doc, model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2"))
model.do()
# Cycle : use previous step to get the next one, translated and rotated
stairFeature = Extrusion_1
# 15 steps now to fille the whole cylinder, but it may be changed
for step in range(nb_marches):
  Translation = model.addTranslation(Part_1_doc, [stairFeature.result()], model.selection("EDGE", "PartSet/OZ"), "h_marche")
  Recover = model.addRecover(Part_1_doc, Translation, [stairFeature.result()])
  Rotation = model.addRotation(Part_1_doc, [Translation.result()], model.selection("EDGE", "Axis_1"), "angle")
  model.do() # next transaction
  stairFeature = Rotation # store the next step feature to translate/rotate it in the next iteration

#====================================================================================
model.end()

assert(Part_1_doc.size("Bodies") == nb_marches + 2)
model.testResultsVolumes(stairFeature, [23382.6888388])

assert(model.checkPythonDump())
