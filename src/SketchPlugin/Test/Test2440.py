# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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
    Test2440.py
    Test case for issue #2440 "Can't validate coincidence between a line and an imported line"
"""


from SketchAPI import *

from salome.shaper import model

from math import fabs

import os

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
shape_path = os.path.join(os.getenv("DATA_DIR"), "Shapes", "Brep", "profile_top.brep")
Import_1 = model.addImport(Part_1_doc, shape_path)
model.do()
Import_1.setName("profile_top")
Import_1.result().setName("profile_top_1")

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(-0.5, 0, -0.1, 0.5)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "profile_top_1/Shape_2"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
model.do()

assert(SketchProjection_1.feature().error() == "")

SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
model.do()

assert(SketchConstraintCoincidence_1.feature().error() == "")
assert(Sketch_1.feature().error() == "")
assert(Sketch_1.solverError().value() == "")

REF_POINT = [-0.0215, 0.5492]
TOLERANCE = 1.e-7
assert(fabs(SketchLine_1.endPoint().x() - REF_POINT[0]) < TOLERANCE and fabs(SketchLine_1.endPoint().y() - REF_POINT[1]) < TOLERANCE)

model.end()
