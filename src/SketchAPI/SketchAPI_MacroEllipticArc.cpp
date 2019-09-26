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

#include "SketchAPI_MacroEllipticArc.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_RefAttr.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_MacroEllipticArc.h>

#define CENTER_POINT (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                      feature()->attribute(SketchPlugin_MacroEllipticArc::CENTER_ID())))
#define MAJOR_AXIS_POSITIVE (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                      feature()->attribute(SketchPlugin_MacroEllipticArc::MAJOR_AXIS_POINT_ID())))
#define START_POINT (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                     feature()->attribute(SketchPlugin_MacroEllipticArc::START_POINT_ID())))
#define END_POINT (std::dynamic_pointer_cast<GeomDataAPI_Point2D>( \
                   feature()->attribute(SketchPlugin_MacroEllipticArc::END_POINT_ID())))

#define CENTER_POINT_REF (feature()->refattr(SketchPlugin_MacroEllipticArc::CENTER_REF_ID()))
#define MAJOR_AXIS_POSITIVE_REF (feature()->refattr( \
                                 SketchPlugin_MacroEllipticArc::MAJOR_AXIS_POINT_REF_ID()))
#define START_POINT_REF (feature()->refattr(SketchPlugin_MacroEllipticArc::START_POINT_REF_ID()))
#define END_POINT_REF (feature()->refattr(SketchPlugin_MacroEllipticArc::END_POINT_REF_ID()))


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

SketchAPI_MacroEllipticArc::SketchAPI_MacroEllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : SketchAPI_MacroEllipse(theFeature)
{
}

SketchAPI_MacroEllipticArc::SketchAPI_MacroEllipticArc(
    const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::shared_ptr<GeomAPI_Pnt2d>&    theCenter,
    const ModelHighAPI_RefAttr&              theCenterRef,
    const std::shared_ptr<GeomAPI_Pnt2d>&    theMajorAxisPoint,
    const ModelHighAPI_RefAttr&              theMajorAxisPointRef,
    const std::shared_ptr<GeomAPI_Pnt2d>&    theArcStart,
    const ModelHighAPI_RefAttr&              theArcStartRef,
    const std::shared_ptr<GeomAPI_Pnt2d>&    theArcEnd,
    const ModelHighAPI_RefAttr&              theArcEndRef,
    const bool                               theReversed)
  : SketchAPI_MacroEllipse(theFeature)
{
  if (initialize()) {
    fillAttribute(theCenter, theCenterRef,
                  CENTER_POINT, CENTER_POINT_REF);
    fillAttribute(theMajorAxisPoint, theMajorAxisPointRef,
                  MAJOR_AXIS_POSITIVE, MAJOR_AXIS_POSITIVE_REF);
    fillAttribute(theArcStart, theArcStartRef, START_POINT, START_POINT_REF);
    fillAttribute(theArcEnd, theArcEndRef, END_POINT, END_POINT_REF);

    fillAttribute(theReversed, reversed());

    execute();
  }
}

SketchAPI_MacroEllipticArc::~SketchAPI_MacroEllipticArc()
{
}
