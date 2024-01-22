// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "GeomValidators_ShapeType.h"
#include "GeomValidators_Tools.h"

#include <GeomAPI_Curve.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>

#include <Events_InfoMessage.h>

#include <algorithm>
#include <string>
#include <map>


typedef std::map<std::string, GeomValidators_ShapeType::TypeOfShape> EdgeTypes;

static EdgeTypes MyShapeTypes;
GeomValidators_ShapeType::TypeOfShape
  GeomValidators_ShapeType::shapeType(const std::string& theType)
{
  if (MyShapeTypes.size() == 0) {
    MyShapeTypes["empty"]     = Empty;
    MyShapeTypes["vertex"]    = Vertex;
    MyShapeTypes["vertices"]  = Vertex;
    MyShapeTypes["edge"]      = Edge;
    MyShapeTypes["edges"]     = Edge;
    MyShapeTypes["line"]      = Line;
    MyShapeTypes["circle"]    = Circle;
    MyShapeTypes["wire"]      = Wire;
    MyShapeTypes["face"]      = Face;
    MyShapeTypes["faces"]     = Face;
    MyShapeTypes["plane"]     = Plane;
    MyShapeTypes["shell"]     = Shell;
    MyShapeTypes["solid"]     = Solid;
    MyShapeTypes["solids"]    = Solid;
    MyShapeTypes["compsolid"] = CompSolid;
    MyShapeTypes["compound"]  = Compound;
  }
  std::string aType = std::string(theType.c_str());
  std::transform(aType.begin(), aType.end(), aType.begin(),
                 [](char c) { return static_cast<char>(::tolower(c)); });
  if (MyShapeTypes.find(aType) != MyShapeTypes.end())
    return MyShapeTypes[aType];

// LCOV_EXCL_START
  //Events_InfoMessage("Shape type defined in XML is not implemented!").send();
  return AnyShape;
// LCOV_EXCL_STOP
}

// LCOV_EXCL_START
std::string getShapeTypeDescription(const GeomValidators_ShapeType::TypeOfShape& theType)
{
  std::string aValue = "";

  if (MyShapeTypes.size() != 0) {
    std::map<std::string, GeomValidators_ShapeType::TypeOfShape>::const_iterator
      anIt = MyShapeTypes.begin(), aLast = MyShapeTypes.end();
    for (; anIt != aLast; anIt++)
      if (anIt->second == theType) {
        aValue = anIt->first;
        break;
      }
  }
  return aValue;
}
// LCOV_EXCL_STOP

bool GeomValidators_ShapeType::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments,
                                       Events_InfoMessage& theError) const
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
    // returns true if the attribute satisfies at least one of given arguments
    for (anIt = theArguments.begin(); anIt != aLast; anIt++) {
      if (!aTypes.empty())
        aTypes += ", ";
      aTypes += *anIt;
    }
    theError = "It does not contain element with acceptable shape type. "
               "The type should be one of the next: %1";
    theError.arg(aTypes);
  }

  return aValid;
}

bool GeomValidators_ShapeType::isValidAttribute(const AttributePtr& theAttribute,
                                                const TypeOfShape theShapeType,
                                                Events_InfoMessage& theError) const
{
  bool aValid = true;

  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    GeomShapePtr aShape = anAttr->value();
    if (aShape.get())
      aValid = isValidShape(aShape, theShapeType, anAttr->isGeometricalSelection(), theError);
    else {
      if (anAttr->context().get())
        aValid = isValidObject(anAttr->context(),
                               theShapeType,
                               anAttr->isGeometricalSelection(),
                               theError);
      else
        aValid = isValidObject(anAttr->contextFeature(),
                               theShapeType,
                               anAttr->isGeometricalSelection(),
                               theError);
    }
  }
  else if (anAttributeType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr->isObject()) {
      aValid = isValidObject(anAttr->object(),
                             theShapeType,
                             false,
                             theError);
    }
    else if (theShapeType == Vertex) {
      AttributePtr aRefAttr = anAttr->attr();
      if (!aRefAttr.get()){
        aValid = false;
        theError = "It has reference to an empty attribute";
      }
      else {
        std::string anAttrType = aRefAttr->attributeType();
        aValid = anAttrType == GeomDataAPI_Point2D::typeId();
        if (!aValid) {
// LCOV_EXCL_START
          theError = "Shape type is \"%1\", it should be \"%2\"";
          theError.arg(anAttrType).arg(getShapeTypeDescription(theShapeType));
// LCOV_EXCL_STOP
        }
      }
    }
    else
      aValid = false;
  }
  else if (anAttributeType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    aValid = isValidObject(anAttr->value(), theShapeType, false, theError);
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
// LCOV_EXCL_START
    aValid = false;
    theError = "The attribute with the %1 type is not processed";
    theError.arg(anAttributeType);
// LCOV_EXCL_STOP
  }
  if (aValid)
    theError = "";
  return aValid;
}

bool GeomValidators_ShapeType::isValidObject(const ObjectPtr& theObject,
                                             const TypeOfShape theShapeType,
                                             const bool theIsGeometricalSelection,
                                             Events_InfoMessage& theError) const
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
    if (aResult.get()) {
      if (theShapeType == Plane)
      {
        ResultConstructionPtr aResultConstruction =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
        FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
        const std::string& aKind = aFeature->getKind();
        return aResult.get() != NULL && aKind == "Plane";
      }
      if (!aResult.get()) {
        aValid = false;
        theError = "The result is empty";
      } else {
        aValid = isValidShape(aResult->shape(), theShapeType, theIsGeometricalSelection, theError);
      }
    } else {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
      if (aFeature.get() && (theShapeType == CompSolid))
        return aValid;
      else {
        aValid = false;
        theError = "The feature has to produce a compsolid";
      }
    }
  }
  return aValid;
}

bool GeomValidators_ShapeType::isValidShape(const GeomShapePtr theShape,
                                            const TypeOfShape theShapeType,
                                            const bool theIsGeometricalSelection,
                                            Events_InfoMessage& theError) const
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
    case Line: {
      if (theIsGeometricalSelection && theShape->isCompound()) {
        aValid = true;
        for (GeomAPI_ShapeIterator anIt(theShape); anIt.more(); anIt.next()) {
          if (!anIt.current()->isEdge() || !GeomAPI_Curve(anIt.current()).isLine()) {
            aValid = false;
            break;
          }
        }
      }
      else {
        aValid = theShape->isEdge() && GeomAPI_Curve(theShape).isLine();
      }
      break;
    }
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
    case Plane:
      aValid = theShape->isPlanar();
      break;
    case Solid:
      aValid = theShape->isSolid() || theShape->isCompSolid() ||
               theShape->isCompoundOfSolids();
      break;
    case CompSolid:
      aValid = theShape->shapeType() == GeomAPI_Shape::COMPSOLID;
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
