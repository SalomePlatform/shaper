// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SolveSpaceSolver_ParameterWrapper_H_
#define SolveSpaceSolver_ParameterWrapper_H_

#include <SketchSolver_IParameterWrapper.h>
#include <SolveSpaceSolver_Solver.h>

/**
 *  Wrapper providing operations with parameters in SolveSpace.
 */
class SolveSpaceSolver_ParameterWrapper : public SketchSolver_IParameterWrapper
{
public:
  SolveSpaceSolver_ParameterWrapper(const Slvs_Param& theParam);

  /// \brief Return SolveSpace parameter
  const Slvs_Param& parameter() const
  { return myParameter; }
  /// \brief Return SolveSpace parameter to change
  Slvs_Param& changeParameter()
  { return myParameter; }

  /// \brief Return ID of current parameter
  virtual ParameterID id() const;

  /// \brief Change group for the parameter
  virtual void setGroup(const GroupID& theGroup)
  { myParameter.group = (Slvs_hGroup)theGroup; }

  /// \brief Return identifier of the group the parameter belongs to
  virtual GroupID group() const
  { return (GroupID)myParameter.group; }

  /// \brief Change value of parameter
  virtual void setValue(double theValue);
  /// \brief Return value of parameter
  virtual double value() const;

  /// \brief Compare current parameter with other
  virtual bool isEqual(const ParameterWrapperPtr& theOther);

  /// \brief Update value of parameter by the given one
  /// \return \c true if the value of parameter is changed
  virtual bool update(const std::shared_ptr<SketchSolver_IParameterWrapper>& theOther);

private:
  Slvs_Param myParameter;
};

#endif
