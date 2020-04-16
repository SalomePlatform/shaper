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

#include <SketchSolver_ConstraintEqual.h>
#include <SketchSolver_Error.h>
#include <PlaneGCSSolver_AttributeBuilder.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <SketchPlugin_Line.h>

void SketchSolver_ConstraintEqual::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] || !theAttributes[3]) {
    theAttributes.clear();
    return;
  }

  // Check the quantity of entities of each type
  int aNbLines = 0;
  int aNbArcs = 0;
  int aNbCircs = 0;
  int aNbEllipses = 0;
  bool isArcFirst = false; // in line-arc equivalence, the line should be first
  std::vector<EntityWrapperPtr>::iterator anAttrIt = theAttributes.begin() + 2;
  for (; anAttrIt != theAttributes.end(); ++anAttrIt) {
    SketchSolver_EntityType aType = (*anAttrIt)->type();
    if (aType == ENTITY_LINE)
      ++aNbLines;
    else if (aType == ENTITY_CIRCLE)
      ++aNbCircs;
    else if (aType == ENTITY_ARC) {
      ++aNbArcs;
      isArcFirst = (aNbLines == 0);
    }
    else if (aType == ENTITY_ELLIPSE || aType == ENTITY_ELLIPTIC_ARC)
      ++aNbEllipses;
  }

  if (aNbLines + aNbArcs + aNbCircs + aNbEllipses != 2 ||
     (aNbArcs == 1 && aNbEllipses != 0) || aNbEllipses == 1) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  switch (aNbLines) {
  case 0:
    if (aNbEllipses == 2) {
      myType = CONSTRAINT_EQUAL_ELLIPSES;
      std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
          std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);
      myAuxValue = ScalarWrapperPtr(new PlaneGCSSolver_ScalarWrapper(aStorage->createParameter()));
      theValue = myAuxValue;
    }
    else
      myType = CONSTRAINT_EQUAL_RADIUS;
    break;
  case 1:
    myType = CONSTRAINT_EQUAL_LINE_ARC;
    if (isArcFirst) { // change the order of arc and line
      EntityWrapperPtr aTmp = theAttributes[2];
      theAttributes[2] = theAttributes[3];
      theAttributes[3] = aTmp;
    }
    break;
  default:
    myType = CONSTRAINT_EQUAL_LINES;

    AttributeRefAttrPtr aRefLine1 =
        myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
    FeaturePtr aLine1 = ModelAPI_Feature::feature(aRefLine1->object());
    if (aLine1) {
      PlaneGCSSolver_AttributeBuilder aBuilder(myStorage);
      // store length of first line as a value for constraint
      // (will be used to make equal lengths of lines)
      theValue = aBuilder.createAttribute(aLine1->attribute(SketchPlugin_Line::LENGTH_ID()));
    }
    break;
  }
}

bool SketchSolver_ConstraintEqual::remove()
{
  if (myAuxValue) {
    std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);
    GCS::SET_pD aParams;
    aParams.insert(myAuxValue->scalar());
    aStorage->removeParameters(aParams);
  }
  return SketchSolver_Constraint::remove();
}
