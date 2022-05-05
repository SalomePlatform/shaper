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
Test case for Translation feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Boxes
Box_1 = model.addBox(Part_1_doc, 10, 20, 10)
Box_2 = model.addBox(Part_1_doc, 10, 20, 10)
Box_3 = model.addBox(Part_1_doc, 10, 20, 10)
Box_4 = model.addBox(Part_1_doc, 10, 20, 10)
Box_5 = model.addBox(Part_1_doc, 10, 20, 10)
Box_6 = model.addBox(Part_1_doc, 10, 20, 10)
Box_7 = model.addBox(Part_1_doc, 10, 20, 10)
Box_8 = model.addBox(Part_1_doc, 10, 20, 10)
Box_9 = model.addBox(Part_1_doc, 10, 20, 10)
Box_10 = model.addBox(Part_1_doc, 10, 20, 10)
Box_11 = model.addBox(Part_1_doc, 10, 20, 10)
Box_12 = model.addBox(Part_1_doc, 10, 20, 10)
Box_13 = model.addBox(Part_1_doc, 10, 20, 10)
Box_14 = model.addBox(Part_1_doc, 10, 20, 10)
Box_15 = model.addBox(Part_1_doc, 10, 20, 10)
Box_16 = model.addBox(Part_1_doc, 10, 20, 10)
Box_17 = model.addBox(Part_1_doc, 10, 20, 10)
Box_18 = model.addBox(Part_1_doc, 10, 20, 10)

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Box_19 = model.addBox(Part_2_doc, 10, 20, 10)

Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Box_20 = model.addBox(Part_3_doc, 10, 20, 10)

Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()
Box_21 = model.addBox(Part_4_doc, 10, 20, 10)

Part_5 = model.addPart(partSet)
Part_5_doc = Part_5.document()
Box_22 = model.addBox(Part_5_doc, 10, 20, 10)

Part_6 = model.addPart(partSet)
Part_6_doc = Part_6.document()
Box_23 = model.addBox(Part_6_doc, 10, 20, 10)

Part_7 = model.addPart(partSet)
Part_7_doc = Part_7.document()
Box_24 = model.addBox(Part_7_doc, 10, 20, 10)

Part_8 = model.addPart(partSet)
Part_8_doc = Part_8.document()
Box_25 = model.addBox(Part_8_doc, 10, 20, 10)

Part_9 = model.addPart(partSet)
Part_9_doc = Part_9.document()
Box_26 = model.addBox(Part_9_doc, 10, 20, 10)

