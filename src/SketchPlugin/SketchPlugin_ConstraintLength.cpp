// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "SketchPlugin_ConstraintLength.h"
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Tools.h>

#include <GeomDataAPI_Point2D.h>

#include <SketcherPrs_Factory.h>
#include <SketcherPrs_Tools.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <Config_PropManager.h>

#include <math.h>

const double tolerance = 1e-7;

SketchPlugin_ConstraintLength::SketchPlugin_ConstraintLength()
{
  myFlyoutUpdate = false;
}

void SketchPlugin_ConstraintLength::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(SketchPlugin_ConstraintLength::LOCATION_TYPE_ID(),
                       ModelAPI_AttributeInteger::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LOCATION_TYPE_ID());
}

void SketchPlugin_ConstraintLength::colorConfigInfo(std::string& theSection, std::string& theName,
                                                    std::string& theDefault)
{
  theSection = "Visualization";
  theName = "sketch_dimension_color";
  theDefault = SKETCH_DIMENSION_COLOR;
}

void SketchPlugin_ConstraintLength::execute()
{
  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (aFeature) {
    // set length value
    std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(SketchPlugin_Line::END_ID()));

    if (aPoint1.get() && aPoint2.get()) {
      // the value should to be computed here, not in the getAISObject
      // in order to change the model value
      // inside the object transaction. This is important for creating a constraint by preselection.
      // The display of the presentation in this case happens after the transaction commit
      AttributePtr aFlyOutAttribute =
        data()->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
      if (!aFlyOutAttribute->isInitialized()) {
        compute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
      }
    }
  }
}

bool SketchPlugin_ConstraintLength::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;

  std::shared_ptr<GeomAPI_Pnt> aPoint1, aPoint2;
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
  if (!getPoints(aPoint1, aPoint2, aStartPoint, anEndPoint))
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(theAttributeId));

  std::shared_ptr<GeomAPI_Lin2d> aLine =
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aStartPoint->pnt(), anEndPoint->pnt()));
  if (!aFlyOutAttr->isInitialized() ||
      (fabs(aFlyOutAttr->x()) < tolerance && fabs(aFlyOutAttr->y()) < tolerance)) {
    double aDist = aPoint1->distance(aPoint2)/5.;
    std::shared_ptr<GeomAPI_Pnt2d> aFPnt = aLine->shiftedLocation(aDist);
    aFlyOutAttr->setValue(aFPnt);
  }

  return true;
}

bool SketchPlugin_ConstraintLength::computeLenghtValue(double& theValue)
{
  bool aResult = false;
  std::shared_ptr<GeomAPI_Pnt> aPoint1, aPoint2;
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
  if (getPoints(aPoint1, aPoint2, aStartPoint, anEndPoint)) {
    theValue = aPoint1->distance(aPoint2);
    aResult = true;
  }
  return aResult;
}

bool SketchPlugin_ConstraintLength::getPoints(
  std::shared_ptr<GeomAPI_Pnt>& thePoint1, std::shared_ptr<GeomAPI_Pnt>& thePoint2,
  std::shared_ptr<GeomDataAPI_Point2D>& theStartPoint,
  std::shared_ptr<GeomDataAPI_Point2D>& theEndPoint)
{
  if (!sketch())
    return false;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return false;
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
  if (!aFeature || aFeature->getKind() != SketchPlugin_Line::ID())
    return false;
  DataPtr aData = aFeature->data();
  theStartPoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::START_ID()));
  theEndPoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));
  thePoint1 = sketch()->to3D(theStartPoint->x(), theStartPoint->y());
  thePoint2 = sketch()->to3D(theEndPoint->x(), theEndPoint->y());
  return true;
}

AISObjectPtr SketchPlugin_ConstraintLength::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::lengthDimensionConstraint(this,
    sketch(), thePrevious);
  if (anAIS.get() && !thePrevious.get())
    SketchPlugin_Tools::setDimensionColor(anAIS);
  return anAIS;
}

void SketchPlugin_ConstraintLength::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_Constraint::ENTITY_A())
  {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) {
      // only if it is not initialized, try to compute the current value
      double aLength;
      if (computeLenghtValue(aLength))
        aValueAttr->setValue(aLength);
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    // Recalculate flyout point in local coordinates of the line:
    // the X coordinate is a length of projection of the flyout point on the line
    // the Y coordinate is a distance from the point to the line
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
    AttributeRefAttrPtr aLineAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        attribute(SketchPlugin_Constraint::ENTITY_A()));
    if (!aLineAttr || !aLineAttr->isObject())
      return;
    FeaturePtr aLine = ModelAPI_Feature::feature(aLineAttr->object());
    if (!aLine || aLine->getKind() != SketchPlugin_Line::ID())
      return;

    std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = aFlyoutAttr->pnt();
    std::shared_ptr<GeomAPI_XY> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aLine->attribute(SketchPlugin_Line::START_ID()))->pnt()->xy();
    std::shared_ptr<GeomAPI_XY> aEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aLine->attribute(SketchPlugin_Line::END_ID()))->pnt()->xy();

    myFlyoutUpdate = true;
    std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aStartPnt);
    double X = aFlyoutDir->dot(aLineDir->xy());
    double Y = -aFlyoutDir->cross(aLineDir->xy());
    aFlyoutAttr->setValue(X, Y);
    myFlyoutUpdate = false;
  }
}
