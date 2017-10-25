// Copyright (C) 2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

// File:    SketchPlugin_ConstraintDistanceAlongDir.cpp
// Created: 24 October 2017
// Author:  Artem ZHIDKOV

#include <SketchPlugin_ConstraintDistanceAlongDir.h>

#include <SketcherPrs_Tools.h>
#include <SketcherPrs_Factory.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <cmath>

const double tolerance = 1e-7;


SketchPlugin_ConstraintDistanceAlongDir::SketchPlugin_ConstraintDistanceAlongDir()
  : SketchPlugin_ConstraintDistance(),
    myValue(-1.e100),
    myValueUpdate(false)
{
}

//*************************************************************************************
void SketchPlugin_ConstraintDistanceAlongDir::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(LOCATION_TYPE_ID(), ModelAPI_AttributeInteger::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LOCATION_TYPE_ID());

  data()->addAttribute(DISTANCE_VALUE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(NEGATIVE_TYPE_ID(), ModelAPI_AttributeBoolean::typeId());
  boolean(NEGATIVE_TYPE_ID())->setValue(false);
}

//*************************************************************************************
void SketchPlugin_ConstraintDistanceAlongDir::execute()
{
  AttributeDoublePtr anAttrValue = real(SketchPlugin_Constraint::VALUE());
  if (anAttrValue->isInitialized() || !areAttributesInitialized())
    return;

  double aDistance = calculateCurrentDistance();
  anAttrValue->setValue(aDistance);
}

//*************************************************************************************
AISObjectPtr SketchPlugin_ConstraintDistanceAlongDir::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::lengthDimensionConstraint(this,
                                                                      sketch()->coordinatePlane(),
                                                                      thePrevious);
  return anAIS;
}

void SketchPlugin_ConstraintDistanceAlongDir::attributeChanged(const std::string& theID)
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
  } else if (theID == SketchPlugin_Constraint::VALUE() && !myValueUpdate) {
    myValueUpdate = true;
    // value of the distance shown to the user should be always positive
    AttributeDoublePtr aDistanceValueAttr = real(DISTANCE_VALUE_ID());
    double aConstraintValue = real(SketchPlugin_Constraint::VALUE())->value();
    aDistanceValueAttr->setValue(fabs(aConstraintValue));
    myValueUpdate = false;
  } else if (theID == DISTANCE_VALUE_ID() && !myValueUpdate){
    myValueUpdate = true;
    // update value of the distance according to the value set by user
    double aDistanceValue = real(DISTANCE_VALUE_ID())->value();
    AttributeDoublePtr aConstraintValueAttr = real(SketchPlugin_Constraint::VALUE());
    if (aConstraintValueAttr->value() < 0.0)
      aDistanceValue = -aDistanceValue;
    aConstraintValueAttr->setValue(aDistanceValue);
    myValueUpdate = false;
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

    std::shared_ptr<GeomAPI_XY> aStartPnt = aPointA->pnt()->xy();
    std::shared_ptr<GeomAPI_XY> aEndPnt = aPointB->pnt()->xy();

    if (aEndPnt->distance(aStartPnt) < tolerance)
      return;

    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aEndPnt);
    myFlyoutUpdate = true;
    updateFlyoutPoint();
    myFlyoutUpdate = false;
  }
}
