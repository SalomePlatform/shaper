// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.cpp
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Tools.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDouble.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Lin2d.h>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Tool.hxx>
#include <Precision.hxx>

namespace SketcherPrs_Tools {

ObjectPtr getResult(ModelAPI_Feature* theFeature, const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = aData->refattr(theAttrName);
  return anAttr->object();
}


std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject)
{
  ResultConstructionPtr aRes = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
  if (aRes.get() != NULL) {
    return aRes->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}


std::shared_ptr<GeomAPI_Pnt2d> getPoint(ModelAPI_Feature* theFeature,
                                        const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theFeature->data())
    return std::shared_ptr<GeomAPI_Pnt2d>();

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
  if(!anAttr.get()) {
    return std::shared_ptr<GeomAPI_Pnt2d>();
  }
  aFeature = ModelAPI_Feature::feature(anAttr->object());

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));

  else if (anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  if (aPointAttr.get() != NULL)
    return aPointAttr->pnt();
  return std::shared_ptr<GeomAPI_Pnt2d>();
}

//*************************************************************************************
/// Find an attribute of the given object which corresponds to a vetrex 
/// defined by a shape
/// \param theObject an object
/// \param theShape a vertex
/// \param thePlane a projection plane (sketcher plane)
std::shared_ptr<GeomDataAPI_Point2D> findGeomPoint(ObjectPtr theObject, 
                                    const TopoDS_Shape& theShape, 
                                    const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomDataAPI_Point2D> aGeomPoint;

  FeaturePtr anObjectFeature = ModelAPI_Feature::feature(theObject);
  if (anObjectFeature) {
    if (theShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aShapeVertex = TopoDS::Vertex(theShape);
      if (!aShapeVertex.IsNull())  {
        gp_Pnt aShapePoint = BRep_Tool::Pnt(aShapeVertex);
        std::shared_ptr<GeomAPI_Pnt> aShapeGeomPnt = std::shared_ptr<GeomAPI_Pnt>(
            new GeomAPI_Pnt(aShapePoint.X(), aShapePoint.Y(), aShapePoint.Z()));

        // find the given point in the feature attributes
        std::list<AttributePtr> anObjectAttiributes = 
          anObjectFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
        std::list<AttributePtr>::const_iterator anIt = anObjectAttiributes.begin(), 
                                                aLast = anObjectAttiributes.end();
        for (; anIt != aLast && !aGeomPoint; anIt++) {
          std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint = 
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);

          std::shared_ptr<GeomAPI_Pnt> anAttributePnt = thePlane->to3D(anAttributePoint->x(),
                                                                       anAttributePoint->y());
          if (anAttributePnt.get() &&
              anAttributePnt->distance(aShapeGeomPnt) < Precision::Confusion()) {
            aGeomPoint = anAttributePoint;
            break;
          }
        }
      }
    }
  }
  return aGeomPoint;
}

//*************************************************************************************
std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                     const std::string& theAttribute,
                                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theData)
    return aPointAttr;

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr) {
    if (anAttr->isObject()) {
      ObjectPtr anObject = anAttr->object();
      aFeature = ModelAPI_Feature::feature(anObject);
      if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID()) {
        // Attribute refers to a point
        aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                     aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));
      }
      else {
        // if the attribute refers on another object
        ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
        if (aRes.get()) {
          GeomShapePtr aShape = aRes->shape();
          if (aShape.get()) {
            TopoDS_Shape aTDSShape = aShape->impl<TopoDS_Shape>();
            aPointAttr = findGeomPoint(anObject, aTDSShape, thePlane);
          }
        }
      }
    }
    else if (anAttr->attr()) {
      // If attribute is a point
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
    }
  }
  return aPointAttr;
}

//*************************************************************************************
FeaturePtr getFeatureLine(DataPtr theData,
                          const std::string& theAttribute)
{
  FeaturePtr aLine;
  if (!theData)
    return aLine;

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (aFeature && aFeature->getKind() == SketchPlugin_Line::ID()) {
      return aFeature;
    }
  }
  return aLine;
}

//*************************************************************************************
std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(const FeaturePtr theLine,
                                                  const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  DataPtr aData = theLine->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());
  return aLin2d.project(thePoint);
}


static double MyArrowSize = 20;
double getArrowSize()
{
  return MyArrowSize;
}

void setArrowSize(double theSize)
{
  MyArrowSize = theSize;
}

int getDefaultArrowSize()
{
  return 20;
}

static double MyTextHeight = 16;
double getTextHeight()
{
  return MyTextHeight;
}

void setTextHeight(double theHeight)
{
  MyTextHeight = theHeight;
}

double getDefaultTextHeight()
{
  return 16;
}

double getFlyoutDistance(const ModelAPI_Feature* theConstraint)
{
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      const_cast<ModelAPI_Feature*>(theConstraint)->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

  return aFlyoutPoint->y();
}

std::shared_ptr<GeomAPI_Pnt> getAnchorPoint(const ModelAPI_Feature* theConstraint,
                                            const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  ModelAPI_Feature* aConstraint = const_cast<ModelAPI_Feature*>(theConstraint);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!aRefAttr || !aRefAttr->isObject() || !aRefAttr->object())
    return std::shared_ptr<GeomAPI_Pnt>();

  FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
  std::shared_ptr<GeomAPI_Pnt2d> aCenter;
  if (aFeature->getKind() == SketchPlugin_Arc::ID()) { // arc
    aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
  } else if (aFeature->getKind() == SketchPlugin_Circle::ID()) { // circle
    aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->attribute(SketchPlugin_Circle::CENTER_ID()))->pnt();
  } else 
    return std::shared_ptr<GeomAPI_Pnt>();

  std::shared_ptr<GeomAPI_Pnt2d> anOrigin(new GeomAPI_Pnt2d(0.0, 0.0));
  std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aConstraint->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()))->pnt();
  double aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aConstraint->attribute(SketchPlugin_Constraint::VALUE()))->value();
  double aLen = aFlyoutPnt->distance(anOrigin);
  aRadius *= 1.001; // a gap to make point much closer to the circle, but not lying on it
  aFlyoutPnt->setX(aCenter->x() + aFlyoutPnt->x() * aRadius / aLen);
  aFlyoutPnt->setY(aCenter->y() + aFlyoutPnt->y() * aRadius / aLen);
  return thePlane->to3D(aFlyoutPnt->x(), aFlyoutPnt->y());
}

};
