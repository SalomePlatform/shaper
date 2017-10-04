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

// File:    SketchSolver_ConstraintMovement.h
// Created: 15 Jun 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMovement_H_
#define SketchSolver_ConstraintMovement_H_

#include <SketchSolver_ConstraintFixed.h>

class GeomAPI_Pnt2d;

/** \class   SketchSolver_ConstraintMovement
 *  \ingroup Plugins
 *  \brief   Stores data to the Fixed constraint for the moved feature only
 */
class SketchSolver_ConstraintMovement : public SketchSolver_ConstraintFixed
{
public:
  /// Creates movement constraint based on feature
  SketchSolver_ConstraintMovement(FeaturePtr theFeature);

  /// Creates movement constraint based on point
  SketchSolver_ConstraintMovement(AttributePtr thePoint);

  /// \brief Set coordinates of the start point of the movement
  void startPoint(const std::shared_ptr<GeomAPI_Pnt2d>& theStartPoint);

  /// \brief Set coordinates of fixed feature to the values where it has been dragged.
  ///        Useful when the feature is being moved.
  void moveTo(const std::shared_ptr<GeomAPI_Pnt2d>& theDestinationPoint);

  /// \brief Block or unblock events from this constraint
  virtual void blockEvents(bool isBlocked);

  /// \brief Returns moved feature
  FeaturePtr movedFeature() const
  { return myMovedFeature; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Create Fixed constraint for the feature basing on its type and moved point
  /// \return Fixed constraint
  ConstraintWrapperPtr initMovement();

  /// \brief Create constraint to fix moved arc extremity
  ConstraintWrapperPtr fixArcExtremity(const EntityWrapperPtr& theArcExtremity);

  /// \brief Creat constraint to fix moved point on circle/arc
  ConstraintWrapperPtr fixPointOnCircle(const EntityWrapperPtr& theCircular);

private:
  FeaturePtr       myMovedFeature; ///< fixed feature (if set, myBaseConstraint should be NULL)
  AttributePtr     myDraggedPoint; ///< one of the feature points which has been moved
  std::shared_ptr<GeomAPI_Pnt2d> myStartPoint; ///< start point of the movement

  bool mySimpleMove; ///< simple move, thus all parameters should be increased by movement delta
};

#endif
