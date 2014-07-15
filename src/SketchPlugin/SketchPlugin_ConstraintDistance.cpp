// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"
#include <SketchPlugin_Point.h>

#include <GeomAPI_Lin2D.h>
#include <GeomAPI_Pnt2D.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

/// Obtain the point object from specified constraint parameter
static boost::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(
            DataPtr             theData,
            const std::string&  theAttribute);


SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
}

void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintDistance::execute()
{
  boost::shared_ptr<ModelAPI_Data> aData = data();

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A());
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B());
  if (aPoint_A && aPoint_B) {
    AttributeDoublePtr anAttr_Value =
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));

    if (!anAttr_Value->isInitialized()) {
      anAttr_Value->setValue(aPoint_A->pnt()->distance(aPoint_B->pnt()));
    }
  }
}

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_ConstraintDistance::getAISObject(
                    boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  DataPtr aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A());
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B());
  if (!aPoint_A || !aPoint_B)
    return thePrevious;

  boost::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

  boost::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aPoint_A->x(), aPoint_A->y());
  boost::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(aPoint_B->x(), aPoint_B->y());
  boost::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = aFlyOutAttr->isInitialized() ? 
                                              sketch()->to3D(aFlyOutAttr->x(), aFlyOutAttr->y()) : 
                                              boost::shared_ptr<GeomAPI_Pnt>();

  // value calculation
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aValue = aValueAttr->value();

  boost::shared_ptr<GeomAPI_AISObject> anAIS = thePrevious;
  if (!anAIS)
    anAIS = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject);
  anAIS->createDistance(aPoint1, aPoint2, aFlyoutPnt, aPlane, aValue);
  return anAIS;
}

void SketchPlugin_ConstraintDistance::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}

boost::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theData)
    return aPointAttr;

  FeaturePtr aFeature;
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr)
    aFeature = anAttr->feature();

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                           (aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));
  else {
    if (anAttr->attr())
      aPointAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  return aPointAttr;
}
