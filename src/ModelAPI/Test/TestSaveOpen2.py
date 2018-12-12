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

# -*- coding: utf-8 -*-

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")])
Field_1 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]")])
Field_1.addStep(0, 5, [[0], [3], [4]])
model.do()
Translation_1 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_1/")], model.selection("EDGE", "OY"), 20)
model.end()

# check save document in a current folder
aSession = ModelAPI_Session.get()
aFiles = StringList()
aSession.save(".", aFiles)
assert(len(aFiles) == 3)

# check open of the same document
assert(aSession.load(".") == False) # error because this document is already opened

# close the current document
aSession.closeAll()

# open again: it must be correct now
assert(aSession.load("."))

# activate the Part of session
model.begin()
partSet = model.moduleDocument()
assert(partSet.size("Features") == 2)
aPart = objectToFeature(partSet.object("Features", 0))
aPartResult = modelAPI_ResultPart(aPart.results()[0])
aPartResult.activate()
aPartDoc = aPartResult.partDoc()
aSession.setActiveDocument(aPartDoc, True)
model.do()
# check the field result data
aFieldFeature = aPartDoc.objectByName("Features", "Field_1")
aFieldResult = modelAPI_ResultField(objectToFeature(aFieldFeature).results()[0])
assert(aFieldResult.textLine(0) == "5")
aSession.setActiveDocument(partSet, True)
aTranslation = objectToFeature(partSet.objectByName("Features", "Translation_1"))
partSet.setCurrentFeature(aTranslation, True)
model.end()

# check the translation feature has a correct result
assert(not aTranslation.firstResult().shape().isNull())