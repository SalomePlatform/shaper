# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    Check that there is no crash while changing the projecting feature
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20, -50, 70, 50)
model.do()

Sketch_2 = model.addSketch(partSet, model.defaultPlane("YOZ"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), True)
model.do()

# change the feature projected (no crash expected)
SketchProjection_1.setExternalFeature(model.selection("EDGE", "Sketch_1/SketchLine_1"))
model.do()

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbSubShapes(Sketch_2, GeomAPI_Shape.EDGE, [1])
