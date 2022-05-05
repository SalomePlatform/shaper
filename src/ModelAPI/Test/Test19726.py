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

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchCircle_1 = Sketch_1.addCircle(18.39021699364881, 11.50401662688171, 4.123179523509023)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
model.do()
Translation_1 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_1/")], axis = model.selection("EDGE", "Part_1/([Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/From_Face])([Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face])"), distance = 20, keepSubResults = True)
aName1 = Translation_1.axisObject().namingName()
model.end()

# update the extrusion in the part, so, outside the part selection must be moved in history
model.begin()
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], axis = model.selection("EDGE", "PartSet/OY"), angle = 45)
model.do()
aSession = ModelAPI_Session.get()
aSession.setActiveDocument(partSet)
model.end()

# check that name was changed to the rotation
aName2 = Translation_1.axisObject().namingName()
assert(aName1 != aName2)
assert(len(aName2) > 7)

assert(model.checkPythonDump())
