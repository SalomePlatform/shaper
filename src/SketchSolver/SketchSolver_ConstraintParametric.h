// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintParametric.h
// Created: 15 Jun 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintParametric_H_
#define SketchSolver_ConstraintParametric_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintRigid.h>

/** \class   SketchSolver_ConstraintParametric
 *  \ingroup Plugins
 *  \brief   Stores data of Rigid (Fixed) constraint for the attribute
 *           which coordinates are given by parametric expression
 */
class SketchSolver_ConstraintParametric : public SketchSolver_ConstraintRigid
{
private:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintParametric()
    : SketchSolver_ConstraintRigid(ConstraintPtr())
  {}

public:
  /// Creates temporary constraint based on feature
  SketchSolver_ConstraintParametric(AttributePtr theAttribute);

  /// \brief Update constraint
  virtual void update(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Update points coincident with parametric one
  virtual void refresh();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Convert attribute to the entity
  /// \param[out] theAttributeID   identifier of the entity related to the attribute
  virtual void getAttributes(Slvs_hEntity& theAttributeID);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

private:
  /// \brief Create SolveSpace line with given coordinates
  /// \return created line
  Slvs_Entity createLine(double theStartX, double theStartY, double theEndX, double theEndY);

private:
  AttributePtr myBaseAttribute; ///< attribute given by expression
  Slvs_hEntity myHorizLineID;   ///< identifier of horizontal line, containing the point
  Slvs_hEntity myVertLineID;    ///< identifier of vertical line, containing the point
  double myX, myY;
};

#endif
