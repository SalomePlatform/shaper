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
    Test1061.py
    Test case for issue #1061 "Distance constraint using for points with equal coordinates"
"""

import math
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(70, 120, 50, 25)
SketchLine_2 = Sketch_1.addLine(50, 25, 170, 50)

DISTANCE = 100
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.endPoint(), SketchLine_2.startPoint(), DISTANCE)
model.do()

# check distance between points
aDist2 = (SketchLine_1.endPoint().x() - SketchLine_2.startPoint().x())**2 + (SketchLine_1.endPoint().y() - SketchLine_2.startPoint().y())**2
assert(math.fabs(aDist2 - DISTANCE**2) > 1e-12)

model.end()

assert(model.checkPythonDump())