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

# Checks selection of the whole featurte and move to the end of the group created on results of this feature..

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(11.02869497636673, 9.8764247475525, 3.312248077480665)
SketchCircle_2 = Sketch_1.addCircle(4.278198729238611, 4.677840612715367, 1.794922837237287)
model.do()
Copy_1 = model.addCopy(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], 1)
Copy_2 = model.addCopy(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], 1)
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(not aFactory.validate(Copy_1.feature()))
assert(not aFactory.validate(Copy_2.feature()))
