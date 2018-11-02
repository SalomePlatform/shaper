## Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

from SketchAPI import *
from GeomAPI import GeomAPI_Shape
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(40, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 40)
SketchLine_3 = Sketch_1.addLine(0, 40, 40, 40)
SketchLine_4 = Sketch_1.addLine(40, 40, 40, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 40)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r_wire")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_5 = Sketch_2.addLine(20, 0, 0, 0)
SketchLine_6 = Sketch_2.addLine(0, 0, 0, 20)
SketchLine_7 = Sketch_2.addLine(0, 20, 20, 20)
SketchLine_8 = Sketch_2.addLine(20, 20, 20, 0)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_2"), False)
SketchLine_9 = SketchProjection_2.createdFeature()
SketchConstraintMiddle_1 = Sketch_2.setMiddlePoint(SketchLine_5.startPoint(), SketchLine_9.result())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_9).startPoint(), SketchLine_5.endPoint())
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchLine_5.result(), SketchLine_6.result())
SketchLine_10 = Sketch_2.addLine(20, 20, 20, 40)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_2.addLine(20, 40, 0, 40)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_2.addLine(0, 40, 0, 20)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_12.endPoint())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintEqual_3 = Sketch_2.setEqual(SketchLine_6.result(), SketchLine_12.result())
model.do()
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r_wire")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OZ"), 270, 2)
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "AngularCopy_1_1_1"), model.selection("FACE", "AngularCopy_1_1_2")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_7f-SketchLine_10f-SketchLine_11f-SketchLine_12f")], model.selection(), 40, 0)
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_13 = Sketch_3.addLine(40, 0, 20, 0)
SketchLine_14 = Sketch_3.addLine(20, 0, 20, 40)
SketchLine_15 = Sketch_3.addLine(20, 40, 40, 40)
SketchLine_16 = Sketch_3.addLine(40, 40, 40, 0)
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_7 = Sketch_3.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_8 = Sketch_3.setVertical(SketchLine_16.result())
SketchProjection_3 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_2/SketchLine_5_StartVertex"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchAPI_Point(SketchPoint_2).coordinates())
SketchProjection_4 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_2/SketchLine_10_EndVertex"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchAPI_Point(SketchPoint_3).coordinates())
SketchProjection_5 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_2_EndVertex"), False)
SketchPoint_4 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchLine_13.startPoint(), SketchAPI_Point(SketchPoint_4).coordinates())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_16r-SketchLine_15r-SketchLine_14r-SketchLine_13r")], model.selection(), 20, 0)
Box_1 = model.addBox(Part_1_doc, 20, 20, 20)
model.do()

def createCompSolidStepByStep(theSelection, theExpectedType):
    CompSolid = model.addCompSolid(Part_1_doc, [theSelection[0]])
    aList = []
    for selection, expected in zip(theSelection, theExpectedType):
        aList.append(selection)
        CompSolid.setBase(aList)
        if expected == GeomAPI_Shape.SHAPE:
            assert(len(CompSolid.results()) == 0), "FAILED: CompSolid should not be built"
        else:
            assert(len(CompSolid.results()) > 0), "FAILED: CompSolid is not built"
            resultType = CompSolid.results()[0].resultSubShapePair()[0].shape().shapeType()
            assert(resultType == expected), "FAILED: Result has unexpected type"
    # remove solid if it is not built
    if len(CompSolid.results()) == 0:
        Part_1_doc.removeFeature(CompSolid.feature())
        return None
    return CompSolid

# =============================================================================
# Test 1. Build compsolid cointaining 2 solids
# =============================================================================
boundaries1 = [model.selection("FACE", "Face_1_1"),
               model.selection("SHELL", "Shell_1_1"),
               model.selection("FACE", "Sketch_3/Face-SketchLine_16r-SketchLine_15r-SketchLine_14r-SketchLine_13r"),
               model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_7"),
               model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_3/SketchLine_14"),
               model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_3/SketchLine_15"),
               model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_3/SketchLine_16"),
               model.selection("FACE", "Extrusion_2_1/To_Face"),
               model.selection("FACE", "Box_1_1/Front")]
expectType1 = [GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SOLID,
               GeomAPI_Shape.COMPSOLID]
CompSolid_1 = createCompSolidStepByStep(boundaries1, expectType1)

model.checkResult(CompSolid_1, model, 1, [2], [2], [13], [54], [108])
model.testHaveNamingSubshapes(CompSolid_1, model, Part_1_doc)

# =============================================================================
# Test 2. Build compsolid containing 3 solids
# =============================================================================
boundaries2 = [model.selection("FACE", "Face_1_1"),
               model.selection("SHELL", "Shell_1_1"),
               model.selection("FACE", "Sketch_3/Face-SketchLine_16r-SketchLine_15r-SketchLine_14r-SketchLine_13r"),
               model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_7"),
               model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_10"),
               model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_3/SketchLine_15"),
               model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_3/SketchLine_16"),
               model.selection("FACE", "Extrusion_2_1/To_Face"),
               model.selection("FACE", "Box_1_1/Front"),
               model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_3/SketchLine_14"),
               model.selection("SOLID", "Extrusion_1_1")]
expectType2 = [GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SHAPE,
               GeomAPI_Shape.SOLID,
               GeomAPI_Shape.COMPSOLID,
               GeomAPI_Shape.COMPSOLID]
CompSolid_2 = createCompSolidStepByStep(boundaries2, expectType2)

model.checkResult(CompSolid_2, model, 1, [3], [3], [21], [88], [176])
model.testHaveNamingSubshapes(CompSolid_2, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
