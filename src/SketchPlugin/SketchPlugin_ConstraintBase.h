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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketchPlugin_ConstraintBase_H_
#define SketchPlugin_ConstraintBase_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Sketch.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomAPI_IPresentable.h>
#include <GeomAPI_ICustomPrs.h>

#include <list>

/** Description: 
 *    Each constraint uses a set of parameters. In the SolveSpace library 
 *    these parameters are named "valA", "ptA", "ptB", "entityA", "entityB". 
 *    The "ptA" and "ptB" parameters represents a point in the constraint.
 *    The "entityA" and "entityB" represents any other object (and a point too).
 *    And the "valA" represents a real value.
 *
 *    The attributes below are named independent of the SolveSpace.
 *    Some of them may be unused. 
 *
 *    Also the list of possible attributes is provided to simplify assignment.
 */

#define SKETCH_DIMENSION_COLOR "64,128,225"

/** \class SketchPlugin_ConstraintBase
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint between other features.
 *         Some feature's methods implemented here as dummy to
 *         Base class for all constraints.
 */
class SketchPlugin_ConstraintBase : public SketchPlugin_Constraint, public GeomAPI_IPresentable,
                                    public GeomAPI_ICustomPrs
{
 public:
  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /** \brief Adds sub-feature of the higher level feature (sub-element of the sketch)
   *  \param theFeature sub-feature
   */
  SKETCHPLUGIN_EXPORT virtual const void addSub(const FeaturePtr& theFeature);
  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

      /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs)
  {
    return theDefaultPrs->customisePresentation(theResult, thePrs, theDefaultPrs);
  }

 protected:
  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintBase()
  {
  }
};

#endif
