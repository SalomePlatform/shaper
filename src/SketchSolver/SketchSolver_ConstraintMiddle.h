// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef SketchSolver_ConstraintMiddle_H_
#define SketchSolver_ConstraintMiddle_H_

#include <SketchSolver_ConstraintCoincidence.h>

/** \class   SketchSolver_ConstraintMiddle
 *  \ingroup Plugins
 *  \brief   Converts middle-point constraint to the constraint applicable for solver
 */
class SketchSolver_ConstraintMiddle : public SketchSolver_ConstraintCoincidence
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMiddle(ConstraintPtr theConstraint)
    : SketchSolver_ConstraintCoincidence(theConstraint)
  {}

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater);

  /// \brief Remove constraint
  virtual bool remove();

protected:
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr&              theValue,
                             std::vector<EntityWrapperPtr>& theAttributes);

private:
  ConstraintWrapperPtr myMiddle;
  GCSPointPtr myOddPoint; ///< auxiliary point to adjust midpoint-on-arc
};

#endif
