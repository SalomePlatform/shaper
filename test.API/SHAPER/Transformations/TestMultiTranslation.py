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

"""
Test case for MultiTranslation feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Create Boxes
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Box_4 = model.addBox(Part_1_doc, 10, 10, 10)
Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
Box_6 = model.addBox(Part_1_doc, 10, 10, 10)
Box_7 = model.addBox(Part_1_doc, 10, 10, 10)
Box_8 = model.addBox(Part_1_doc, 10, 10, 10)
Box_9 = model.addBox(Part_1_doc, 10, 10, 10)

# Create Sketches
# Sketch contenant un contour ferme de 3 lignes
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20.57127738485033, 88.55774147773481, -110.037034305799, -25.45393392073257)
SketchLine_2 = Sketch_1.addLine(-110.037034305799, -25.45393392073257, 49.43499267560162, -77.40862144408482)
SketchLine_3 = Sketch_1.addLine(49.43499267560162, -77.40862144408482, 68.1964076145899, 55.3644688933709)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())

# Sketch creer sur la face de la box confondue avec le plan XOZ contenant un cercle inclus dans la face de la box
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_9_1/Left"))
SketchCircle_1 = Sketch_2.addCircle(3.420516499282639, 6.702654232424678, 2.660489383382409)
model.do()

# Create Wire & Extrusion
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_2"), model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), model.selection("EDGE", "Sketch_1/Edge-SketchLine_3")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection("EDGE", "PartSet/OZ"), 50, 0)

# Revolution of the circle
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("WIRE", "Sketch_2/Wire-SketchCircle_1_2f")], model.selection("EDGE", "PartSet/OZ"), 360, 0)

# Create MultiTranslations
MultiTranslation_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 15, 5)
MultiTranslation_2 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OX"), -15, 5)
MultiTranslation_3 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OX"), 15, -2)
MultiTranslation_4 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_4_1")], model.selection("EDGE", "InvalidName"), 15, -2)
MultiTranslation_5 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], model.selection("EDGE", "PartSet/OX"), 10, 5, model.selection("EDGE", "PartSet/OY"), 10, 5)
MultiTranslation_6 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_6_1")], model.selection("EDGE", "PartSet/OX"), 10, 5, model.selection("EDGE", "PartSet/OY"), -10, 5)
MultiTranslation_7 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_7_1")], model.selection("EDGE", "PartSet/OX"), 10, 5, model.selection("EDGE", "PartSet/OY"), 10, -2)
MultiTranslation_8 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_8_1")], model.selection("EDGE", "PartSet/OX"), 10, 5, model.selection("EDGE", "InvalidName"), 10, 5)
MultiTranslation_9 = model.addMultiTranslation(Part_1_doc, [model.selection("SHELL", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OZ"), 50, 10)
MultiTranslation_10 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Revolution_1_1"), model.selection("SOLID", "Box_9_1")], model.selection("EDGE", "PartSet/OY"), 20, 3)

model.do()
model.end()



# Checks
from GeomAPI import GeomAPI_Shape, GeomAPI_ShapeExplorer
from GeomAlgoAPI import *

model.testNbResults(MultiTranslation_1, 1)
model.testNbSubResults(MultiTranslation_1, [5])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.FACE, [30])
model.testHaveNamingFaces(MultiTranslation_1, model, Part_1_doc)

model.testNbResults(MultiTranslation_2, 1)
model.testNbSubResults(MultiTranslation_2, [5])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.FACE, [30])
model.testHaveNamingFaces(MultiTranslation_2, model, Part_1_doc)

model.testNbResults(MultiTranslation_5, 1)
model.testNbSubResults(MultiTranslation_5, [25])
model.testNbSubShapes(MultiTranslation_5, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiTranslation_5, GeomAPI_Shape.SOLID, [25])
model.testNbSubShapes(MultiTranslation_5, GeomAPI_Shape.FACE, [150])
model.testHaveNamingFaces(MultiTranslation_5, model, Part_1_doc)

model.testNbResults(MultiTranslation_6, 1)
model.testNbSubResults(MultiTranslation_6, [25])
model.testNbSubShapes(MultiTranslation_6, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiTranslation_6, GeomAPI_Shape.SOLID, [25])
model.testNbSubShapes(MultiTranslation_6, GeomAPI_Shape.FACE, [150])
model.testHaveNamingFaces(MultiTranslation_6, model, Part_1_doc)

####################################################################################################
# Test of MultiTranslation 10
model.testNbResults(MultiTranslation_10, 2)
model.testNbSubResults(MultiTranslation_10, [3, 3])

def testSubResult(theFeature, theModel, thePartDoc, resultIndex, nbSolidsExpected, nbFacesExpected):
    result = theFeature.results()[resultIndex].resultSubShapePair()[0]
    shape = result.shape()
    shapeExplorer = GeomAPI_ShapeExplorer(shape, GeomAPI_Shape.SOLID)
    nbSolids = 0
    while shapeExplorer.more():
        nbSolids += 1
        shapeExplorer.next()
    assert(nbSolids == nbSolidsExpected)

    shapeExplorer = GeomAPI_ShapeExplorer(shape, GeomAPI_Shape.FACE)
    nbFaces = 0
    selectionList = []
    while shapeExplorer.more():
        nbFaces += 1
        selection = theModel.selection(result, shapeExplorer.current()) # First argument should be result/sub-result, second is sub-shape on this result/sub-result
        selectionList.append(selection)
        shapeExplorer.next()
    assert(nbFaces == nbFacesExpected)
    # Create group with this selection list
    Group_1 = theModel.addGroup(thePartDoc, selectionList)
    theModel.do()
    theModel.end()

    # Now you can check that all selected shapes in group have right shape type and name.
    groupFeature = Group_1.feature()
    groupSelectionList = groupFeature.selectionList("group_list")
    theModel.end()
    assert(groupSelectionList.size() == len(selectionList))
    for index in range(0, groupSelectionList.size()):
        attrSelection = groupSelectionList.value(index)
        shape = attrSelection.value()
        name = attrSelection.namingName()
        assert(shape.isFace())
        assert(name != ""), "String empty"

testSubResult(MultiTranslation_10, model, Part_1_doc, 0, 3, 3)
testSubResult(MultiTranslation_10, model, Part_1_doc, 1, 3, 18)
####################################################################################################

model.testNbResults(MultiTranslation_3, 0)
assert(MultiTranslation_3.feature().error() == 'Multitranslation builder :: the number of copies for the first direction is null or negative.')

model.testNbResults(MultiTranslation_4, 0)
assert(MultiTranslation_4.feature().error() == 'Attribute "axis_first_dir" is not initialized.')

model.testNbResults(MultiTranslation_7, 0)
assert(MultiTranslation_7.feature().error() == 'Multitranslation builder :: the number of copies for the second direction is null or negative.')

model.testNbResults(MultiTranslation_8, 0)
assert(MultiTranslation_8.feature().error() == 'Attribute "axis_second_dir" is not initialized.')


# Multitranslation en erreur
#model.testNbResults(MultiTranslation_9, 1)
#model.testNbSubResults(MultiTranslation_9, [10])
#model.testNbSubShapes(MultiTranslation_9, GeomAPI_Shape.COMPOUND, [1])
#model.testNbSubShapes(MultiTranslation_9, GeomAPI_Shape.SOLID, [0])
#model.testNbSubShapes(MultiTranslation_9, GeomAPI_Shape.SHELL, [10])
#model.testNbSubShapes(MultiTranslation_9, GeomAPI_Shape.FACE, [30])
#model.testHaveNamingFaces(MultiTranslation_9, model, Part_1_doc)