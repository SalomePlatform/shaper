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

#ifndef SketcherPrs_Factory_H_
#define SketcherPrs_Factory_H_

#include "SketcherPrs.h"
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_CompositeFeature.h>

#include <GeomAPI_Ax3.h>
#include <GeomAPI_AISObject.h>

#define GET_CONSTRAINT_PRS(NAME) \
  static AISObjectPtr NAME(ModelAPI_Feature* theConstraint, \
                           SketchPlugin_Sketch* theSketcher, \
                           AISObjectPtr thePrevious);

/**
* Class which creates constraints presentations
*/
class SKETCHERPRS_EXPORT SketcherPrs_Factory
{
public:
  /// Creates collinear constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(collinearConstraint)

  /// Creates parallel constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(parallelConstraint)

  /// Creates coincedent perpendicular presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(perpendicularConstraint)

  /// Creates rigid constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(rigidConstraint)

  /// Creates horizontal constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(horisontalConstraint)

  /// Creates vertical constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(verticalConstraint)

  /// Creates equal constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(equalConstraint)

  /// Creates tangent constraiont presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(tangentConstraint)

  /// Creates middle constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(middleConstraint)

  /// Creates mirror constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(mirrorConstraint)

  /// Creates translate constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(translateConstraint)

  /// Creates rotate constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(rotateConstraint)

  /// Creates coincedent constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(coincidentConstraint)

  /// Creates length dimension presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(lengthDimensionConstraint)

  /// Creates angle constraint presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(angleConstraint)

  /// Creates radius dimension presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(radiusConstraint)

  /// Creates radius dimension presentation
  /// \param theConstraint the constraint
  /// \param thePlane the current sketch plane
  /// \param thePrevious the previous presentation
  GET_CONSTRAINT_PRS(offsetObject)
};

#endif
