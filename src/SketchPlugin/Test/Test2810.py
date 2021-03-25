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
    Test2810.py
    Test case for issue #2810 "crash creating arc fillet"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(10, 10, 11, 10, 10, 11, False)
SketchLine_1 = Sketch_1.addLine(12, 10, 12, 9)
model.do()

Sketch_1.setFixed(SketchArc_1.results()[1])
Sketch_1.setFixed(SketchLine_1.result())

SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.startPoint())
model.do()

SketchFillet = Sketch_1.setFillet(SketchArc_1.startPoint())
model.do()

assert(Sketch_1.feature().error() != "")

model.end()
