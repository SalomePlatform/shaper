// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "GeomValidators_ShapeType.h"
#include "GeomValidators_Tools.h"

#include <GeomAPI_Curve.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Result.h>
#include "ModelAPI_AttributeRefAttr.h"

#include <Events_Error.h>

#include <string>
#include <map>


typedef std::map<std::string, GeomValidators_ShapeType::TypeOfShape> EdgeTypes;
static EdgeTypes MyEdgeTypes;

GeomValidators_ShapeType::TypeOfShape GeomValidators_ShapeType::shapeType(const std::string& theType)
{
  if (MyEdgeTypes.size() == 0) {
    MyEdgeTypes["vertex"] = Vertex;
    MyEdgeTypes["line"]   = Line;
    MyEdgeTypes["circle"] = Circle;
  }
  std::string aType = std::string(theType.c_str());
  if (MyEdgeTypes.find(aType) != MyEdgeTypes.end())
    return MyEdgeTypes[aType];
  
  Events_Error::send("Edge type defined in XML is not implemented!");
  return AnyShape;
}

bool GeomValidators_ShapeType::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments) const
{
  bool aValid = false;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
  for (; anIt != aLast && !aValid; anIt++) {
    aValid = isValidArgument(theAttribute, *anIt);
  }

  return aValid;
}

bool GeomValidators_ShapeType::isValidArgument(const AttributePtr& theAttribute,
                                               const std::string& theArgument) const
{
  bool aValid = false;
  TypeOfShape aShapeType = shapeType(theArgument);
  if (aShapeType == AnyShape)
    return true;

  ObjectPtr anObject = GeomValidators_Tools::getObject(theAttribute);
  if (anObject.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aResult.get() != NULL) {
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get() != NULL) {
        switch (aShapeType) {
          case Line:
            aValid = aShape->isEdge() && !GeomAPI_Curve(aShape).isCircle();
          break;
          case Circle:
            aValid = aShape->isEdge() && GeomAPI_Curve(aShape).isCircle();
          break;
          case Vertex:
            aValid = aShape->isVertex();
          break;
          default: break;
        }
      }
    }
  }
  else {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr.get() != NULL) {
      if (aShapeType == Vertex) {
        AttributePtr aRefAttr = anAttr->attr();
        aValid = aRefAttr.get() != NULL && aRefAttr->attributeType() == GeomDataAPI_Point2D::typeId();
      }
    }
  }
  return aValid;
}
