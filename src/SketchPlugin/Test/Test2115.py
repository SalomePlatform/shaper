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

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Create a centered rectangle (from the 2115 issue description)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(132.9002660287441, 132.9002660287441, -132.9002660287441, 132.9002660287441)
SketchLine_2 = Sketch_1.addLine(-132.9002660287441, 132.9002660287441, -132.9002660287441, -132.9002660287441)
SketchLine_3 = Sketch_1.addLine(-132.9002660287441, -132.9002660287441, 132.9002660287441, -132.9002660287441)
SketchLine_4 = Sketch_1.addLine(132.9002660287441, -132.9002660287441, 132.9002660287441, 132.9002660287441)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-132.9002660287441, -132.9002660287441, 132.9002660287441, 132.9002660287441)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(-132.9002660287441, 132.9002660287441, 132.9002660287441, -132.9002660287441)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_6.endPoint())
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_5.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_6.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())

# create a circle by 3 points of this rectangle (to make the last as a line later)
circle = Sketch_1.addCircle(SketchLine_2.startPoint().pnt(), SketchLine_1.startPoint().pnt(), SketchLine_3.endPoint().pnt())

# create by the real references
circle.feature().refattr("first_point_ref").setAttr(SketchLine_2.startPoint())
circle.feature().refattr("second_point_ref").setAttr(SketchLine_1.startPoint())
circle.feature().refattr("third_point_ref").setObject(SketchLine_6.feature().firstResult())
# here there was a crash in the issue (actually, sketch is overconstrained, so, assertion is not used, just call for validators)
aFactory = ModelAPI_Session.get().validators()
aFactory.validate(circle.feature())
model.end()
