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

#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Line.h>


void SketchSolver_ConstraintLength::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] ||
      theAttributes[2]->type() != ENTITY_LINE) {
    theAttributes.clear();
    return;
  }

  AttributeRefAttrPtr aRefAttr = myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLine = ModelAPI_Feature::feature(aRefAttr->object());

  theAttributes[0] = myStorage->entity(aLine->attribute(SketchPlugin_Line::START_ID()));
  theAttributes[1] = myStorage->entity(aLine->attribute(SketchPlugin_Line::END_ID()));

  myType = CONSTRAINT_PT_PT_DISTANCE;
}

