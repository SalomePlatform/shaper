# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
    Test2741.py
    Test case for issue #2741 "Undo of distance argument change produces crash"
"""

from salome.shaper import model
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-34.48932587013569, -5.994823429605995, -23.03828378136105, 27.99739301876914)
SketchLine_2 = Sketch_1.addLine(-23.03828378136105, 27.99739301876914, -14.34307552407934, -3.849202930145424)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_2.result(), 20, True)
model.do()

# Change point-line distance constraint to point-point distance
SketchAPI_Constraint(SketchConstraintDistance_1).setEntityB(SketchLine_2.endPoint())
model.do()
model.end()

# Check no crash on undo distance changing
model.undo()
