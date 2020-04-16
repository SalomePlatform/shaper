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

#include <SketchSolver_ConstraintAngle.h>

#include <ModelAPI_AttributeInteger.h>
#include <SketchPlugin_ConstraintAngle.h>

void SketchSolver_ConstraintAngle::getAttributes(
    EntityWrapperPtr& theValue, std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  myType = myBaseConstraint->integer(SketchPlugin_ConstraintAngle::TYPE_ID())->value();
}


void SketchSolver_ConstraintAngle::adjustConstraint()
{
  int aType = myBaseConstraint->integer(SketchPlugin_ConstraintAngle::TYPE_ID())->value();
  if (aType != myType) {
    myType = aType;
    myStorage->setNeedToResolve(true);
  }
}
