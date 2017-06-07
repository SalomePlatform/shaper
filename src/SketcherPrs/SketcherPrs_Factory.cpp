// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SketcherPrs_Factory.h"

#include "SketcherPrs_Coincident.h"
#include "SketcherPrs_Collinear.h"
#include "SketcherPrs_Parallel.h"
#include "SketcherPrs_Perpendicular.h"
#include "SketcherPrs_Rigid.h"
#include "SketcherPrs_HVDirection.h"
#include "SketcherPrs_Equal.h"
#include "SketcherPrs_Tangent.h"
#include "SketcherPrs_Radius.h"
#include "SketcherPrs_LengthDimension.h"
#include "SketcherPrs_Middle.h"
#include "SketcherPrs_Mirror.h"
#include "SketcherPrs_Transformation.h"
#include "SketcherPrs_Angle.h"

// Macros for constraint presentation definition
#define CONSTRAINT_PRS_IMPL(NAME, CLASS) \
AISObjectPtr SketcherPrs_Factory::NAME(ModelAPI_Feature* theConstraint, \
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane, \
                                       AISObjectPtr thePrevious) \
{ \
  std::shared_ptr<GeomAPI_AISObject> anAISObj; \
  if (CLASS::IsReadyToDisplay(theConstraint, thePlane)) { \
    if (thePrevious.get()) \
      anAISObj = thePrevious; \
    else { \
      anAISObj = AISObjectPtr(new GeomAPI_AISObject()); \
      Handle(CLASS) aPrs = new CLASS(theConstraint, thePlane); \
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs)); \
    } \
  } \
  return anAISObj; \
}

CONSTRAINT_PRS_IMPL(coincidentConstraint, SketcherPrs_Coincident);
CONSTRAINT_PRS_IMPL(collinearConstraint, SketcherPrs_Collinear);
CONSTRAINT_PRS_IMPL(parallelConstraint, SketcherPrs_Parallel);
CONSTRAINT_PRS_IMPL(perpendicularConstraint, SketcherPrs_Perpendicular);
CONSTRAINT_PRS_IMPL(rigidConstraint, SketcherPrs_Rigid);
CONSTRAINT_PRS_IMPL(equalConstraint, SketcherPrs_Equal);
CONSTRAINT_PRS_IMPL(tangentConstraint, SketcherPrs_Tangent);
CONSTRAINT_PRS_IMPL(radiusConstraint, SketcherPrs_Radius);
CONSTRAINT_PRS_IMPL(lengthDimensionConstraint, SketcherPrs_LengthDimension);
CONSTRAINT_PRS_IMPL(middleConstraint, SketcherPrs_Middle);
CONSTRAINT_PRS_IMPL(mirrorConstraint, SketcherPrs_Mirror);
CONSTRAINT_PRS_IMPL(angleConstraint, SketcherPrs_Angle);

// Non-standard constraints definition
AISObjectPtr SketcherPrs_Factory::horisontalConstraint(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                       AISObjectPtr thePrevious)
{
  std::shared_ptr<GeomAPI_AISObject> anAISObj;
  if (SketcherPrs_HVDirection::IsReadyToDisplay(theConstraint, thePlane)) {
    if (thePrevious.get())
      anAISObj = thePrevious;
    else {
      anAISObj = AISObjectPtr(new GeomAPI_AISObject());
      Handle(SketcherPrs_HVDirection) aPrs =
        new SketcherPrs_HVDirection(theConstraint, thePlane, true);
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
    }
  }
  return anAISObj;
}

AISObjectPtr SketcherPrs_Factory::verticalConstraint(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                       AISObjectPtr thePrevious)
{
  std::shared_ptr<GeomAPI_AISObject> anAISObj;
  if (SketcherPrs_HVDirection::IsReadyToDisplay(theConstraint, thePlane)) {
    if (thePrevious.get())
      anAISObj = thePrevious;
    else {
      anAISObj = AISObjectPtr(new GeomAPI_AISObject());
      Handle(SketcherPrs_HVDirection) aPrs =
        new SketcherPrs_HVDirection(theConstraint, thePlane, false);
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
    }
  }
  return anAISObj;
}

AISObjectPtr SketcherPrs_Factory::translateConstraint(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                       AISObjectPtr thePrevious)
{
  std::shared_ptr<GeomAPI_AISObject> anAISObj;
  if (SketcherPrs_Transformation::IsReadyToDisplay(theConstraint, thePlane)) {
    if (thePrevious.get())
      anAISObj = thePrevious;
    else {
      anAISObj = AISObjectPtr(new GeomAPI_AISObject());
      Handle(SketcherPrs_Transformation) aPrs =
        new SketcherPrs_Transformation(theConstraint, thePlane, true);
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
    }
  }
  return anAISObj;
}

AISObjectPtr SketcherPrs_Factory::rotateConstraint(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                       AISObjectPtr thePrevious)
{
  std::shared_ptr<GeomAPI_AISObject> anAISObj;
  if (SketcherPrs_Transformation::IsReadyToDisplay(theConstraint, thePlane)) {
    if (thePrevious.get())
      anAISObj = thePrevious;
    else {
      anAISObj = AISObjectPtr(new GeomAPI_AISObject());
      Handle(SketcherPrs_Transformation) aPrs =
        new SketcherPrs_Transformation(theConstraint, thePlane, false);
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(aPrs));
    }
  }
  return anAISObj;
}
