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

#include <SolveSpaceSolver_ParameterWrapper.h>

#include <math.h>

SolveSpaceSolver_ParameterWrapper::SolveSpaceSolver_ParameterWrapper(const Slvs_Param& theParam)
  : myParameter(theParam)
{
}

ParameterID SolveSpaceSolver_ParameterWrapper::id() const
{
  return (ParameterID)myParameter.h;
}

void SolveSpaceSolver_ParameterWrapper::setValue(double theValue)
{
  myParameter.val = theValue;
}

double SolveSpaceSolver_ParameterWrapper::value() const
{
  return myParameter.val;
}

bool SolveSpaceSolver_ParameterWrapper::isEqual(const ParameterWrapperPtr& theOther)
{
  std::shared_ptr<SolveSpaceSolver_ParameterWrapper> anOtherParam =
      std::dynamic_pointer_cast<SolveSpaceSolver_ParameterWrapper>(theOther);
  return anOtherParam && fabs(value() - anOtherParam->value()) < tolerance;
}

bool SolveSpaceSolver_ParameterWrapper::update(const ParameterWrapperPtr& theOther)
{
  std::shared_ptr<SolveSpaceSolver_ParameterWrapper> anOther =
      std::dynamic_pointer_cast<SolveSpaceSolver_ParameterWrapper>(theOther);
  if (fabs(value() - anOther->value()) < tolerance)
    return false;
  myParameter.val = anOther->value();
  myIsParametric = theOther->isParametric();
  return true;
}
