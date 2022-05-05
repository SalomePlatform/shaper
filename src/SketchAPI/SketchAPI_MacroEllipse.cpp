// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "SketchAPI_MacroEllipse.h"
#include "SketchAPI_Line.h"
#include "SketchAPI_Point.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_RefAttr.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_Sketch.h>

#define POINT_ATTR(x) (std::dynamic_pointer_cast<GeomDataAPI_Point2D>(feature()->attribute((x))))
#define POINT_REF(x)  (feature()->refattr((x)))


SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               bool callInitialize)
: SketchAPI_SketchEntity(theFeature)
{
  if (callInitialize && initialize())
    storeSketch(theFeature);
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               double theX1, double theY1,
                                               double theX2, double theY2,
                                               double theX3, double theY3,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    static const ModelHighAPI_RefAttr DUMMY_REF;

    GeomPnt2dPtr aPnt1(new GeomAPI_Pnt2d(theX1, theY1));
    GeomPnt2dPtr aPnt2(new GeomAPI_Pnt2d(theX2, theY2));
    GeomPnt2dPtr aPnt3(new GeomAPI_Pnt2d(theX3, theY3));

    if (byCenter)
      setByCenterAndPassedPoints(aPnt1, DUMMY_REF, aPnt2, DUMMY_REF, aPnt3, DUMMY_REF);
    else
      setByMajorAxisAndPassedPoint(aPnt1, DUMMY_REF, aPnt2, DUMMY_REF, aPnt3, DUMMY_REF);
  }
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    static const ModelHighAPI_RefAttr DUMMY_REF;
    if (byCenter)
      setByCenterAndPassedPoints(thePoint1, DUMMY_REF, thePoint2, DUMMY_REF, thePoint3, DUMMY_REF);
    else {
      setByMajorAxisAndPassedPoint(thePoint1, DUMMY_REF,
                                   thePoint2, DUMMY_REF,
                                   thePoint3, DUMMY_REF);
    }
  }
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                               const ModelHighAPI_RefAttr&           thePoint1Ref,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                               const ModelHighAPI_RefAttr&           thePoint2Ref,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
                                               const ModelHighAPI_RefAttr&           thePoint3Ref,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    if (byCenter) {
      setByCenterAndPassedPoints(thePoint1, thePoint1Ref,
                                 thePoint2, thePoint2Ref,
                                 thePoint3, thePoint3Ref);
    }
    else {
      setByMajorAxisAndPassedPoint(thePoint1, thePoint1Ref,
                                   thePoint2, thePoint2Ref,
                                   thePoint3, thePoint3Ref);
    }
  }
}

SketchAPI_MacroEllipse::~SketchAPI_MacroEllipse()
{
}

static void fillAttribute(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                          const ModelHighAPI_RefAttr& thePointRef,
                          std::shared_ptr<GeomDataAPI_Point2D> thePointAttr,
                          AttributeRefAttrPtr thePointRefAttr)
{
  GeomPnt2dPtr aPoint = thePoint;
  if (!thePointRef.isEmpty()) {
    fillAttribute(thePointRef, thePointRefAttr);
    std::shared_ptr<GeomDataAPI_Point2D> anAttrPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(thePointRefAttr->attr());
    if (anAttrPnt)
      aPoint = anAttrPnt->pnt();
  }
  fillAttribute(aPoint, thePointAttr);
}

void SketchAPI_MacroEllipse::setByCenterAndPassedPoints(
    const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
    const ModelHighAPI_RefAttr&           theCenterRef,
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint,
    const ModelHighAPI_RefAttr&           theMajorAxisPointRef,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint,
    const ModelHighAPI_RefAttr&           thePassedPointRef)
{
  fillAttribute(SketchPlugin_MacroEllipse::ELLIPSE_TYPE_BY_CENTER_AXIS_POINT(), myellipseType);

  AttributePoint2DPtr aCenterAttr = POINT_ATTR(SketchPlugin_MacroEllipse::CENTER_POINT_ID());
  AttributePoint2DPtr aMajorAxisAttr = POINT_ATTR(SketchPlugin_MacroEllipse::MAJOR_AXIS_POINT_ID());
  AttributePoint2DPtr aPassedAttr = POINT_ATTR(SketchPlugin_MacroEllipse::PASSED_POINT_ID());

  AttributeRefAttrPtr aCenterRef = POINT_REF(SketchPlugin_MacroEllipse::CENTER_POINT_REF_ID());
  AttributeRefAttrPtr aMajorAxisRef =
      POINT_REF(SketchPlugin_MacroEllipse::MAJOR_AXIS_POINT_REF_ID());
  AttributeRefAttrPtr aPassedRef = POINT_REF(SketchPlugin_MacroEllipse::PASSED_POINT_REF_ID());

  fillAttribute(theCenter, theCenterRef, aCenterAttr, aCenterRef);
  fillAttribute(theMajorAxisPoint, theMajorAxisPointRef, aMajorAxisAttr, aMajorAxisRef);
  fillAttribute(thePassedPoint, thePassedPointRef, aPassedAttr, aPassedRef);

  storeSketch(feature());
  execute();
}

