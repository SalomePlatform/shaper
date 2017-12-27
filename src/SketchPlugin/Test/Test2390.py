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
    Test2390.py
    Test case for issue #2390 "Revolution become invalid after changing parameter"
"""


from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Param_Width = model.addParameter(Part_1_doc, "Width", "64")
Param_Drill2Loc = model.addParameter(Part_1_doc, "Drill2_Loc", "6.5")
Param_Drill2Radius = model.addParameter(Part_1_doc, "Drill2_Radius", "1.4")
Param_Drill2InnerRadius = model.addParameter(Part_1_doc, "Drill2_InnerRadius", "0.5")
Param_Drill2DepthMin = model.addParameter(Part_1_doc, "Drill2_DepthMin", "6.5")
Param_Drill2DepthMax = model.addParameter(Part_1_doc, "Drill2_DepthMax", "7.5")

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(64, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 36)
SketchLine_3 = Sketch_1.addLine(0, 36, 64, 36)
SketchLine_4 = Sketch_1.addLine(64, 36, 64, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchLine_3.endPoint(), "Width")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 36)
SketchLine_5 = Sketch_1.addLine(22, 36, 22, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_3.result())
SketchLine_6 = Sketch_1.addLine(32, 36, 32, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_2.endPoint(), SketchLine_5.startPoint(), "Width/32*11")
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchLine_6.startPoint(), SketchLine_3.endPoint(), "Width/2")
model.do()

Sketch_4 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_52 = Sketch_4.addLine(28.5, -19, 28.5, 0)
SketchProjection_12 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_53 = SketchProjection_12.createdFeature()
SketchConstraintCoincidence_47 = Sketch_4.setCoincident(SketchLine_52.endPoint(), SketchLine_53.result())
SketchLine_54 = Sketch_4.addLine(28.5, 0, 29, 0)
SketchConstraintCoincidence_48 = Sketch_4.setCoincident(SketchLine_52.endPoint(), SketchLine_54.startPoint())
SketchConstraintCoincidence_49 = Sketch_4.setCoincident(SketchLine_54.endPoint(), SketchLine_53.result())
SketchLine_55 = Sketch_4.addLine(29, 0, 29, -11.5)
SketchConstraintCoincidence_50 = Sketch_4.setCoincident(SketchLine_54.endPoint(), SketchLine_55.startPoint())
SketchLine_56 = Sketch_4.addLine(28.5, -19, 29.9, -19)
SketchConstraintCoincidence_51 = Sketch_4.setCoincident(SketchLine_52.startPoint(), SketchLine_56.startPoint())
SketchLine_57 = Sketch_4.addLine(29.9, -19, 29.4, -18.5)
SketchConstraintCoincidence_52 = Sketch_4.setCoincident(SketchLine_56.endPoint(), SketchLine_57.startPoint())
SketchLine_58 = Sketch_4.addLine(29.4, -18.5, 29.4, -12.5)
SketchConstraintCoincidence_53 = Sketch_4.setCoincident(SketchLine_57.endPoint(), SketchLine_58.startPoint())
SketchArc_2 = Sketch_4.addArc(27.95, -12.5, 29.4, -12.5, 29, -11.5, False)
SketchConstraintCoincidence_54 = Sketch_4.setCoincident(SketchLine_58.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_55 = Sketch_4.setCoincident(SketchLine_55.endPoint(), SketchArc_2.endPoint())
SketchConstraintVertical_15 = Sketch_4.setVertical(SketchLine_52.result())
SketchConstraintVertical_16 = Sketch_4.setVertical(SketchLine_55.result())
SketchConstraintVertical_17 = Sketch_4.setVertical(SketchLine_58.result())
SketchConstraintHorizontal_11 = Sketch_4.setHorizontal(SketchLine_56.result())
SketchProjection_13 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_5e"), False)
SketchPoint_4 = SketchProjection_13.createdFeature()
SketchConstraintDistanceHorizontal_17 = Sketch_4.setHorizontalDistance(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_52.endPoint(), "Drill2_Loc")
SketchConstraintLength_3 = Sketch_4.setLength(SketchLine_54.result(), "Drill2_InnerRadius")
SketchProjection_14 = Sketch_4.addProjection(model.selection("EDGE", "Sketch_1/Edge-SketchLine_3"), False)
SketchLine_59 = SketchProjection_14.createdFeature()
SketchConstraintAngle_6 = Sketch_4.setAngle(SketchLine_57.result(), SketchLine_56.result(), 45)
SketchConstraintLength_4 = Sketch_4.setLength(SketchLine_56.result(), "Drill2_Radius")
SketchConstraintMirror_2 = Sketch_4.addMirror(SketchLine_53.result(), [SketchLine_59.result()])
[SketchLine_60] = SketchConstraintMirror_2.mirrored()
SketchLine_60.setAuxiliary(True)
SketchConstraintCoincidence_56 = Sketch_4.setCoincident(SketchLine_56.startPoint(), SketchLine_60.result())
SketchConstraintDistance_12 = Sketch_4.setDistance(SketchArc_2.endPoint(), SketchLine_60.result(), "Drill2_DepthMax", True)
SketchConstraintDistance_13 = Sketch_4.setDistance(SketchLine_58.endPoint(), SketchLine_60.result(), "Drill2_DepthMin", True)
SketchConstraintDistance_14 = Sketch_4.setDistance(SketchLine_57.endPoint(), SketchLine_60.result(), 0.5, True)
SketchConstraintTangent_3 = Sketch_4.setTangent(SketchLine_58.result(), SketchArc_2.results()[1])
model.do()

Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_7r-SketchLine_9r-SketchLine_10r-SketchLine_11f-SketchLine_12f-SketchLine_13f-SketchArc_1_2f")], model.selection("EDGE", "Sketch_2/Edge-SketchLine_7"), 360, 0)
model.do()

from GeomAPI import GeomAPI_Shape

# Check Revolution_1 is correct
model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [36])
model.testResultsVolumes(Revolution_1, [41.691543241529835484016075497])

# Update Width parameter
Param_Width.setValue(100)
model.do()

# compare only 5 digits in the volume, due to small deviations
SIGNIFICANT_DIGITS = 5

# Check Revolution_1 is correct
model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [36])
model.testResultsVolumes(Revolution_1, [41.691543241529835484016075497], SIGNIFICANT_DIGITS)

# Update Width parameter once again
Param_Width.setValue(150)
model.do()

# Check Revolution_1 is correct
model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [36])
model.testResultsVolumes(Revolution_1, [41.691543241529835484016075497], SIGNIFICANT_DIGITS)

model.end()

# TODO: uncomment following line when issue #2389 is fixed
#assert(model.checkPythonDump())