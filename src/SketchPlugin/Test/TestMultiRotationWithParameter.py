# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

from GeomAPI import *
from GeomAlgoAPI import GeomAlgoAPI_ShapeTools
from SketchAPI import *
from math import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param = model.addParameter(Part_1_doc, "N", "3")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(30, 20, 10, 20)
SketchLine_2 = Sketch_1.addLine(10, 20, 10, 40)
SketchLine_3 = Sketch_1.addLine(10, 40, 30, 40)
SketchLine_4 = Sketch_1.addLine(30, 40, 30, 20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
model.do()
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_2.result()], SketchAPI_Point(SketchPoint_1).coordinates(), "360/N", "N")
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
model.do()

# change parameters
N =[6, 5]
for n in N:
    Param.setValue(n)
    model.do()
    Part_1_doc.setCurrentFeature(Extrusion_1.feature(), True)
    model.do()

# check coordinates of center of mass of each line
REF_DATA = [[-25.4415255451, 18.7810749942],
            [-25.7237275125, -18.3926573083],
            [9.54338762502, -30.1483623542],
            [31.6218654326, -0.240055331703]]
TOLERANCE = 1.e-7
aLines = SketchMultiRotation_1.rotated()
ind = 0
for lin in aLines:
    curShape = lin.feature().lastResult().shape()
    massCenter = GeomAlgoAPI_ShapeTools.centreOfMass(curShape)
    assert(fabs(massCenter.x() - REF_DATA[ind][0]) < TOLERANCE and fabs(massCenter.y() - REF_DATA[ind][1]) < TOLERANCE and massCenter.z() == 0), "({}, {}, {}) != ({}, {}, {})".format(massCenter.x(), massCenter.y(), massCenter.z(), REF_DATA[ind][0], REF_DATA[ind][1], 0.0)
    ind += 1

model.end()

assert(model.checkPythonDump())
