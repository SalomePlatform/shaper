// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "GeomValidators_ShapeType.h"
#include "GeomValidators_Tools.h"

#include <GeomAPI_Curve.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>

#include <Events_Error.h>

#include <string>
#include <map>


typedef std::map<std::string, GeomValidators_ShapeType::TypeOfShape> EdgeTypes;

GeomValidators_ShapeType::TypeOfShape GeomValidators_ShapeType::shapeType(const std::string& theType)
{
  static EdgeTypes MyShapeTypes;

  if (MyShapeTypes.size() == 0) {
    MyShapeTypes["empty"]  = Empty;
    MyShapeTypes["vertex"] = Vertex;
    MyShapeTypes["edge"]   = Edge;
    MyShapeTypes["line"]   = Line;
    MyShapeTypes["circle"] = Circle;
    MyShapeTypes["face"]   = Face;
    MyShapeTypes["solid"]  = Solid;
  }
  std::string aType = std::string(theType.c_str());
  if (MyShapeTypes.find(aType) != MyShapeTypes.end())
    return MyShapeTypes[aType];
  
  Events_Error::send("Shape type defined in XML is not implemented!");
  return AnyShape;
}

bool GeomValidators_ShapeType::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments,
                                       std::string& theError) const
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

  AttributeSelectionListPtr aListAttr = 
           std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (aListAttr.get()) {
    if(aListAttr->size() == 0 && shapeType(theArgument) == Empty) {
      return true;
    }
    for (int i = 0; i < aListAttr->size(); i++) {
      aValid = isValidAttribute(aListAttr->value(i), aShapeType);
      if (!aValid) // if at least one attribute is invalid, the result is false
        break;
    }
  }
  else {
    aValid = isValidAttribute(theAttribute, aShapeType);
  }
  return aValid;
}

bool GeomValidators_ShapeType::isValidAttribute(const AttributePtr& theAttribute,
                                                const TypeOfShape theShapeType) const
{
  bool aValid = false;

  std::string anAttributeType = theAttribute->attributeType();

  if (anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    GeomShapePtr aShape = anAttr->value();
    if (aShape.get())
      aValid = isValidShape(aShape, theShapeType);
    else
      aValid = isValidObject(anAttr->context(), theShapeType);
  }
  else if (anAttributeType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr.get() != NULL) {
      if (anAttr->isObject()) {
        aValid = isValidObject(anAttr->object(), theShapeType);
      }
      else if (theShapeType == Vertex) {
        AttributePtr aRefAttr = anAttr->attr();
        aValid = aRefAttr.get() != NULL && aRefAttr->attributeType() == GeomDataAPI_Point2D::typeId();
      }
    }
  }
  else if (anAttributeType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    if (anAttr.get() != NULL)
      aValid = isValidObject(anAttr->value(), theShapeType);
  }
  return aValid;
}

bool GeomValidators_ShapeType::isValidObject(const ObjectPtr& theObject,
                                             const TypeOfShape theShapeType) const
{
  bool aValid = false;
  if (theObject.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get() != NULL) {
      GeomShapePtr aShape = aResult->shape();
      aValid = isValidShape(aShape, theShapeType);
    }
  }
  return aValid;
}

bool GeomValidators_ShapeType::isValidShape(const GeomShapePtr theShape,
                                            const TypeOfShape theShapeType) const
{
  bool aValid = false;

  if (theShape.get() != NULL) {
    switch (theShapeType) {
      case Edge:
        aValid = theShape->isEdge();
      break;
      case Line:
        aValid = theShape->isEdge() && !GeomAPI_Curve(theShape).isCircle();
      break;
      case Circle:
        aValid = theShape->isEdge() && GeomAPI_Curve(theShape).isCircle();
      break;
      case Vertex:
        aValid = theShape->isVertex();
      break;
      case Solid:
        aValid = theShape->isSolid() || theShape->isCompoundOfSolids();
        break;
      case Face:
        aValid = theShape->isFace();
        break;
      case Compound:
        aValid = theShape->isCompound();
        break;
      default: break;
    }
  }
  return aValid;
}
