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

#ifndef SketchSolver_ConstraintDistance_H_
#define SketchSolver_ConstraintDistance_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintDistance
 *  \ingroup Plugins
 *  \brief   Convert distance constraint to SolveSpace structure
 */
class SketchSolver_ConstraintDistance : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintDistance(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myIsSigned(false),
      mySignValue(0.0)
  {}

  /// \brief Update constraint
  virtual void update();

  /// \brief Remove constraint
  virtual bool remove();

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr& theFeature, PlaneGCSSolver_Update*);

protected:
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr&              theValue,
                             std::vector<EntityWrapperPtr>& theAttributes);

  /// \brief This method is used in derived objects to check consistence of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint();

private:
  void addConstraintsToKeepSign();
  void removeConstraintsKeepingSign();

private:
  double myPrevValue; ///< previous value of distance (for correct calculation of a distance sign)
  bool   myIsSigned;  ///< true if the sign of point-line distance should be kept everytime
  GCSPointPtr myOddPoint; ///< auxiliary point to keep sign of distance
  double mySignValue;
};

#endif
