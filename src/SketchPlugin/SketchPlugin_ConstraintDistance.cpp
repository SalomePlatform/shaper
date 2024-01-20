// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketchPlugin_ConstraintDistance.h"
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Tools.h>

#include <SketcherPrs_Tools.h>
#include <SketcherPrs_Factory.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Config_PropManager.h>

#include <math.h>

const double tolerance = 1e-7;


SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
  myFlyoutUpdate = false;
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SIGNED(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(SketchPlugin_ConstraintDistance::LOCATION_TYPE_ID(),
                       ModelAPI_AttributeInteger::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LOCATION_TYPE_ID());

  AttributePtr anAttr = data()->addAttribute(SketchPlugin_ConstraintDistance::DIRECTION_ID(),
                                             GeomDataAPI_Dir::typeId());
  anAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), DIRECTION_ID());
}

void SketchPlugin_ConstraintDistance::colorConfigInfo(std::string& theSection, std::string& theName,
                                                      std::string& theDefault)
{
  theSection = "Visualization";
  theName = "sketch_dimension_color";
  theDefault = SKETCH_DIMENSION_COLOR;
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeDoublePtr anAttrValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()));

  if(anAttrValue->isInitialized())
    return;

  double aDistance = calculateCurrentDistance();
  if(aDistance >= 0)
    anAttrValue->setValue(aDistance);
}

//*************************************************************************************
AISObjectPtr SketchPlugin_ConstraintDistance::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::lengthDimensionConstraint(this,
                                                                      sketch(),
                                                                      thePrevious);
  if (anAIS.get() && !thePrevious.get())
    SketchPlugin_Tools::setDimensionColor(anAIS);
  return anAIS;
}

static std::shared_ptr<GeomAPI_Lin2d> getLine(DataPtr theData, const std::string& theAttrName)
{
  FeaturePtr aLineFeature = SketcherPrs_Tools::getFeatureLine(theData, theAttrName);
  if (!aLineFeature)
    return GeomLine2dPtr();

  std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineFeature->attribute(SketchPlugin_Line::END_ID()));

  return GeomLine2dPtr(new GeomAPI_Lin2d(aStart->x(), aStart->y(), aEnd->x(), aEnd->y()));
}

double SketchPlugin_ConstraintDistance::calculateCurrentDistance()
{
  double aDistance = -1.;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
    SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B(), aPlane);

  GeomPnt2dPtr aGeomPntA, aGeomPntB;
  GeomLine2dPtr aLine;
  if (aPointA.get() && aPointB.get()) {  // both points
    aGeomPntA = aPointA->pnt();
    aGeomPntB = aPointB->pnt();
  } else {
    if (!aPointA.get() && aPointB.get()) {  //Line and point
      aLine = getLine(aData, SketchPlugin_Constraint::ENTITY_A());
      aGeomPntB = aPointB->pnt();
      aGeomPntA = aLine ? aLine->project(aGeomPntB) : GeomPnt2dPtr();
    } else if (aPointA.get() && !aPointB.get()) {  // Point and line
      aLine = getLine(aData, SketchPlugin_Constraint::ENTITY_B());
      aGeomPntA = aPointA->pnt();
      aGeomPntB = aLine ? aLine->project(aGeomPntA) : GeomPnt2dPtr();
    }
  }

  if (aGeomPntA.get() && aGeomPntB.get()) {
    aDistance = aGeomPntA->distance(aGeomPntB);
    if (aDistance < tolerance)
      aDistance = 0.0;
  }

  // keep the direction between arguments for processing of the zero distance
  std::shared_ptr<GeomDataAPI_Dir> aPointPointDir =
      std::dynamic_pointer_cast<GeomDataAPI_Dir>(attribute(DIRECTION_ID()));
  if (aDistance > tolerance)
    aPointPointDir->setValue(aGeomPntB->x() - aGeomPntA->x(), aGeomPntB->y() - aGeomPntA->y(), 0.0);
  else if (aLine) {
    GeomDir2dPtr aLineDir = aLine->direction();
    aPointPointDir->setValue(-aLineDir->y(), aLineDir->x(), 0.0);
  }

  return aDistance;
}

bool SketchPlugin_ConstraintDistance::areAttributesInitialized()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B(), aPlane);

  if (!aPointA && !aPointB)
    return false;
  else if (aPointA || aPointB) {
    FeaturePtr aLine;
    if (!aPointA)
      aLine = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
    else if (!aPointB)
      aLine = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
    else // both points are initialized
      return true;

    if (!aLine || aLine->getKind() != SketchPlugin_Line::ID())
      return false;
  }
  return true;
}

void SketchPlugin_ConstraintDistance::attributeChanged(const std::string& theID)
{
  if (theID == SketchPlugin_Constraint::ENTITY_A() ||
      theID == SketchPlugin_Constraint::ENTITY_B())
  {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) {
      // only if it is not initialized, try to compute the current value
      double aDistance = calculateCurrentDistance();
      if (aDistance >= 0) { // set as value the length of updated references
        aValueAttr->setValue(aDistance);
      }
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
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

    std::shared_ptr<GeomAPI_XY> aStartPnt;
    std::shared_ptr<GeomAPI_XY> aEndPnt;
    if (aPointA && aPointB) {
      aStartPnt = aPointA->pnt()->xy();
      aEndPnt = aPointB->pnt()->xy();
    } else if (aPointA) {
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(data(),
          SketchPlugin_Constraint::ENTITY_B());
      if (!aLine)
        return;
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointA->pnt();
      aStartPnt = aPoint->xy();
      aEndPnt = SketcherPrs_Tools::getProjectionPoint(aLine, aPoint)->xy();
    } else if (aPointB) {
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(data(),
          SketchPlugin_Constraint::ENTITY_A());
      if (!aLine)
        return;
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointB->pnt();
      aStartPnt = SketcherPrs_Tools::getProjectionPoint(aLine, aPoint)->xy();
      aEndPnt = aPoint->xy();
    } else
      return;

    myFlyoutUpdate = true;
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aStartPnt);
    if (aEndPnt->distance(aStartPnt) >= tolerance) {
      std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));
      double X = aFlyoutDir->dot(aLineDir->xy());
      double Y = -aFlyoutDir->cross(aLineDir->xy());
      aFlyoutAttr->setValue(X, Y);
    }
    else
      aFlyoutAttr->setValue(aFlyoutDir->x(), aFlyoutDir->y());
    myFlyoutUpdate = false;
  }
}

