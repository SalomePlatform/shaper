// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomDataAPI_Point2D.cpp
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV


#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeRefAttr.h>

void GeomDataAPI_Point2D::move(const double theDeltaX, const double theDeltaY)
{
  setValue(x() + theDeltaX, y() + theDeltaY);
}

std::string GeomDataAPI_Point2D::attributeType()
{
  return typeId();
}

std::shared_ptr<GeomDataAPI_Point2D> GeomDataAPI_Point2D::getPoint2D(
                                  const DataPtr& theData, const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
  if (!theData)
    return aPointAttr;

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
                                 ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr && anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  return aPointAttr;
}


GeomDataAPI_Point2D::GeomDataAPI_Point2D()
{
}

GeomDataAPI_Point2D::~GeomDataAPI_Point2D()
{
}
