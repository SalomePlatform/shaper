// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMultiRotation.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMultiRotation_H_
#define SketchSolver_ConstraintMultiRotation_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintMulti.h>

#include "GeomDataAPI_Point2D.h"

/** \class   SketchSolver_ConstraintMultiRotation
 *  \ingroup Plugins
 *  \brief   Convert rotated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiRotation : public SketchSolver_ConstraintMulti
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMultiRotation(ConstraintPtr theConstraint) :
      SketchSolver_ConstraintMulti(theConstraint)
  {}

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of rotated entities
  /// \param[out] theCenter   central point of rotation
  /// \param[out] theAngle    rotation angle
  /// \param[out] theFullValue  applying translation using the disstance as a full or single value
  /// \param[out] theEntities list of entities and their rotated copies
  void getAttributes(EntityWrapperPtr& theCenter, double& theAngle, bool& theFullValue,
                     std::list< std::list<EntityWrapperPtr> >& theEntities);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters (called from base class)
  virtual void updateLocal();

private:
  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbObjects();

  AttributePoint2DPtr myCenterPointAttribute; ///< a center of rotation
  double              myAngle;           ///< angle of rotation
  bool                myIsFullValue;     ///< value whether the angle is a full or single for objects
};

#endif