void SketchAPI_MacroEllipse::setByMajorAxisAndPassedPoint(
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisStart,
    const ModelHighAPI_RefAttr&           theMajorAxisStartRef,
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisEnd,
    const ModelHighAPI_RefAttr&           theMajorAxisEndRef,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint,
    const ModelHighAPI_RefAttr&           thePassedPointRef)
{
  fillAttribute(SketchPlugin_MacroEllipse::ELLIPSE_TYPE_BY_AXIS_AND_POINT(), myellipseType);

  AttributePoint2DPtr aMajorAxisStartAttr =
      POINT_ATTR(SketchPlugin_MacroEllipse::MAJOR_AXIS_START_ID());
  AttributePoint2DPtr aMajorAxisEndAttr =
      POINT_ATTR(SketchPlugin_MacroEllipse::MAJOR_AXIS_END_ID());
  AttributePoint2DPtr aPassedAttr = POINT_ATTR(SketchPlugin_MacroEllipse::PASSED_POINT_1_ID());

  AttributeRefAttrPtr aMajorAxisStartRef =
      POINT_REF(SketchPlugin_MacroEllipse::MAJOR_AXIS_START_REF_ID());
  AttributeRefAttrPtr aMajorAxisEndRef =
      POINT_REF(SketchPlugin_MacroEllipse::MAJOR_AXIS_END_REF_ID());
  AttributeRefAttrPtr aPassedRef = POINT_REF(SketchPlugin_MacroEllipse::PASSED_POINT_1_REF_ID());

  fillAttribute(theMajorAxisStart, theMajorAxisStartRef, aMajorAxisStartAttr, aMajorAxisStartRef);
  fillAttribute(theMajorAxisEnd, theMajorAxisEndRef, aMajorAxisEndAttr, aMajorAxisEndRef);
  fillAttribute(thePassedPoint, thePassedPointRef, aPassedAttr, aPassedRef);

  storeSketch(feature());
  execute();
}

void SketchAPI_MacroEllipse::storeSketch(const FeaturePtr& theFeature)
{
  const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt)
    if ((*anIt)->id() == SketchPlugin_Sketch::FEATURES_ID()) {
      mySketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>((*anIt)->owner());
      break;
    }
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::center()
{
  if (!myCenter)
    collectAuxiliary();
  return myCenter;
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::focus1()
{
  if (!myFocus1)
    collectAuxiliary();
  return myFocus1;
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::focus2()
{
  if (!myFocus2)
    collectAuxiliary();
  return myFocus2;
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::majorAxisStart()
{
  if (!myMajorAxisStart)
    collectAuxiliary();
  return myMajorAxisStart;
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::majorAxisEnd()
{
  if (!myMajorAxisEnd)
    collectAuxiliary();
  return myMajorAxisEnd;
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::minorAxisStart()
{
  if (!myMinorAxisStart)
    collectAuxiliary();
  return myMinorAxisStart;
}

std::shared_ptr<SketchAPI_Point> SketchAPI_MacroEllipse::minorAxisEnd()
{
  if (!myMinorAxisEnd)
    collectAuxiliary();
  return myMinorAxisEnd;
}

std::shared_ptr<SketchAPI_Line> SketchAPI_MacroEllipse::majorAxis()
{
  if (!myMajorAxis)
    collectAuxiliary();
  return myMajorAxis;
}

std::shared_ptr<SketchAPI_Line> SketchAPI_MacroEllipse::minorAxis()
{
  if (!myMinorAxis)
    collectAuxiliary();
  return myMinorAxis;
}

void SketchAPI_MacroEllipse::collectAuxiliary()
{
  // collect auxiliary features
  int aNbSubs = mySketch->numberOfSubs();
  std::shared_ptr<SketchAPI_Point>* anAuxPoint[] = {
    &myCenter, &myFocus1, &myFocus2,
    &myMajorAxisStart, &myMajorAxisEnd,
    &myMinorAxisStart, &myMinorAxisEnd
  };
  std::shared_ptr<SketchAPI_Line>* anAuxLine[] = {&myMajorAxis, &myMinorAxis};
  for (int aPtInd = 7, aLinInd = 2; (aPtInd > 0 || aLinInd > 0) && aNbSubs >= 0; ) {
    FeaturePtr aCurFeature = mySketch->subFeature(--aNbSubs);
    if (aPtInd > 0 && aCurFeature->getKind() == SketchPlugin_Point::ID())
      anAuxPoint[--aPtInd]->reset(new SketchAPI_Point(aCurFeature));
    else if (aLinInd > 0 && aCurFeature->getKind() == SketchPlugin_Line::ID())
      anAuxLine[--aLinInd]->reset(new SketchAPI_Line(aCurFeature));
  }
}
