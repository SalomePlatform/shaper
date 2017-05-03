// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintDistanceVertical.cpp
// Created: 10 May 2017
// Author:  Artem ZHIDKOV

#include <SketchPlugin_ConstraintDistanceVertical.h>

#include <SketcherPrs_Tools.h>
#include <SketcherPrs_Factory.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>

const double tolerance = 1e-7;


SketchPlugin_ConstraintDistanceVertical::SketchPlugin_ConstraintDistanceVertical()
  : SketchPlugin_ConstraintDistance()
{
}

//*************************************************************************************
void SketchPlugin_ConstraintDistanceVertical::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

//*************************************************************************************
void SketchPlugin_ConstraintDistanceVertical::execute()
{
  AttributeDoublePtr anAttrValue = real(SketchPlugin_Constraint::VALUE());
  if (anAttrValue->isInitialized() || !areAttributesInitialized())
    return;

  double aDistance = calculateCurrentDistance();
  anAttrValue->setValue(aDistance);
}

//*************************************************************************************
AISObjectPtr SketchPlugin_ConstraintDistanceVertical::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::lengthDimensionConstraint(this,
                                                                      sketch()->coordinatePlane(),
                                                                      thePrevious);
  return anAIS;
}

//*************************************************************************************
void SketchPlugin_ConstraintDistanceVertical::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  // Recalculate a shift of flyout point in terms of local coordinates
  std::shared_ptr<GeomAPI_XY> aDir(new GeomAPI_XY(theDeltaX, theDeltaY));
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA = SketcherPrs_Tools::getFeaturePoint(
      data(), SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB = SketcherPrs_Tools::getFeaturePoint(
      data(), SketchPlugin_Constraint::ENTITY_B(), aPlane);

  if (!aPointA || !aPointB)
    return;

  std::shared_ptr<GeomAPI_XY> aStartPnt = aPointA->pnt()->xy();
  std::shared_ptr<GeomAPI_XY> aEndPnt = aPointB->pnt()->xy();

  std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));
  double dX = aDir->dot(aLineDir->xy());
  double dY = -aDir->cross(aLineDir->xy());

  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  myFlyoutUpdate = true;
  if (aPoint->isInitialized()) {
    aPoint->setValue(aPoint->x() + dX, aPoint->y() + dY);
  } else {
    aPoint->setValue(dX, dY);
  }
  myFlyoutUpdate = false;
}

double SketchPlugin_ConstraintDistanceVertical::calculateCurrentDistance()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
    SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B(), aPlane);

  return aPointB->y() - aPointA->y();
}

void SketchPlugin_ConstraintDistanceVertical::attributeChanged(const std::string& theID)
{
  if (theID == SketchPlugin_Constraint::ENTITY_A() ||
      theID == SketchPlugin_Constraint::ENTITY_B())
  {
    AttributeDoublePtr aValueAttr = real(SketchPlugin_Constraint::VALUE());
    if (!aValueAttr->isInitialized() && areAttributesInitialized()) {
      // only if it is not initialized, try to compute the current value
      double aDistance = calculateCurrentDistance();
      aValueAttr->setValue(aDistance);
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    myFlyoutUpdate = true;
    // Recalculate flyout point in local coordinates of the distance constraint:
    // the X coordinate is a length of projection of the flyout point on the
    //                  line binding two distanced points
    //                  or a line of projection of the distanced point onto the distanced segment
    // the Y coordinate is a distance from the flyout point to the line
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
    std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = aFlyoutAttr->pnt();

    std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
    std::shared_ptr<GeomDataAPI_Point2D> aPointA = SketcherPrs_Tools::getFeaturePoint(
        data(), SketchPlugin_Constraint::ENTITY_A(), aPlane);
    std::shared_ptr<GeomDataAPI_Point2D> aPointB = SketcherPrs_Tools::getFeaturePoint(
        data(), SketchPlugin_Constraint::ENTITY_B(), aPlane);

    std::shared_ptr<GeomAPI_XY> aStartPnt = aPointA->pnt()->xy();
    std::shared_ptr<GeomAPI_XY> aEndPnt = aPointB->pnt()->xy();

    if (aEndPnt->distance(aStartPnt) < tolerance)
      return;

    std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aStartPnt);

    double X = aFlyoutDir->dot(aLineDir->xy());
    double Y = -aFlyoutDir->cross(aLineDir->xy());
    aFlyoutAttr->setValue(X, Y);
    myFlyoutUpdate = false;
  }
}
