# Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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
    Test3019.py
    Test case for issue #3019 "Error when load python script"
"""

from salome.shaper import model
from SketchAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "k", "30")
model.addParameter(Part_1_doc, "M", "10")
model.addParameter(Part_1_doc, "h", "20")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(15, -8.660254037843767, 15, 8.660254037843762)
SketchLine_2 = Sketch_1.addLine(15, 8.660254037843762, 0, 0)
SketchLine_2.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 0, 15, -8.660254037843767)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_1.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_1.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_1.result(), "k/2", True)
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchLine_3.startPoint(), 360, 6, True)
[SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = SketchMultiRotation_1.rotated()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "M")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_3.startPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchCircle_1_2r")], model.selection(), "h", 0)
Point_2 = model.addPoint(Part_1_doc, "0", "0", "h/2")
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "Point_1"), 19.5)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Sphere_1_1")])
Folder_1 = model.addFolder(Part_1_doc, Sketch_1, Extrusion_1)
Folder_2 = model.addFolder(Part_1_doc, Point_2, Sphere_1)
model.end()

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [21])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [114])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [228])
model.testResultsVolumes(Common_1, [9302.86764641653])

assert(model.checkPythonDump())
