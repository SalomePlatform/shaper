import GeomAlgoAPI

# A python implementation of shapeInfo functionality.
# By the given shape it will provide information, similar to data, displayed in the second part
# of Inspection Panel on selection of face, edge or vertex.
# The method returns a tuple with same data and in the same order as it is presented in the
# Information Panel: type of shape, then values (may be not-numerical, like Boolean 'Closed' for Polygon).
def shapeInfo(theShape):
  anInfo = GeomAlgoAPI.GeomAlgoAPI_ShapeInfo(theShape)
  aVals = anInfo.values()
  aResList = [anInfo.shapeType()]
  for aVal in aVals:
    if aVal.type() == 0:
      aResList.append(aVal.string())
    elif aVal.type() == 1:
      aResList.append(aVal.real())
    elif aVal.type() == 2:
      aResList.append(aVal.boolean())
  return tuple(aResList)
