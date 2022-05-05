// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#ifndef SketchSolver_ConstraintOffset_H_
#define SketchSolver_ConstraintOffset_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintOffset
 *  \ingroup Plugins
 *  \brief   Convert offset to the solver's data model
 */
class SketchSolver_ConstraintOffset : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintOffset(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  /// \brief Update constraint
  virtual void update();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of entities of mirror constraint
  virtual void getAttributes(EntityWrapperPtr&, std::vector<EntityWrapperPtr>&);
};

#endif
