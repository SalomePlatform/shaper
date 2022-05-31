# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
    TestDistanceDump.py

    Check that distances set by parameters are dumped correctly
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Parameter_1 = model.addParameter(Part_1_doc, "a", "10")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(22.18754371333437, 31.96766851898132, 36.16301361286303, 24.640865178987)
SketchLine_2 = Sketch_1.addLine(36.16301361286303, 24.640865178987, 45.95667186595785, 26.6618199092043)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(45.95667186595785, 26.6618199092043, 55.95667186595785, 16.6618199092043)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_2.result(), Parameter_1.name().value(), True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), Parameter_1.name().value())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_2.endPoint(), SketchLine_3.endPoint(), Parameter_1.name().value())
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_2.endPoint(), SketchLine_3.endPoint(), Parameter_1.name().value())
model.do()
model.end()

def assertEqual(str1, str2):
    assert(str1 == str2), "{} != {}".format(str1, str2)

assertEqual(SketchConstraintDistance_1.feature().real("ConstraintValue").text(), Parameter_1.name().value())
assertEqual(SketchConstraintLength_1.feature().real("ConstraintValue").text(), Parameter_1.name().value())
assertEqual(SketchConstraintDistanceHorizontal_1.feature().real("DistanceValue").text(), Parameter_1.name().value())
assertEqual(SketchConstraintDistanceVertical_1.feature().real("DistanceValue").text(), Parameter_1.name().value())

assert(model.checkPythonDump())
