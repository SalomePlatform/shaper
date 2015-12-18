// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMultiTranslation.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMultiTranslation_H_
#define SketchSolver_ConstraintMultiTranslation_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintMulti.h>

#include "GeomDataAPI_Point2D.h"

/** \class   SketchSolver_ConstraintMultiTranslation
 *  \ingroup Plugins
 *  \brief   Convert translated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiTranslation : public SketchSolver_ConstraintMulti
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMultiTranslation(ConstraintPtr theConstraint) :
      SketchSolver_ConstraintMulti(theConstraint)
  {}

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of translated entities
  /// \param[out] theStartPoint start point of translation
  /// \param[out] theEndPoint   final point of translation
  /// \param[out] theEntities   list of entities and their translated copies
  void getAttributes(EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
                     std::list< std::list<EntityWrapperPtr> >& theEntities);

  /// \brief Update parameters (called from base class)
  virtual void updateLocal();

private:
  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbObjects();

private:
  AttributePoint2DPtr myStartPointAttribute;
  AttributePoint2DPtr myEndPointAttribute;
  bool                myIsFullValue;
};

#endif
