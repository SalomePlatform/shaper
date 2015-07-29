// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "GeomValidators_Face.h"
#include "GeomValidators_Tools.h"

#include "ModelAPI_AttributeSelection.h"

#include <GeomAPI_Face.h>

#include <Events_Error.h>

#include <QString>
#include <QMap>

typedef std::map<std::string, GeomAbs_SurfaceType> FaceTypes;
static FaceTypes MyFaceTypes;

GeomAbs_SurfaceType GeomValidators_Face::faceType(const std::string& theType)
{
  if (MyFaceTypes.size() == 0) {
    MyFaceTypes["plane"] = GeomAbs_Plane;
    MyFaceTypes["cylinder"] = GeomAbs_Cylinder;
  }
  std::string aType = std::string(theType.c_str());
  if (MyFaceTypes.find(aType) != MyFaceTypes.end())
    return MyFaceTypes[aType];
  
  Events_Error::send("Face type defined in XML is not implemented!");
  return GeomAbs_Plane;
}

bool GeomValidators_Face::isValid(const AttributePtr& theAttribute,
                                  const std::list<std::string>& theArguments,
                                  std::string& theError) const
{
  bool aValid = false;

  GeomAbs_SurfaceType aFaceType = GeomAbs_Plane;
  if (theArguments.size() == 1) {
    std::string anArgument = theArguments.front();
    aFaceType = faceType(anArgument);
  }

  ObjectPtr anObject = GeomValidators_Tools::getObject(theAttribute);
  if (anObject.get() != NULL) {
    AttributeSelectionPtr aSelectionAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                                                 (theAttribute);
    std::shared_ptr<GeomAPI_Shape> aGeomShape = aSelectionAttr->value();
    if (!aGeomShape.get()) {
      // if the shape is empty, apply the validator to the shape of result
      aGeomShape = aSelectionAttr->context()->shape();
    }
    // it is necessary to check whether the shape is face in order to set in selection a value
    // with any type and check the type in this validator
    // It is realized to select any object in OB and filter it in this validator (sketch plane)
    if (aGeomShape->isFace()) {
      std::shared_ptr<GeomAPI_Face> aGeomFace(new GeomAPI_Face(aGeomShape));
      if (aGeomFace.get() != NULL) {
        switch(aFaceType) {
            case GeomAbs_Plane:
              aValid = aGeomFace->isPlanar();
              break;
            case GeomAbs_Cylinder:
              aValid = aGeomFace->isCylindrical();
              break;
            default:
              break;
        }
      }
    }
  }
  else
    aValid = true; // an empty face selected is valid.
  return aValid;
}
