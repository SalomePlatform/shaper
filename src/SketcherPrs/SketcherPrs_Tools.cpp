// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.cpp
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Tools.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomDataAPI_Point2D.h>


namespace SketcherPrs_Tools {


std::shared_ptr<GeomAPI_Edge> getLine(SketchPlugin_Constraint* theFeature,
                                      const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(theAttrName));

  ObjectPtr aObject = anAttr->object();
  ResultConstructionPtr aRes = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObject);
  if (aRes.get() != NULL) {
    std::shared_ptr<GeomAPI_Shape> aShape = aRes->shape();
    if (aShape.get() != NULL) {   
      return std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aShape));
    }
  }
  return std::shared_ptr<GeomAPI_Edge>();
}


std::shared_ptr<GeomAPI_Pnt2d> getPoint(SketchPlugin_Constraint* theFeature,
                                               const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theFeature->data())
    return std::shared_ptr<GeomAPI_Pnt2d>();

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
  if (anAttr)
    aFeature = ModelAPI_Feature::feature(anAttr->object());

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));

  else if (aFeature && aFeature->getKind() == SketchPlugin_Circle::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));

  else if (anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  return aPointAttr->pnt();
}


};