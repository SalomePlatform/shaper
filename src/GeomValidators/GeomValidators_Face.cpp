// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "GeomValidators_Face.h"
#include "GeomValidators_Tools.h"

#include "ModelAPI_AttributeSelection.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeIterator.h>

#include <GeomAbs_SurfaceType.hxx>

#include <Events_InfoMessage.h>

typedef std::map<std::string, GeomAbs_SurfaceType> FaceTypes;
static FaceTypes MyFaceTypes;

GeomAbs_SurfaceType faceType(const std::string& theType)
{
  if (MyFaceTypes.size() == 0) {
    MyFaceTypes["plane"] = GeomAbs_Plane;
    MyFaceTypes["cylinder"] = GeomAbs_Cylinder;
  }
  std::string aType = std::string(theType.c_str());
  if (MyFaceTypes.find(aType) != MyFaceTypes.end())
    return MyFaceTypes[aType];

// LCOV_EXCL_START
  Events_InfoMessage("GeomValidators_Face", "Face type defined in XML is not implemented!").send();
  return GeomAbs_Plane;
// LCOV_EXCL_STOP
}

bool isValidFace(const GeomShapePtr theShape,
                 const GeomAbs_SurfaceType theFaceType,
                 Events_InfoMessage& theError)
{
  GeomFacePtr aGeomFace = theShape->face();

  if (!aGeomFace.get()) {
    theError = "The shape is not a face.";
      return false;
  }

  bool aValid = true;

  switch (theFaceType) {
    case GeomAbs_Plane: {
      aValid = aGeomFace->isPlanar();
      if (!aValid) theError = "The shape is not a plane.";
      break;
    }
    case GeomAbs_Cylinder: {
      aValid = aGeomFace->isCylindrical();
      if (!aValid) theError = "The shape is not a cylinder.";
      break;
    }
    default: {
      aValid = false;
      theError = "The shape is not an available face.";
      break;
    }
  }

  return aValid;
}

bool GeomValidators_Face::isValid(const AttributePtr& theAttribute,
                                  const std::list<std::string>& theArguments,
                                  Events_InfoMessage& theError) const
{
  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType != ModelAPI_AttributeSelection::typeId()) {
// LCOV_EXCL_START
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
// LCOV_EXCL_STOP
  }

  bool aValid = true;
  ObjectPtr anObject = GeomValidators_Tools::getObject(theAttribute);
  if (!anObject.get()) {
    aValid = true; // an empty face selected is valid.
  }
  else {
    AttributeSelectionPtr aSelectionAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    std::shared_ptr<GeomAPI_Shape> aGeomShape = aSelectionAttr->value();
    if (!aGeomShape.get()) {
      // if the shape is empty, apply the validator to the shape of result
      aGeomShape = aSelectionAttr->context()->shape();
    }
    // it is necessary to check whether the shape is face in order to set in selection a value
    // with any type and check the type in this validator
    // It is realized to select any object in OB and filter it in this validator (sketch plane)
    GeomAbs_SurfaceType aFaceType = GeomAbs_Plane;
    if (theArguments.size() == 1)
      aFaceType = faceType(theArguments.front());
    if (aGeomShape->isFace()) {
      aValid = isValidFace(aGeomShape, aFaceType, theError);
    }
    else if (aSelectionAttr->isGeometricalSelection() && aGeomShape->isCompound()) {
      for (GeomAPI_ShapeIterator anIt(aGeomShape); anIt.more() && aValid; anIt.next()) {
        aValid = isValidFace(anIt.current(), aFaceType, theError);
      }
    }
    else {
      aValid = false;
      theError = "The shape is not a face.";
    }
  }
  return aValid;
}
