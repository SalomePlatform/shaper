// File:    SketchPlugin_ConstraintPerpendicular.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintPerpendicular.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Pnt.h>

SketchPlugin_ConstraintPerpendicular::SketchPlugin_ConstraintPerpendicular()
{
}

void SketchPlugin_ConstraintPerpendicular::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintPerpendicular::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintPerpendicular::preview()
{
  /// \todo Preview for perpendicular constraint
  return getPreview();
}

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_ConstraintPerpendicular::getAISObject(
                    boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr1 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr2 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!anAttr1 || !anAttr1->isFeature() || 
      !anAttr2 || !anAttr2->isFeature())
    return thePrevious;
  boost::shared_ptr<SketchPlugin_Line> aLine1Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(anAttr1->feature());
  boost::shared_ptr<SketchPlugin_Line> aLine2Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(anAttr2->feature());
  if (!aLine1Feature || !aLine2Feature)
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  boost::shared_ptr<GeomAPI_Shape> aLine1 = aLine1Feature->preview();
  boost::shared_ptr<GeomAPI_Shape> aLine2 = aLine2Feature->preview();

  boost::shared_ptr<GeomAPI_AISObject> anAIS = thePrevious;
  if (!anAIS)
    anAIS = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject);
  anAIS->createPerpendicular(aLine1, aLine2, aPlane);
  return anAIS;
}

void SketchPlugin_ConstraintPerpendicular::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aPoint->setValue(aPoint->x() + theDeltaX, aPoint->y() + theDeltaY);
}

