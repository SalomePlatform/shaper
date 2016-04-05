// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "GeomValidators_ShapeType.h"
#include "GeomValidators_Tools.h"

#include <GeomAPI_Curve.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Result.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>

#include <Events_Error.h>

#include <string>
#include <map>


typedef std::map<std::string, GeomValidators_ShapeType::TypeOfShape> EdgeTypes;

static EdgeTypes MyShapeTypes;
GeomValidators_ShapeType::TypeOfShape GeomValidators_ShapeType::shapeType(const std::string& theType)
{
  if (MyShapeTypes.size() == 0) {
    MyShapeTypes["empty"]  = Empty;
    MyShapeTypes["vertex"] = Vertex;
    MyShapeTypes["edge"]   = Edge;
    MyShapeTypes["line"]   = Line;
    MyShapeTypes["circle"] = Circle;
    MyShapeTypes["wire"]   = Wire;
    MyShapeTypes["face"]   = Face;
    MyShapeTypes["plane"]  = Plane;
    MyShapeTypes["shell"]  = Shell;
    MyShapeTypes["solid"]  = Solid;
  }
  std::string aType = std::string(theType.c_str());
  if (MyShapeTypes.find(aType) != MyShapeTypes.end())
    return MyShapeTypes[aType];

  Events_Error::send("Shape type defined in XML is not implemented!");
  return AnyShape;
}

std::string getShapeTypeDescription(const GeomValidators_ShapeType::TypeOfShape& theType)
{
  std::string aValue = "";

  if (MyShapeTypes.size() != 0) {
    std::map<std::string, GeomValidators_ShapeType::TypeOfShape>::const_iterator anIt = MyShapeTypes.begin(),
                                                                                 aLast = MyShapeTypes.end();
    for (; anIt != aLast; anIt++) {
      if (anIt->second == theType)
        aValue = anIt->first;
        break;
    }
  }
  return aValue;
}

bool GeomValidators_ShapeType::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments,
                                       std::string& theError) const
{
  bool aValid = false;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
  // returns true if the attribute satisfies at least one of given arguments
  for (; anIt != aLast; anIt++) {
    TypeOfShape aShapeType = shapeType(*anIt);
    // if arguments contain any shape type value, the validator returns true
    if (aShapeType == AnyShape) {
      aValid = true;
      break;
    }
    if (isValidAttribute(theAttribute, aShapeType, theError)) {
      aValid = true;
      break;
    }
  }
  if (!aValid && theError.empty()) {
    std::string aTypes;
    std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
    // returns true if the attribute satisfies at least one of given arguments
    for (; anIt != aLast; anIt++) {
      if (!aTypes.empty())
        aTypes += ", ";
    }
    theError = "It does not contain element with acceptable shape type. The type should be one of the next: "
               + aTypes;
  }

  return aValid;
}

bool GeomValidators_ShapeType::isValidAttribute(const AttributePtr& theAttribute,
                                                const TypeOfShape theShapeType,
                                                std::string& theError) const
{
  bool aValid = true;

  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    GeomShapePtr aShape = anAttr->value();
    if (aShape.get())
      aValid = isValidShape(aShape, theShapeType, theError);
    else
      aValid = isValidObject(anAttr->context(), theShapeType, theError);
  }
  else if (anAttributeType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr->isObject()) {
      aValid = isValidObject(anAttr->object(), theShapeType, theError);
    }
    else if (theShapeType == Vertex) {
      AttributePtr aRefAttr = anAttr->attr();
      if (!aRefAttr.get()){
        aValid = false;
        theError = "It has reference to an empty attribute";
      }
      else {
        std::string anAttributeType = aRefAttr->attributeType();
        aValid = anAttributeType == GeomDataAPI_Point2D::typeId();
        if (!aValid)
          theError = "Shape type is \"" + anAttributeType +
                     "\", it should be \"" + getShapeTypeDescription(theShapeType) + "\"";
      }
    }
  }
  else if (anAttributeType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    aValid = isValidObject(anAttr->value(), theShapeType, theError);
  }
  else if (anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr =
                          std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // the Empty value means that the attribute selection list is valid if it is empty
    if (aListAttr->size() == 0 && theShapeType == Empty) {
      return true;
    }
    aValid = false; // the list should have elements if the shape type is not Empty
    for (int i = 0; i < aListAttr->size(); i++) {
      aValid = isValidAttribute(aListAttr->value(i), theShapeType, theError);
      if (!aValid) // if at least one attribute is invalid, the result is false
        break;
    }
  }
  else {
    aValid = false;
    theError = "The attribute with the " + anAttributeType  + " type is not processed";
  }
  return aValid;
}

bool GeomValidators_ShapeType::isValidObject(const ObjectPtr& theObject,
                                             const TypeOfShape theShapeType,
                                             std::string& theError) const
{
  bool aValid = true;
  if (!theObject.get()) {
    if(theShapeType != Empty) {
      aValid = false;
      theError = "The object is empty";
    }
  }
  else {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if( theShapeType==Plane )
    {
      ResultConstructionPtr aResultConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
      FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
      const std::string& aKind = aFeature->getKind();
      return aResult.get() != NULL && aKind == "Plane";
    }
    if (!aResult.get()) {
      aValid = false;
      theError = "The result is empty";
    }
    else {
      aValid = isValidShape(aResult->shape(), theShapeType, theError);
    }
  }
  return aValid;
}

bool GeomValidators_ShapeType::isValidShape(const GeomShapePtr theShape,
                                            const TypeOfShape theShapeType,
                                            std::string& theError) const
{
  bool aValid = true;

  if (!theShape.get()) {
    aValid = false;
    theError = "The shape is empty";
  }
  else {
    switch (theShapeType) {
    case Vertex:
      aValid = theShape->isVertex();
      break;
    case Edge:
      aValid = theShape->isEdge();
      break;
    case Line:
      aValid = theShape->isEdge() && !GeomAPI_Curve(theShape).isCircle();
      break;
    case Circle:
      aValid = theShape->isEdge() && GeomAPI_Curve(theShape).isCircle();
      break;
    case Wire:
      aValid = theShape->shapeType() == GeomAPI_Shape::WIRE;
      break;
    case Face:
      aValid = theShape->isFace();
      break;
    case Shell:
      aValid = theShape->shapeType() == GeomAPI_Shape::SHELL;
      break;
    case Solid:
      aValid = theShape->isSolid() || theShape->isCompSolid() ||
               theShape->isCompoundOfSolids();
      break;
    case Compound:
      aValid = theShape->isCompound();
      break;
    default:
      aValid = false;
      break;
    }
  }
  return aValid;
}
