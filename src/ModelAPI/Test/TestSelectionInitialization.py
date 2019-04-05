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

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 1, 2, 3)
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "Point_1"), 10)
model.end()

# save document in a current folder
aSession = ModelAPI_Session.get()
aFiles = StringList()
aSession.save(".", aFiles)
# close the current document
aSession.closeAll()
# open the saved document
assert(aSession.load("."))

# activate the Part of session
model.begin()
partSet = model.moduleDocument()
assert(partSet.size("Features") == 1)
aPart = objectToFeature(partSet.object("Features", 0))
aPartResult = modelAPI_ResultPart(aPart.results()[0])
aPartResult.activate()
aPartDoc = aPartResult.partDoc()
aSession.setActiveDocument(aPartDoc, True)
model.end()

# check the sphere location (it should not be "Origin" - default one)
aSphere = objectToFeature(aPartDoc.objectByName("Features", "Sphere_1"))
assert(aSphere.selection("center_point").namingName() == "Point_1")
