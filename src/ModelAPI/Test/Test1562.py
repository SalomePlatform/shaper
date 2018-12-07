## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
lenParam = model.addParameter(Part_1_doc, "len", "45")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(63.23267137124048, -31.30105453337777, -32.56343706523536, -31.30105453337777)
SketchLine_2 = Sketch_1.addLine(-32.56343706523536, -31.30105453337777, -32.56343706523536, -163.6364723017416)
SketchLine_3 = Sketch_1.addLine(-32.56343706523536, -163.6364723017416, 63.23267137124048, -163.6364723017416)
SketchLine_4 = Sketch_1.addLine(63.23267137124048, -163.6364723017416, 63.23267137124048, -31.30105453337777)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"))
SketchLine_5 = Sketch_2.addLine(43.23267137124047, 69.97394590606471, -1.767328628759523, 69.97394590606471)
SketchLine_6 = Sketch_2.addLine(-1.767328628759523, 69.97394590606471, -1.767328628759523, 20.8583125692591)
SketchLine_7 = Sketch_2.addLine(-1.767328628759523, 20.8583125692591, 43.23267137124047, 20.8583125692591)
SketchLine_8 = Sketch_2.addLine(43.23267137124047, 20.8583125692591, 43.23267137124047, 69.97394590606471)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4]"), False)
SketchLine_9 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_9.result(), SketchLine_7.endPoint(), 20, True)
SketchConstraintLength_1 = Sketch_2.setLength(SketchLine_7.result(), "len")
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_5r-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"), 0, model.selection(), 0, [model.selection("SOLID", "Extrusion_1_1")])
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"), 0, model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_8"), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_8"))
SketchCircle_1 = Sketch_3.addCircle(-89.76476373103716, -34.76230113958862, 5.223553090716001)
ExtrusionCut_2.setNestedSketch(Sketch_3)
model.do()
# on increase of parameter selected part of extrusion is increased and selected face is removed (U becomes L)
# so, check that selection is still correct and cut from not existing part produces correct shape anyway
lenParam.setValue(100)
model.do()

# check the result validity
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(ExtrusionCut_2.feature()))

# set the value back to avoid changed the name text in selection of "to_object" in check python dump
lenParam.setValue(45)

model.end()

assert(model.checkPythonDump())
