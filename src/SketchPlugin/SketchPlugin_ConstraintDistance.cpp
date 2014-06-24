// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"

#include "GeomDataAPI_Point2D.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
}

void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintDistance::execute()
{
  boost::shared_ptr<ModelAPI_Data> aData = data();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr_A = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr_B = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_B));

  AttributeDoublePtr anAttribute =
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));

  if (anAttr_A && anAttr_B && anAttribute->value() == 0)
  {
    FeaturePtr aFeature_A = anAttr_A->feature();
    FeaturePtr aFeature_B = anAttr_B->feature();

    double aValue = 40; // TODO
    anAttribute->setValue(aValue);
  }
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintDistance::preview()
{
  /// \todo Preview for distance constraint
  return getPreview();
}

