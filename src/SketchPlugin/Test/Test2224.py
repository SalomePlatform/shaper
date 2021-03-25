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
    Test2224.py
    Test case for issue #2224 "tangent arc problem when end point is on edge that is tangent to arc"
"""

from salome.shaper import model

from ModelAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(8.985073018276022, 21.83713948745567, 31.50462311471469, 21.83713948745567)
SketchArc_1 = Sketch_1.addArc(31.50462311471469, -6.977379981512561, 31.50462311471469, 21.83713948745567, 25.29690414212808, 21.16050830785517, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
model.do()
model.end()


# case 1: undo the error
model.begin()
aCoincidence = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
model.do()
model.end()
model.checkSketchErrorDegenerated(Sketch_1)
model.undo()
model.checkSketch(Sketch_1)

# case 2: remove degeneracy-producting constraint
model.begin()
aCoincidence = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
model.do()
model.checkSketchErrorDegenerated(Sketch_1)
Part_1_doc.removeFeature(aCoincidence.feature())
model.do()
model.end()
model.checkSketch(Sketch_1)

# case 3: remove degenerated edge
model.begin()
aCoincidence = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
model.do()
model.checkSketchErrorDegenerated(Sketch_1)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchArc_1.feature()]))
model.do()
model.end()
model.checkSketch(Sketch_1)
