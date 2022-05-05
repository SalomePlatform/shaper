# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
    Test2034_2.py
    Test case for issue #2034 "Conflicting constraint not raised (but found by planegcs and shaper's dof computation)"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(2.537917624456907, 8.497392663945117, 15)
SketchLine_1 = Sketch_1.addLine(-12.22290362866818, 11.16537990805196, 2.537924341198721, 8.497429825311462)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchLine_2 = Sketch_1.addLine(2.537924341198721, 8.497429825311462, 17.29875231106562, 5.829479742570965)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 15)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 15)
model.do()

# check sketch is correct
assert(Sketch_1.solverError().value() == "")

# add coincidence between center of circle and the end point of line
Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.endPoint())
model.do()
assert(Sketch_1.solverError().value() != "")


model.end()
