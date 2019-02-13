## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
    Test2860.py
    Test case for issue #2650 "setAngleComplementary becomes setAngle in BARCOM"
    Test case for issue #2860 "Error in python dump reload because of setAngleComplementary"
"""

from salome.shaper import model
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-9.326798599059195, -0.7578903795715272, 33.09960827213365, 41.66851649162133)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 60)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_1.result(), SketchLine_2.result(), 45)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_1.endPoint(), 5)
model.do()
model.end()

assert(model.checkPythonDump())
