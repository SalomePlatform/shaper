# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

from ModelAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "Wall_R", "6.188")
model.addParameter(Part_1_doc, "Wall_T1", "0.089")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(0, 2.424999999999997)
SketchLine_1 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result())
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchPoint_1.coordinates())
SketchLine_2 = Sketch_1.addLine(6.277, 3.700188311077954, 6.277, -1.894463229482514)
SketchLine_2.setAuxiliary(True)
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_2.result(), "Wall_R+Wall_T1")
model.end()

# check that resulting sketch is not valid (since PlanGCS 0.18)
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Sketch_1.feature()) == False)
