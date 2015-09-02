// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMultiRotation.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMultiRotation_H_
#define SketchSolver_ConstraintMultiRotation_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintMulti.h>

/** \class   SketchSolver_ConstraintMultiRotation
 *  \ingroup Plugins
 *  \brief   Convert rotated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiRotation : public SketchSolver_ConstraintMulti
{
public:
  SketchSolver_ConstraintMultiRotation(ConstraintPtr theConstraint) :
      SketchSolver_ConstraintMulti(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_MULTI_ROTATION; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of rotated entities
  /// \param[out] theCenter   ID of central point of rotation
  /// \param[out] theAngle    rotation angle
  /// \param[out] thePoints   list of IDs of initial points and their rotated copies
  /// \param[out] theEntities list of IDs of entities and their rotated copies
  void getAttributes(Slvs_hEntity& theCenter, double& theAngle,
                     std::vector< std::vector<Slvs_hEntity> >& thePoints,
                     std::vector< std::vector<Slvs_hEntity> >& theEntities);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters (called from base class)
  virtual void updateLocal();

private:
  /// \brief Convert absolute coordinates to relative coordinates
  virtual void getRelative(double theAbsX, double theAbsY, double& theRelX, double& theRelY);
  /// \brief Convert relative coordinates to absolute coordinates
  virtual void getAbsolute(double theRelX, double theRelY, double& theAbsX, double& theAbsY);
  /// \brief Apply transformation for relative coordinates
  virtual void transformRelative(double& theX, double& theY);

private:
  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbCopies();

private:
  Slvs_hEntity myRotationCenter; ///< ID of center of rotation
  double       myAngle;           ///< angle of rotation

  double myCenterCoord[2]; ///< coordinates of rotation center
  double myRotationVal[2]; ///< sinus and cosinus of rotation angle
};

#endif
