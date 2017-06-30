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

#ifndef SketchSolver_ConstraintTangent_H_
#define SketchSolver_ConstraintTangent_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintTangent
 *  \ingroup Plugins
 *  \brief   Convert tangency constraint to SolveSpace structure
 */
class SketchSolver_ConstraintTangent : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintTangent(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      isArcArcInternal(false),
      myCurveCurveAngle(0.0)
  {}

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater);

protected:
  /// \brief Converts SketchPlugin constraint to a list of solver constraints
  virtual void process();

  /// \brief Remove current constraint from the storage and build is again
  void rebuild();

  /// \brief This method is used in derived objects to check consistency of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint();

private:
  bool isArcArcInternal;
  double myCurveCurveAngle;
  AttributePtr mySharedPoint;
};

#endif
