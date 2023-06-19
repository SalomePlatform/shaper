# Copyright (C) 2014-2023  CEA, EDF
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
      TestDocument.py
      Unit test for Model_Document/ModelAPI_Document class

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *

__updated__ = "2014-12-26"

#=========================================================================
# Creation and activation of documents
#=========================================================================
aSession = ModelAPI_Session.get()
assert(aSession.moduleDocument())
assert(aSession.moduleDocument().id() == 0)
assert(aSession.moduleDocument().kind() == "PartSet")
assert(aSession.hasModuleDocument())
# Create a new document
aSession.startOperation()
aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()

assert(aSession.activeDocument())
assert(aSession.activeDocument().id() == 1)
assert(aSession.activeDocument().kind() == "Part")
# Activate root doc
aRootDoc = aSession.document(0)
assert(aRootDoc)
aSession.startOperation()
aSession.setActiveDocument(aRootDoc, False)
aSession.finishOperation()
assert(aSession.activeDocument())
assert(aSession.activeDocument().id() == 0)
# check all opened docs
allDocsList = aSession.allOpenedDocuments()
assert(len(allDocsList) != 0)
# Activate Part_1 doc back for further testing
aSession.startOperation()
aSession.setActiveDocument(aSession.document(1), False)
aSession.finishOperation()
#=========================================================================
# Duplication of a document
#=========================================================================
aPart = aSession.activeDocument()
assert(aPart.size("Features") == 0)
# Create a point in the current document to check if it is duplicated
aSession.startOperation()
aFeature = aPart.addFeature("Point")
aFeatureData = aFeature.data()
assert(aFeatureData is not None)
geomDataAPI_Point(aFeatureData.attribute("point3d")).setValue(15., 10., 20.)
aSession.finishOperation()
assert(aPart.size("Features") == 1)
# Duplicate the document
assert(aSession.moduleDocument().size("Parts") == 1)
aSession.startOperation()
aPart.addFeature("Duplicate")
aSession.finishOperation()
assert(aSession.moduleDocument().size("Parts") == 2)
aCopyOfPart = aSession.activeDocument()
assert(aCopyOfPart.id() == 2)
assert(aCopyOfPart.kind() == "Part")
assert(aCopyOfPart.size("Features") == 1)
assert(aCopyOfPart != aPart)
#=========================================================================
# Remove document
#=========================================================================
assert(aSession.moduleDocument().size("Parts") == 2)
aSession.startOperation()
aPart.addFeature("Remove")
aSession.finishOperation()
# First part is deleted, but active is Part_2, so, it is still active
assert(aSession.moduleDocument().size("Parts") == 1)
assert(aSession.activeDocument().id() == aCopyOfPart.id())
# Remove another one document
aSession.startOperation()
aDoc2 = aSession.document(2)
aSession.setActiveDocument(aDoc2, False)
aDoc2.addFeature("Remove")
aSession.finishOperation()
assert(aSession.moduleDocument().size("Parts") == 0)
assert(aSession.activeDocument())

from salome.shaper import model
assert(model.checkPythonDump())