####################
# Sketches
# Sketch contenant un triangle
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(105.8795836373178, 106.9780351893485, -55.30451701345322, -118.6797057217308)
SketchLine_2 = Sketch_1.addLine(-55.30451701345322, -118.6797057217308, -134.9484255703047, 86.11891628160174)
SketchLine_3 = Sketch_1.addLine(-134.9484255703047, 86.11891628160174, 105.8795836373178, 106.9780351893485)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_2 = model.addSketch(Part_2_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_2.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_4 = Sketch_2.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_2 = Sketch_3.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_5 = Sketch_3.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_4 = model.addSketch(Part_3_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_3 = Sketch_4.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_6 = Sketch_4.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_5 = model.addSketch(Part_4_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_4 = Sketch_5.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_7 = Sketch_5.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_6 = model.addSketch(Part_5_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_5 = Sketch_6.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_8 = Sketch_6.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_7 = model.addSketch(Part_6_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_6 = Sketch_7.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_9 = Sketch_7.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_8 = model.addSketch(Part_7_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_7 = Sketch_8.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_10 = Sketch_8.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_9 = model.addSketch(Part_8_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_8 = Sketch_9.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_11 = Sketch_9.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_10 = model.addSketch(Part_9_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_9 = Sketch_10.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_12 = Sketch_10.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

model.do()

# Extrusions
Extrusion_1 = model.addExtrusion(Part_2_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_3 = model.addExtrusion(Part_3_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_4 = model.addExtrusion(Part_4_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_5 = model.addExtrusion(Part_5_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_6 = model.addExtrusion(Part_6_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_7 = model.addExtrusion(Part_7_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_8 = model.addExtrusion(Part_8_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)
Extrusion_9 = model.addExtrusion(Part_9_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 15, 0)

# Edges
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_2")])
Edge_2 = model.addEdge(Part_7_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])

# Axis
Axis_1 = model.addAxis(partSet, 10, 10, 10)

# Parameters
model.addParameter(partSet, "d", "15")

# Multitranslations
MultiTranslation_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_10_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_2 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_11_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_3 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_12_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_4 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_13_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_5 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_14_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_6 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_15_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_7 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_16_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_8 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_17_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)
MultiTranslation_9 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_18_1")], model.selection("EDGE", "PartSet/OX"), 15, 3, model.selection("EDGE", "PartSet/OY"), 15, 3)

# Translations
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 15)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OX"), 0)
Translation_3 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OX"), -15)
Translation_4 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_4_1")], model.selection("EDGE", "InvalidName"), 15)
Translation_5 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], model.selection("EDGE", "PartSet/OY"), "d")
Translation_6 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_6_1")], model.selection("EDGE", "Sketch_1/SketchLine_3"), 15)
Translation_7 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_7_1")], model.selection("EDGE", "Edge_1_1"), 15)
Translation_8 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_8_1")], model.selection("EDGE", "[Box_8_1/Right][Box_8_1/Top]"), 15)
Translation_9 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_9_1")], model.selection("EDGE", "PartSet/Axis_4"), 15)
Translation_10 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_1_1")], model.selection("EDGE", "PartSet/OZ"), 15)
Translation_11 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_2_1")], model.selection("EDGE", "PartSet/OZ"), 0)
Translation_12 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_3_1")], model.selection("EDGE", "PartSet/OZ"), -15)
Translation_13 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_4_1")], model.selection("EDGE", "InvalidName"), 15)
Translation_14 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_5_1")], model.selection("EDGE", "PartSet/OZ"), "d")
Translation_15 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_6_1")], model.selection("EDGE", "Sketch_1/SketchLine_2"), 50)
Translation_16 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_7_1")], model.selection("EDGE", "Edge_1_1"), 50)
Translation_17 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_8_1")], model.selection("EDGE", "[LinearCopy_8_1_3/MF:Translated_Face&Box_17_1/Left][LinearCopy_8_1_3/MF:Translated_Face&Box_17_1/Top]"), 50)
Translation_18 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_9_1")], model.selection("EDGE", "PartSet/Axis_4"), 50)
Translation_19 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_2/")], model.selection("EDGE", "OX"), 15)
Translation_20 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_3/")], model.selection("EDGE", "OX"), 0)
Translation_21 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_4/")], model.selection("EDGE", "OX"), -15)
Translation_22 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_5/")], model.selection("EDGE", "InvalidName"), 15)
Translation_23 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_6/")], model.selection("EDGE", "OX"), "d")
Translation_24 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_7/")], model.selection("EDGE", "Part_7/Edge_1_1"), 15)
Translation_25 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_8/")], model.selection("EDGE", "Part_8/[Box_1_1/Front][Box_1_1/Left]"), 15)
Translation_26 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_9/")], model.selection("EDGE", "Axis_4"), 15)
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Translation_1, 1)
model.testNbSubResults(Translation_1, [0])
model.testNbSubShapes(Translation_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_1, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_1, model, Part_1_doc)

model.testNbResults(Translation_2, 1)
model.testNbSubResults(Translation_2, [0])
model.testNbSubShapes(Translation_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_2, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_2, model, Part_1_doc)

model.testNbResults(Translation_3, 1)
model.testNbSubResults(Translation_3, [0])
model.testNbSubShapes(Translation_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_3, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_3, model, Part_1_doc)

model.testNbResults(Translation_5, 1)
model.testNbSubResults(Translation_5, [0])
model.testNbSubShapes(Translation_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_5, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_5, model, Part_1_doc)

model.testNbResults(Translation_6, 1)
model.testNbSubResults(Translation_6, [0])
model.testNbSubShapes(Translation_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_6, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_6, model, Part_1_doc)

model.testNbResults(Translation_7, 1)
model.testNbSubResults(Translation_7, [0])
model.testNbSubShapes(Translation_7, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_7, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_7, model, Part_1_doc)

model.testNbResults(Translation_8, 1)
model.testNbSubResults(Translation_8, [0])
model.testNbSubShapes(Translation_8, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_8, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_8, model, Part_1_doc)

model.testNbResults(Translation_9, 1)
model.testNbSubResults(Translation_9, [0])
model.testNbSubShapes(Translation_9, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_9, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_9, model, Part_1_doc)

model.testNbResults(Translation_10, 1)
model.testNbSubResults(Translation_10, [9])
model.testNbSubShapes(Translation_10, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_10, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_10, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_10, model, Part_1_doc)

model.testNbResults(Translation_11, 1)
model.testNbSubResults(Translation_11, [9])
model.testNbSubShapes(Translation_11, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_11, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_11, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_11, model, Part_1_doc)

model.testNbResults(Translation_12, 1)
model.testNbSubResults(Translation_12, [9])
model.testNbSubShapes(Translation_12, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_12, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_12, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_12, model, Part_1_doc)

model.testNbResults(Translation_14, 1)
model.testNbSubResults(Translation_14, [9])
model.testNbSubShapes(Translation_14, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_14, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_14, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_14, model, Part_1_doc)

model.testNbResults(Translation_15, 1)
model.testNbSubResults(Translation_15, [9])
model.testNbSubShapes(Translation_15, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_15, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_15, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_15, model, Part_1_doc)

model.testNbResults(Translation_16, 1)
model.testNbSubResults(Translation_16, [9])
model.testNbSubShapes(Translation_16, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_16, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_16, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_16, model, Part_1_doc)

model.testNbResults(Translation_17, 1)
model.testNbSubResults(Translation_17, [9])
model.testNbSubShapes(Translation_17, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_17, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_17, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_17, model, Part_1_doc)

model.testNbResults(Translation_18, 1)
model.testNbSubResults(Translation_18, [9])
model.testNbSubShapes(Translation_18, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Translation_18, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Translation_18, GeomAPI_Shape.FACE, [54])
model.testHaveNamingFaces(Translation_18, model, Part_1_doc)

model.testNbResults(Translation_4, 0)
assert(Translation_4.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Translation_13, 0)
assert(Translation_13.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Translation_22, 0)
assert(Translation_22.feature().error() == 'Attribute "axis_object" is not initialized.')

# See what we can check on a part
model.testNbResults(Translation_19, 1)
#model.testNbSubResults(Translation_19, [0])
#model.testNbSubShapes(Translation_19, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_19, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_19, GeomAPI_Shape.FACE, [9])
#model.testHaveNamingFaces(Translation_19, model, Part_2_doc)

# See what we can check on a part
model.testNbResults(Translation_20, 1)
#model.testNbSubResults(Translation_20, [0])
#model.testNbSubShapes(Translation_20, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_20, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_20, GeomAPI_Shape.FACE, [9])
#model.testHaveNamingFaces(Translation_20, model, Part_3_doc)

# See what we can check on a part
model.testNbResults(Translation_21, 1)
#model.testNbSubResults(Translation_21, [0])
#model.testNbSubShapes(Translation_21, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_21, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_21, GeomAPI_Shape.FACE, [9])
#model.testHaveNamingFaces(Translation_21, model, Part_4_doc)

# See what we can check on a part
model.testNbResults(Translation_23, 1)
#model.testNbSubResults(Translation_23, [0])
#model.testNbSubShapes(Translation_23, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_23, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_23, GeomAPI_Shape.FACE, [9])
#model.testHaveNamingFaces(Translation_23, model, Part_6_doc)

# See what we can check on a part
#model.testNbResults(Translation_24, 1)
#model.testNbSubResults(Translation_24, [0])
#model.testNbSubShapes(Translation_24, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_24, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_24, GeomAPI_Shape.FACE, [9])
#model.testNbSubShapes(Translation_24, GeomAPI_Shape.SOLID, [3])
#model.testNbSubShapes(Translation_24, GeomAPI_Shape.FACE, [18])
#model.testHaveNamingFaces(Translation_24, model, Part_7_doc)

# See what we can check on a part
#model.testNbResults(Translation_25, 1)
#model.testNbSubResults(Translation_25, [0])
#model.testNbSubShapes(Translation_25, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_25, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_25, GeomAPI_Shape.FACE, [9])
#model.testNbSubShapes(Translation_25, GeomAPI_Shape.SOLID, [3])
#model.testNbSubShapes(Translation_25, GeomAPI_Shape.FACE, [18])
#model.testHaveNamingFaces(Translation_25, model, Part_8_doc)

# See what we can check on a part
model.testNbResults(Translation_26, 1)
#model.testNbSubResults(Translation_26, [0])
#model.testNbSubShapes(Translation_26, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(Translation_26, GeomAPI_Shape.SOLID, [2])
#model.testNbSubShapes(Translation_26, GeomAPI_Shape.FACE, [9])
#model.testHaveNamingFaces(Translation_26, model, Part_9_doc)
