"""
      TestDocument.py
      Unit test for Model_Document/ModelAPI_Document class
      
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
# from GeomDataAPI import *
# from GeomAlgoAPI import *

__updated__ = "2014-12-26"

#=========================================================================
# Creation and activation of documents
#=========================================================================
aSession = ModelAPI_Session.get()
# TODO: enable this assertion:
assert(aSession.moduleDocument())
assert(aSession.moduleDocument().id() == "root")
assert(aSession.moduleDocument().kind() == "PartSet")
assert(aSession.hasModuleDocument())
# Create a new document
aSession.startOperation()
aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()

assert(aSession.activeDocument())
assert(aSession.activeDocument().id() == "Part_1")
assert(aSession.activeDocument().kind() == "Part")
# Activate root doc
aRootDoc = aSession.document("root")
assert(aRootDoc)
aSession.startOperation()
aSession.setActiveDocument(aRootDoc, False)
aSession.finishOperation()
assert(aSession.activeDocument())
assert(aSession.activeDocument().id() == "root")
# check all opened docs
allDocsList = aSession.allOpenedDocuments()
assert(len(allDocsList) != 0)
# Activate Part_1 doc back for further testing
aSession.startOperation()
aSession.setActiveDocument(aSession.document("Part_1"), False)
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
aFeatureData.real("x").setValue(15.)
aFeatureData.real("y").setValue(10.)
aFeatureData.real("z").setValue(20.)
aSession.finishOperation()
assert(aPart.size("Features") == 1)
# Duplicate the document
assert(aSession.moduleDocument().size("Parts") == 1)
aSession.startOperation()
aPart.addFeature("Duplicate")
aSession.finishOperation()
assert(aSession.moduleDocument().size("Parts") == 2)
aCopyOfPart = aSession.activeDocument()
assert(aCopyOfPart.id() == "Part_2")
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
assert(aSession.moduleDocument().size("Parts") == 1)
assert(aSession.activeDocument().id() == aSession.moduleDocument().id())
# Remove another one document
aSession.startOperation()
aDoc2 = aSession.document("Part_2")
aSession.setActiveDocument(aDoc2, False)
aDoc2.addFeature("Remove")
aSession.finishOperation()
assert(aSession.moduleDocument().size("Parts") == 0)
assert(aSession.activeDocument())
