// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#define MAJOR_AXIS_NEGATIVE (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                             feature()->attribute(SketchPlugin_MacroEllipse::FIRST_POINT_ID())))
#define MAJOR_AXIS_POSITIVE (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                             feature()->attribute(SketchPlugin_MacroEllipse::SECOND_POINT_ID())))
#define PASSED_POINT (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                      feature()->attribute(SketchPlugin_MacroEllipse::PASSED_POINT_ID())))

#define MAJOR_AXIS_NEGATIVE_REF (feature()->refattr( \
                                 SketchPlugin_MacroEllipse::FIRST_POINT_REF_ID()))
#define MAJOR_AXIS_POSITIVE_REF (feature()->refattr( \
                                 SketchPlugin_MacroEllipse::SECOND_POINT_REF_ID()))
#define PASSED_POINT_REF (feature()->refattr(SketchPlugin_MacroEllipse::PASSED_POINT_REF_ID()))


// find a parent sketch
static CompositeFeaturePtr sketch(FeaturePtr theFeature)
{
  CompositeFeaturePtr aSketch;
  const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
  for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt)
    if ((*anIt)->id() == SketchPlugin_Sketch::FEATURES_ID()) {
      aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>((*anIt)->owner());
      break;
    }
  return aSketch;
}


SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               bool callInitialize)
: SketchAPI_SketchEntity(theFeature)
{
  if (callInitialize && initialize())
    mySketch = sketch(theFeature);
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               double theX1, double theY1,
                                               double theX2, double theY2,
                                               double theX3, double theY3,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    if (byCenter)
      setByCenterAndPassedPoints();
    else
      setByMajorAxisAndPassedPoint();

    initializePoints(theX1, theY1, theX2, theY2, theX3, theY3);
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
    if (byCenter)
      setByCenterAndPassedPoints();
    else
      setByMajorAxisAndPassedPoint();

    initializePoints(thePoint1, thePoint2, thePoint3);
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
    if (byCenter)
      setByCenterAndPassedPoints();
    else
      setByMajorAxisAndPassedPoint();

    initializePoints(thePoint1, thePoint1Ref, thePoint2, thePoint2Ref, thePoint3, thePoint3Ref);
  }
}

SketchAPI_MacroEllipse::~SketchAPI_MacroEllipse()
{
}

void SketchAPI_MacroEllipse::setByCenterAndPassedPoints()
{
  fillAttribute(SketchPlugin_MacroEllipse::ELLIPSE_TYPE_BY_CENTER_AXIS_POINT(), myellipseType);
}

void SketchAPI_MacroEllipse::setByMajorAxisAndPassedPoint()
{
  fillAttribute(SketchPlugin_MacroEllipse::ELLIPSE_TYPE_BY_AXIS_AND_POINT(), myellipseType);
}

void SketchAPI_MacroEllipse::initializePoints(double theX1, double theY1,
                                              double theX2, double theY2,
                                              double theX3, double theY3)
{
  fillAttribute(MAJOR_AXIS_NEGATIVE, theX1, theY1);
  fillAttribute(MAJOR_AXIS_POSITIVE, theX2, theY2);
  fillAttribute(PASSED_POINT, theX3, theY3);

  mySketch = sketch(feature());
  execute();
}

void SketchAPI_MacroEllipse::initializePoints(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  fillAttribute(thePoint1, MAJOR_AXIS_NEGATIVE);
  fillAttribute(thePoint2, MAJOR_AXIS_POSITIVE);
  fillAttribute(thePoint3, PASSED_POINT);

  mySketch = sketch(feature());
  execute();
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

void SketchAPI_MacroEllipse::initializePoints(
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint1,
    const ModelHighAPI_RefAttr&           theMajorAxisPoint1Ref,
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint2,
    const ModelHighAPI_RefAttr&           theMajorAxisPoint2Ref,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint,
    const ModelHighAPI_RefAttr&           thePassedPointRef)
{
  fillAttribute(theMajorAxisPoint1, theMajorAxisPoint1Ref,
                MAJOR_AXIS_NEGATIVE, MAJOR_AXIS_NEGATIVE_REF);
  fillAttribute(theMajorAxisPoint2, theMajorAxisPoint2Ref,
                MAJOR_AXIS_POSITIVE, MAJOR_AXIS_POSITIVE_REF);
  fillAttribute(thePassedPoint, thePassedPointRef,
                PASSED_POINT, PASSED_POINT_REF);

  mySketch = sketch(feature());
  execute();
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
