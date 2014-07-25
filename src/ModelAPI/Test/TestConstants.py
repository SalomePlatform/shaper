#===============================================================================
# Test ModelAPI static methods
#===============================================================================
from ModelAPI import *
assert (ModelAPI_Feature.group() == "Features")
assert (ModelAPI_AttributeDocRef.type() == "DocRef")
assert (ModelAPI_AttributeDouble.type() == "Double")
assert (ModelAPI_AttributeReference.type() == "Reference")
assert (ModelAPI_AttributeRefAttr.type() == "RefAttr")
assert (ModelAPI_AttributeRefList.type() == "RefList")