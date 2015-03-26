#===============================================================================
# Test ModelAPI static methods
#===============================================================================
from ModelAPI import *
assert (ModelAPI_Feature.group() == "Features")
assert (ModelAPI_AttributeDocRef.typeId() == "DocRef")
assert (ModelAPI_AttributeDouble.typeId() == "Double")
assert (ModelAPI_AttributeReference.typeId() == "Reference")
assert (ModelAPI_AttributeRefAttr.typeId() == "RefAttr")
assert (ModelAPI_AttributeRefList.typeId() == "RefList")