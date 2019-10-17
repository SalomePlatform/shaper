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

#include "SketchAPI_MacroArc.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theCenterX, double theCenterY,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenterX, theCenterY, theStartX,
                        theStartY, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenter, theStart, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             double thePassedX, double thePassedY)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStartX, theStartY, theEndX, theEndY, thePassedX, thePassedY);
  }
}

//===============================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStart, theEnd, thePassed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_RefAttr& theTangentPoint,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_RefAttr& theTangentPoint,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::~SketchAPI_MacroArc()
{

}

//================================================================================================
void SketchAPI_MacroArc::setByCenterStartEnd(double theCenterX, double theCenterY,
                                        double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_CENTER_AND_POINTS(), myarcType);
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(startPoint1(), theStartX, theStartY);
  fillAttribute(endPoint1(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_CENTER_AND_POINTS(), myarcType);
  fillAttribute(theCenter, mycenter);
  fillAttribute(theStart, mystartPoint1);
  fillAttribute(theEnd, myendPoint1);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByStartEndPassed(double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        double thePassedX, double thePassedY)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_THREE_POINTS(), myarcType);
  fillAttribute(startPoint2(), theStartX, theStartY);
  fillAttribute(endPoint2(), theEndX, theEndY);
  fillAttribute(passedPoint(), thePassedX, thePassedY);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByStartEndPassed(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_THREE_POINTS(), myarcType);
  fillAttribute(theStart, mystartPoint2);
  fillAttribute(theEnd, myendPoint2);
  fillAttribute(thePassed, mypassedPoint);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                 double theEndX, double theEndY,
                                 bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_TANGENT_EDGE(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(endPoint3(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                 bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_TANGENT_EDGE(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(theEnd, myendPoint3);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByTransversal(const ModelHighAPI_RefAttr& theTransversalPoint,
                                          double theEndX, double theEndY,
                                          bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_TRANSVERSAL_LINE(), myarcType);
  fillAttribute(theTransversalPoint, mytransversalPoint);
  fillAttribute(endPoint4(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByTransversal(const ModelHighAPI_RefAttr& theTransversalPoint,
                                          const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                          bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_TRANSVERSAL_LINE(), myarcType);
  fillAttribute(theTransversalPoint, mytransversalPoint);
  fillAttribute(theEnd, myendPoint4);
  fillAttribute(theInversed, myreversed);

  execute();
}
