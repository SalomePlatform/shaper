// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "GeomValidators_Edge.h"
#include "GeomValidators_Tools.h"

#include <GeomAPI_Curve.h>
#include <Events_Error.h>
#include <ModelAPI_Result.h>

#include <StdSelect_TypeOfEdge.hxx>

#include <string>
#include <map>


typedef std::map<std::string, GeomValidators_Edge::TypeOfEdge> EdgeTypes;
static EdgeTypes MyEdgeTypes;

GeomValidators_Edge::TypeOfEdge GeomValidators_Edge::edgeType(const std::string& theType)
{
  if (MyEdgeTypes.size() == 0) {
    MyEdgeTypes["line"] = Line;
    MyEdgeTypes["circle"] = Circle;
  }
  std::string aType = std::string(theType.c_str());
  if (MyEdgeTypes.find(aType) != MyEdgeTypes.end())
    return MyEdgeTypes[aType];
  
  Events_Error::send("Edge type defined in XML is not implemented!");
  return AnyEdge;
}

bool GeomValidators_Edge::isValid(const AttributePtr& theAttribute,
                                  const std::list<std::string>& theArguments) const
{
  bool aValid = false;

  TypeOfEdge anEdgeType = AnyEdge;
  if (theArguments.size() == 1) {
    std::string anArgument = theArguments.front();
    anEdgeType = edgeType(anArgument);
  }

  ObjectPtr anObject = GeomValidators_Tools::getObject(theAttribute);
  if (anObject.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aResult.get() != NULL) {
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get() != NULL && aShape->isEdge()) {
        aValid = anEdgeType == AnyEdge;
        if (!aValid) {
          bool isCircle = GeomAPI_Curve(aShape).isCircle();
          aValid = (isCircle && anEdgeType == Circle) ||
                   (!isCircle && anEdgeType == Line);
        }
      }
    }
  }
  else {
    //AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  }
  return aValid;
}
