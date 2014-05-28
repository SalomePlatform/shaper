// File:    SketchSolver_Constraint.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Constraint_Headerfile
#define SketchSolver_Constraint_Headerfile

#include "SketchSolver.h"

#include <SketchPlugin_Constraint.h>

#include <string>
#include <vector>

/** \class   SketchSolver_Constraint
 *  \ingroup DataModel
 *  \brief   Obtain information about SketchPlugin's constraint
 */
class SketchSolver_Constraint
{
public:
  SketchSolver_Constraint();
  SketchSolver_Constraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Compute constraint type according to SolveSpace identifiers
   *         and verify that constraint parameters are correct
   *  \param[in]  theConstraint constraint which type should be determined
   *  \return identifier of constraint type or SLVS_C_UNKNOWN if the type is wrong
   */
  const int& getType(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);
  /// \brief Returns the type of myConstraint member
  inline const int& getType() const
  { return myType; }

  /// \brief Returns list of attributes names in the correct order required by SolveSpace
  inline const std::vector<std::string>& getAttributes() const
  { return myAttributesList; }

private:
  boost::shared_ptr<SketchPlugin_Constraint> myConstraint;
  int                                        myType;
  std::vector<std::string>                   myAttributesList;
};

#endif
