// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SketchAPI_MacroEllipticArc_H_
#define SketchAPI_MacroEllipticArc_H_

#include "SketchAPI.h"
#include "SketchAPI_MacroEllipse.h"

#include <SketchPlugin_MacroEllipticArc.h>

/// \class SketchAPI_MacroEllipticArc
/// \ingroup CPPHighAPI
/// \brief Interface for Elliptic Arc feature.
class SketchAPI_MacroEllipticArc: public SketchAPI_MacroEllipse
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroEllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values and/or references.
  SKETCHAPI_EXPORT
  SketchAPI_MacroEllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>&    theCenter,
                             const ModelHighAPI_RefAttr&              theCenterRef,
                             const std::shared_ptr<GeomAPI_Pnt2d>&    theMajorAxisPoint,
                             const ModelHighAPI_RefAttr&              theMajorAxisPointRef,
                             const std::shared_ptr<GeomAPI_Pnt2d>&    theArcStart,
                             const ModelHighAPI_RefAttr&              theArcStartRef,
                             const std::shared_ptr<GeomAPI_Pnt2d>&    theArcEnd,
                             const ModelHighAPI_RefAttr&              theArcEndRef,
                             const bool                               theReversed);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_MacroEllipticArc();

  INTERFACE_3(SketchPlugin_MacroEllipticArc::ID(),
              startPoint, SketchPlugin_MacroEllipticArc::START_POINT_ID(),
              GeomDataAPI_Point2D, /** Start point of elliptic arc */,
              endPoint, SketchPlugin_MacroEllipticArc::END_POINT_ID(),
              GeomDataAPI_Point2D, /** End point of elliptic arc */,
              reversed, SketchPlugin_MacroEllipticArc::REVERSED_ID(),
              ModelAPI_AttributeBoolean, /** Reversed flag for elliptic arc*/)
};

/// Pointer on Elliptic Arc object.
typedef std::shared_ptr<SketchAPI_MacroEllipticArc> MacroEllipticArcPtr;

#endif // SketchAPI_MacroEllipticArc_H_
