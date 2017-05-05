// Copyright (C) 2015-20xx CEA/DEN, EDF R&D

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
