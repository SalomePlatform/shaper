// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef PlaneGCSSolver_ConstraintWrapper_H_
#define PlaneGCSSolver_ConstraintWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_ScalarWrapper.h>

#include <list>

/**
 *  Wrapper providing operations with PlaneGCS constraints.
 */
class PlaneGCSSolver_ConstraintWrapper
{
public:
  PlaneGCSSolver_ConstraintWrapper(const GCSConstraintPtr&            theConstraint,
                                   const SketchSolver_ConstraintType& theType);
  PlaneGCSSolver_ConstraintWrapper(const std::list<GCSConstraintPtr>& theConstraints,
                                   const SketchSolver_ConstraintType& theType);

  /// \brief Return list of constraints
  const std::list<GCSConstraintPtr>& constraints() const
  { return myGCSConstraints; }
  /// \brief Change list of constraints
  void setConstraints(const std::list<GCSConstraintPtr>& theConstraints)
  { myGCSConstraints = theConstraints; }

  /// \brief Return ID of current constraint
  const ConstraintID& id() const
  { return myID; }
  /// \brief Change constraint ID
  virtual void setId(const ConstraintID& theID);

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const
  { return myType; }

  /// \brief Assign numeric parameter of constraint
  virtual void setValue(const double& theValue);
  /// \brief Return numeric parameter of constraint
  virtual double value() const;

  /// \brief Change parameter representing the value of constraint
  void setValueParameter(const ScalarWrapperPtr& theValue);
  /// \brief Return parametric representation of constraint value
  const ScalarWrapperPtr& valueParameter() const
  { return myValueParam; }

private:
  ConstraintID                myID;
  SketchSolver_ConstraintType myType;
  ScalarWrapperPtr            myValueParam;
  std::list<GCSConstraintPtr> myGCSConstraints;
};

typedef std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> ConstraintWrapperPtr;

#endif
