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

#include <PlaneGCSSolver_ConstraintWrapper.h>

PlaneGCSSolver_ConstraintWrapper::PlaneGCSSolver_ConstraintWrapper(
    const GCSConstraintPtr& theConstraint,
    const SketchSolver_ConstraintType& theType)
  : myGCSConstraints(1, theConstraint),
    myType(theType)
{
  myID = CID_UNKNOWN;
}

PlaneGCSSolver_ConstraintWrapper::PlaneGCSSolver_ConstraintWrapper(
    const std::list<GCSConstraintPtr>& theConstraints,
    const SketchSolver_ConstraintType& theType)
  : myGCSConstraints(theConstraints),
    myType(theType)
{
  myID = CID_UNKNOWN;
}

void PlaneGCSSolver_ConstraintWrapper::setId(const ConstraintID& theID)
{
  myID = theID;
}

void PlaneGCSSolver_ConstraintWrapper::setValueParameter(const ScalarWrapperPtr& theValue)
{
  myValueParam = theValue;
}

void PlaneGCSSolver_ConstraintWrapper::setValue(const double& theValue)
{
  myValueParam->setValue(theValue);
}

double PlaneGCSSolver_ConstraintWrapper::value() const
{
  return myValueParam ? myValueParam->value() : 0.0;
}
