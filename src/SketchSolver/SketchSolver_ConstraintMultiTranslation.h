// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMultiTranslation.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMultiTranslation_H_
#define SketchSolver_ConstraintMultiTranslation_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintMulti.h>

/** \class   SketchSolver_ConstraintMultiTranslation
 *  \ingroup Plugins
 *  \brief   Convert translated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiTranslation : public SketchSolver_ConstraintMulti
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMultiTranslation(ConstraintPtr theConstraint) :
      SketchSolver_ConstraintMulti(theConstraint),
      myTranslationLine(SLVS_E_UNKNOWN)
  {}

  virtual int getType() const
  { return SLVS_C_MULTI_TRANSLATION; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of translated entities
  /// \param[out] theStartPoint ID of start point of translation
  /// \param[out] theEndPoint   ID of final point of translation
  /// \param[out] thePoints     list of IDs of initial points and their translated copies
  /// \param[out] theEntities   list of IDs of entities and their translated copies
  void getAttributes(Slvs_hEntity& theStartPoint, Slvs_hEntity& theEndPoint,
                     std::vector< std::vector<Slvs_hEntity> >& thePoints,
                     std::vector< std::vector<Slvs_hEntity> >& theEntities);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters (called from base class)
  virtual void updateLocal()
  {}

private:
  /// \brief Convert absolute coordinates to relative coordinates
  virtual void getRelative(double theAbsX, double theAbsY, double& theRelX, double& theRelY);
  /// \brief Convert relative coordinates to absolute coordinates
  virtual void getAbsolute(double theRelX, double theRelY, double& theAbsX, double& theAbsY);
  /// \brief Apply transformation for relative coordinates
  virtual void transformRelative(double& theX, double& theY);

private:
  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbObjects();

private:
  Slvs_hEntity myTranslationLine; ///< ID of translation line

  double myDelta[2]; ///< increment of translation
};

#endif
