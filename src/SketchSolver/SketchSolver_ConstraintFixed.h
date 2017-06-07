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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketchSolver_ConstraintFixed_H_
#define SketchSolver_ConstraintFixed_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintFixed
 *  \ingroup Plugins
 *  \brief   Stores data of the Fixed constraint
 *
 *  Fixed constraint may have NULL basic SketchPlugin constraint,
 *  because the Fixed constraint may be temporary for correct moving of objects.
 */
class SketchSolver_ConstraintFixed : public SketchSolver_Constraint
{
public:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintFixed(ConstraintPtr theConstraint);
  /// Creates temporary constraint based on feature (useful while the feature is being moved)
  SketchSolver_ConstraintFixed(FeaturePtr theFeature);

  /// \brief Block or unblock events from this constraint
  virtual void blockEvents(bool isBlocked);

  /// \brief Set coordinates of fixed feature to the values where it has been dragged.
  ///        Useful when the feature is being moved.
  void moveFeature();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theBaseEntity  the entity which coordinates should be fixed
  /// \param[out] theFixedEntity the entity containing fixed values
  virtual void getAttributes(EntityWrapperPtr& theBaseEntity,
                             EntityWrapperPtr& theFixedEntity);

  /// \brief Fixed feature basing on its type
  /// \param theFeature [in]  feature, converted to solver specific format
  virtual void fixFeature(EntityWrapperPtr theFeature);

private:
  FeaturePtr myBaseFeature; ///< fixed feature (when it is set, myBaseConstraint should be NULL)

  ConstraintWrapperPtr myConstraint;
  std::vector<double> myFixedValues;
  EntityWrapperPtr    myFixedEntity;
};

#endif
