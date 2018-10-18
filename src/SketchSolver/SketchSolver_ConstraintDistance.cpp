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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>

#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <math.h>


void SketchSolver_ConstraintDistance::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1]) {
    if (myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID())
      myType = CONSTRAINT_HORIZONTAL_DISTANCE;
    else if (myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
      myType = CONSTRAINT_VERTICAL_DISTANCE;
    else
      myType = CONSTRAINT_PT_PT_DISTANCE;
  } else if (theAttributes[2] && theAttributes[2]->type() == ENTITY_LINE)
    myType = CONSTRAINT_PT_LINE_DISTANCE;
  else
    theAttributes.clear();

  myPrevValue = 0.0;
}

void SketchSolver_ConstraintDistance::adjustConstraint()
{
  if (getType() == CONSTRAINT_PT_LINE_DISTANCE) {
    bool isSigned = myBaseConstraint->boolean(SketchPlugin_ConstraintDistance::SIGNED())->value();
    if (myIsSigned == isSigned)
      return; // distance type is not changed => nothing to adjust

    // Adjust point-line distance by setting/removing additional constraints
    if (isSigned)
      addConstraintsToKeepSign();
    else
      removeConstraintsKeepingSign();
    myIsSigned = isSigned;
  }
}

void SketchSolver_ConstraintDistance::update()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint);
  myPrevValue = aConstraint->value();

  SketchSolver_Constraint::update();
}

bool SketchSolver_ConstraintDistance::remove()
{
  removeConstraintsKeepingSign();
  return SketchSolver_Constraint::remove();
}

void SketchSolver_ConstraintDistance::addConstraintsToKeepSign()
{
  std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
      std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

  ConstraintWrapperPtr aConstraint = aStorage->constraint(myBaseConstraint);
  std::list<GCSConstraintPtr> aGCSConstraints = aConstraint->constraints();

  // calculate projection of the point on the line and find a sign of a distance
  EntityWrapperPtr aDistPoint, aDistLine;
  for (int i = 0; i < 2; ++i) {
    AttributePtr anAttr = myBaseConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(i));
    EntityWrapperPtr anEntity = myStorage->entity(anAttr);
    if (anEntity->type() == ENTITY_POINT)
      aDistPoint = anEntity;
    else if (anEntity->type() == ENTITY_LINE)
      aDistLine = anEntity;
  }
  std::shared_ptr<GeomAPI_Lin2d> aLine = PlaneGCSSolver_Tools::line(aDistLine);
  std::shared_ptr<GeomAPI_Pnt2d> aPoint = PlaneGCSSolver_Tools::point(aDistPoint);

  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());
  if (aLineVec->cross(aPtLineVec) >= 0.)
    mySignValue = PI/2.0;
  else
    mySignValue = - PI/2.0;
  double aDot = aPtLineVec->dot(aLineVec);
  std::shared_ptr<GeomAPI_XY> aProjectedPnt =
      aLine->location()->xy()->added(aLineVec->multiplied(aDot));

  // create auxiliary point on the line and set auxiliary constraints
  myOddPoint = GCSPointPtr(new GCS::Point);
  myOddPoint->x = aStorage->createParameter();
  myOddPoint->y = aStorage->createParameter();
  *(myOddPoint->x) = aProjectedPnt->x();
  *(myOddPoint->y) = aProjectedPnt->y();

  PointWrapperPtr aPointWr = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(aDistPoint);
  EdgeWrapperPtr anEdgeWr = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(aDistLine);
  std::shared_ptr<GCS::Line> aGCSLine = std::dynamic_pointer_cast<GCS::Line>(anEdgeWr->entity());
  // point-on-line
  GCSConstraintPtr aNewConstraint(new GCS::ConstraintPointOnLine(*myOddPoint, *aGCSLine));
  aGCSConstraints.push_back(aNewConstraint);
  // angle which keep orientation
  aNewConstraint = GCSConstraintPtr(new GCS::ConstraintL2LAngle(
      aGCSLine->p1, aGCSLine->p2, *myOddPoint, *(aPointWr->point()), &mySignValue));
  aGCSConstraints.push_back(aNewConstraint);

  aConstraint->setConstraints(aGCSConstraints);

  aStorage->removeConstraint(myBaseConstraint);
  aStorage->addConstraint(myBaseConstraint, aConstraint);
}

void SketchSolver_ConstraintDistance::removeConstraintsKeepingSign()
{
  if (!myOddPoint)
    return; // no sign kept => nothing to remove

  std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
      std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

  ConstraintWrapperPtr aConstraint = aStorage->constraint(myBaseConstraint);
  std::list<GCSConstraintPtr> aGCSConstraints = aConstraint->constraints();

  aStorage->removeConstraint(myBaseConstraint);

  // remove parameters related to auxiliary point
  GCS::SET_pD aParams;
  aParams.insert(myOddPoint->x);
  aParams.insert(myOddPoint->y);
  aStorage->removeParameters(aParams);

  aGCSConstraints.pop_back();
  aGCSConstraints.pop_back();
  aConstraint->setConstraints(aGCSConstraints);
  aStorage->addConstraint(myBaseConstraint, aConstraint);

  myIsSigned = false;
}
