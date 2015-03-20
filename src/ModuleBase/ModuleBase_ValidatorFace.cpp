// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "ModuleBase_ValidatorFace.h"
#include "ModuleBase_WidgetShapeSelector.h"

#include "ModelAPI_AttributeSelection.h"

#include <GeomAPI_Face.h>

#include <Events_Error.h>

#include <QString>
#include <QMap>

typedef QMap<QString, GeomAbs_SurfaceType> FaceTypes;
static FaceTypes MyFaceTypes;

GeomAbs_SurfaceType ModuleBase_ValidatorFace::faceType(const std::string& theType)
{
  if (MyFaceTypes.count() == 0) {
    MyFaceTypes["plane"] = GeomAbs_Plane;
    MyFaceTypes["cylinder"] = GeomAbs_Cylinder;
  }
  QString aType = QString(theType.c_str()).toLower();
  if (MyFaceTypes.contains(aType))
    return MyFaceTypes[aType];
  
  Events_Error::send("Face type defined in XML is not implemented!");
  return GeomAbs_Plane;
}

bool ModuleBase_ValidatorFace::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments) const
{
  bool aValid = false;

  GeomAbs_SurfaceType aFaceType = GeomAbs_Plane;
  if (theArguments.size() == 1) {
    std::string anArgument = theArguments.front();
    aFaceType = faceType(anArgument);
  }

  ObjectPtr anObject = ModuleBase_WidgetShapeSelector::getObject(theAttribute);
  if (anObject.get() != NULL) {
    AttributeSelectionPtr aSelectionAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                                                 (theAttribute);
    std::shared_ptr<GeomAPI_Shape> aGeomShape = aSelectionAttr->value();
    std::shared_ptr<GeomAPI_Face> aGeomFace(new GeomAPI_Face(aGeomShape));
    if (aGeomFace.get() != NULL) {
      switch(aFaceType) {
          case GeomAbs_Plane:
            aValid = aGeomFace->isPlanar();
            break;
          case GeomAbs_Cylinder:
            break;
          default:
            break;
      }
    }
  }
  return aValid;
}
