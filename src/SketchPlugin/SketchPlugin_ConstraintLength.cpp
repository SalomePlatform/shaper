// File:    SketchPlugin_ConstraintLength.cpp
// Created: 30 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintLength.h"
#include <SketchPlugin_Line.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

SketchPlugin_ConstraintLength::SketchPlugin_ConstraintLength()
{
}

void SketchPlugin_ConstraintLength::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintLength::execute()
{
  if (data()->attribute(CONSTRAINT_ATTR_ENTITY_A)->isInitialized() &&
      !data()->attribute(CONSTRAINT_ATTR_VALUE)->isInitialized()) {

    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(CONSTRAINT_ATTR_ENTITY_A));
    ObjectPtr aFeature = aRef->object();
    if (aFeature) {
      // set length value
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature->data()->attribute(LINE_ATTR_START));
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature->data()->attribute(LINE_ATTR_END));

      double aLenght = aPoint1->pnt()->distance(aPoint2->pnt());

      boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CONSTRAINT_ATTR_VALUE));
      aValueAttr->setValue(aLenght);
    }
  }
}

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_ConstraintLength::getAISObject(
                    boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->object());
  if (!aFeature || aFeature->getKind() != SKETCH_LINE_KIND)
    return thePrevious;

  boost::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));

  DataPtr aData = aFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  boost::shared_ptr<GeomDataAPI_Point2D> anEndPoint =
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

  boost::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aStartPoint->x(), aStartPoint->y());
  boost::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(anEndPoint->x(),  anEndPoint->y());
  boost::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = aFlyOutAttr->isInitialized() ? 
                                              sketch()->to3D(aFlyOutAttr->x(), aFlyOutAttr->y()) : 
                                              boost::shared_ptr<GeomAPI_Pnt>();

  // value calculation
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CONSTRAINT_ATTR_VALUE));
  double aValue = aValueAttr->value();

  boost::shared_ptr<GeomAPI_AISObject> anAIS = thePrevious;
  if (!anAIS)
    anAIS = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject);
  anAIS->createDistance(aPoint1, aPoint2, aFlyoutPnt, aPlane, aValue);
  return anAIS;
}

void SketchPlugin_ConstraintLength::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}

