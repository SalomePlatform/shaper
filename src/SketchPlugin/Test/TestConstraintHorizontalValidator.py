# Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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
    TestConstraintHorizontalValidator.py
    It tests validation of horizontal and vertical segments in H and V constraints to avoid
    selection segments that already have one of these constraint"
"""

#=========================================================================
# of the test
#=========================================================================
from salome.shaper import model
from ModelAPI import *
import math
#=========================================================================
# Creation of a part
#=========================================================================
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
#=========================================================================
# Creation of a sketch
#=========================================================================
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(20, 20, 40, 80)
SketchLine_2 = Sketch_1.addLine(40, 80, 60, 40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
model.do()
#=========================================================================
# Checking that sketch and constraints become invalid when to one line
# two horizontal/vertical constraints are applied in any combination
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Sketch_1.feature()))
assert(Sketch_1.feature().error() != '')
#=========================================================================
# Remove duplicated Vertical constraints
#=========================================================================
Part_1_doc.removeFeature(SketchConstraintVertical_1.feature())
assert(aFactory.validate(Sketch_1.feature()))
model.do()
#=========================================================================
# Checking that after excess constraints are removed or undone,
# sketch becomes valid.
#=========================================================================
assert(aFactory.validate(Sketch_1.feature()))
assert(Sketch_1.feature().error() == '')

assert(model.checkPythonDump())
