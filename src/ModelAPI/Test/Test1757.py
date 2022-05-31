# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

"""
      Test1757.py
      Unit test for testing the modification of base object after movement of the history line:
      the referenced (that is disabled, after the current position of the history line) attribute
      must be updated.
      The test is created by modification of the dumped script.

"""

from salome.shaper import model

#=========================================================================
# Initialize a part
#=========================================================================
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

#=========================================================================
# Create a base box: rectangular sketch and extrusion
#=========================================================================
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(199.69219283509, 125.016366179415, -144.040270241704, 125.016366179415)
SketchLine_2 = Sketch_1.addLine(-144.040270241704, 125.016366179415, -144.040270241704, -111.365692746051)
SketchLine_3 = Sketch_1.addLine(-144.040270241704, -111.365692746051, 199.69219283509, -111.365692746051)
SketchLine_4 = Sketch_1.addLine(199.69219283509, -111.365692746051, 199.69219283509, 125.016366179415)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 300, 0)

#=========================================================================
# Cut another rectangle to make a groove
#=========================================================================
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), model.selection(), 0, model.selection("FACE", "Extrusion_1_1/To_Face"), 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/From_Face"))
SketchLine_5 = Sketch_2.addLine(-47.794137712106, -188.943752610533, 90.842718875892, -188.943752610533)
SketchLine_6 = Sketch_2.addLine(90.842718875892, -188.943752610533, 90.842718875892, -51.567231082425)
SketchLine_7 = Sketch_2.addLine(90.842718875892, -51.567231082425, -47.794137712106, -51.567231082425)
SketchLine_8 = Sketch_2.addLine(-47.794137712106, -51.567231082425, -47.794137712106, -188.943752610533)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.do()

#=========================================================================
# Make a cylindrical hole using one of the produced faces
#=========================================================================
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [], model.selection(), model.selection(), 0, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "(ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/From_Face)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)(ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face)(Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4)"))
SketchCircle_1 = Sketch_3.addCircle(143.412751420315, -228.52745656314, 32.158435160764)
ExtrusionCut_2.setNestedSketch(Sketch_3)

#=========================================================================
# Move a history line before this hole creation to modify groove
#=========================================================================
Part_1.document().setCurrentFeature(ExtrusionCut_1.feature(), True)

#=========================================================================
# Modify the cylindrical hole base face by another groove in the history before the hole is created
#=========================================================================
ExtrusionCut_3 = model.addExtrusionCut(Part_1_doc, [], model.selection(), model.selection(), 0, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"))
SketchLine_9 = Sketch_4.addLine(-176.701443076057, -151.212050129331, -91.83965287802501, -151.212050129331)
SketchLine_10 = Sketch_4.addLine(-91.83965287802501, -151.212050129331, -91.83965287802501, -84.473709256573)
SketchLine_11 = Sketch_4.addLine(-91.83965287802501, -84.473709256573, -176.701443076057, -84.473709256573)
SketchLine_12 = Sketch_4.addLine(-176.701443076057, -84.473709256573, -176.701443076057, -151.212050129331)
SketchConstraintCoincidence_9 = Sketch_4.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_4.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_4.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_4.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_4.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_4.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_4.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_4.setVertical(SketchLine_12.result())
model.do()
ExtrusionCut_3.setNestedSketch(Sketch_4)

model.do()

#=========================================================================
# Move a history line just to the end to recreate the cylindrical hole
#=========================================================================
Part_1.document().setCurrentFeature(ExtrusionCut_2.feature(), True)

model.end()

#=========================================================================
# Check that the circle-sketch and the cylindrical-hole are correctly updated
#=========================================================================
import ModelAPI

assert(ModelAPI.ModelAPI_Session.get().validators().validate(Sketch_4.feature()))
assert(ModelAPI.ModelAPI_Session.get().validators().validate(ExtrusionCut_2.feature()))
assert(Sketch_3.feature().selection("External").namingName() == "(ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchLine_6)(ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face)")

assert(model.checkPythonDump())
