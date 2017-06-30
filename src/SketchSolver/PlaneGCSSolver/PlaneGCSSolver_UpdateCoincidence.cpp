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

#include <PlaneGCSSolver_UpdateCoincidence.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintMiddle.h>

static bool hasSamePoint(const std::set<EntityWrapperPtr>& theList,
                         const EntityWrapperPtr& thePoint);


void PlaneGCSSolver_UpdateCoincidence::attach(SketchSolver_Constraint* theObserver,
                                              const std::string& theType)
{
  if (theType == GROUP()) {
    std::list<SketchSolver_Constraint*>::iterator aPlaceToAdd = myObservers.end();
    // point-point coincidence is placed first,
    // other constraints are sorted by their type
    for (aPlaceToAdd = myObservers.begin(); aPlaceToAdd != myObservers.end(); ++aPlaceToAdd)
      if ((*aPlaceToAdd)->getType() > theObserver->getType())
        break;
    myObservers.insert(aPlaceToAdd, theObserver);
  } else
    myNext->attach(theObserver, theType);
}

void PlaneGCSSolver_UpdateCoincidence::update(const FeaturePtr& theFeature)
{
  if (theFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID() ||
      theFeature->getKind() == SketchPlugin_ConstraintMiddle::ID() ||
      theFeature->getKind() == SketchPlugin_ConstraintCollinear::ID()) {
    myCoincident.clear();
    // notify listeners and stop procesing
    std::list<SketchSolver_Constraint*>::iterator anIt = myObservers.begin();
    for (; anIt != myObservers.end(); ++anIt)
      (*anIt)->notify(theFeature, this);
  } else
    myNext->update(theFeature);
}

static bool hasAnotherExternalPoint(const std::set<EntityWrapperPtr>& theCoincidences,
                                    const EntityWrapperPtr& thePoint)
{
  std::set<EntityWrapperPtr>::const_iterator anIt = theCoincidences.begin();
  for (; anIt != theCoincidences.end(); ++anIt)
    if ((*anIt)->type() == ENTITY_POINT && (*anIt)->isExternal() && *anIt != thePoint)
      return true;
  return false;
}

bool PlaneGCSSolver_UpdateCoincidence::addCoincidence(
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2)
{
  bool isAccepted = true;
  std::list<CoincidentEntities>::iterator aFound[2] = {
    findGroupOfCoincidence(theEntity1),
    findGroupOfCoincidence(theEntity2)
  };

  if (aFound[0] == myCoincident.end() && aFound[1] == myCoincident.end()) {
    // new group of coincidence
    myCoincident.push_back(CoincidentEntities(theEntity1, theEntity2));
  } else if (aFound[0] == myCoincident.end()) {
    isAccepted = addToGroupOfCoincidence(*aFound[1], theEntity1);
  } else if (aFound[1] == myCoincident.end()) {
    isAccepted = addToGroupOfCoincidence(*aFound[0], theEntity2);
  } else if (aFound[0] == aFound[1]) { // same group => already coincident
    isAccepted = false;
  } else { // merge two groups
    // first check the external points are equal
    EntityWrapperPtr anExternal0 = aFound[0]->externalPoint();
    EntityWrapperPtr anExternal1 = aFound[1]->externalPoint();
    if (anExternal0 && anExternal1) {
      std::set<EntityWrapperPtr> anExtList;
      anExtList.insert(anExternal0);
      if (hasSamePoint(anExtList, anExternal1)) {
        // no need to add coincidence, because all points are
        // already coincident to correct external points
        isAccepted = false;
      }
    }

    // merge
    aFound[0]->merge(*aFound[1]);
    myCoincident.erase(aFound[1]);
  }

  return isAccepted;
}

bool PlaneGCSSolver_UpdateCoincidence::isPointOnEntity(
    const EntityWrapperPtr& thePoint,
    const EntityWrapperPtr& theEntity)
{
  std::list<CoincidentEntities>::iterator anIt = findGroupOfCoincidence(thePoint);
  if (anIt == myCoincident.end())
    return false;

  if (anIt->isExist(theEntity))
    return true;

  if (theEntity->type() == ENTITY_LINE) {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(
        std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity)->entity());
    return anIt->isExist(aLine->p1) || anIt->isExist(aLine->p2);
  }
  return false;
}

std::list<PlaneGCSSolver_UpdateCoincidence::CoincidentEntities>::iterator
  PlaneGCSSolver_UpdateCoincidence::findGroupOfCoincidence(const EntityWrapperPtr& theEntity)
{
  if (theEntity->type() != ENTITY_POINT)
    return myCoincident.end();

  std::list<CoincidentEntities>::iterator aFound = myCoincident.begin();
  for (; aFound != myCoincident.end(); ++aFound)
    if (aFound->isExist(theEntity))
      break;
  return aFound;
}

bool PlaneGCSSolver_UpdateCoincidence::addToGroupOfCoincidence(
    CoincidentEntities& theGroup, const EntityWrapperPtr& theEntity)
{
  if (theGroup.isExist(theEntity))
    return false;
  return theGroup.add(theEntity);
}





static const GCS::Point& toPoint(const EntityWrapperPtr& theEntity)
{
  PointWrapperPtr aPoint = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
  return *(aPoint->point());
}

static double squareDistance(const GCS::Point& thePoint1, const GCS::Point& thePoint2)
{
  double dx = *thePoint1.x - *thePoint2.x;
  double dy = *thePoint1.y - *thePoint2.y;
  return dx * dx + dy * dy;
}

static bool hasSamePoint(const std::set<EntityWrapperPtr>& theList, const GCS::Point& thePoint)
{
  std::set<EntityWrapperPtr>::const_iterator anIt = theList.begin();
  for (; anIt != theList.end(); ++anIt)
    if (squareDistance(thePoint, toPoint(*anIt)) < 1.e-14)
      return true;
  return false;
}

bool hasSamePoint(const std::set<EntityWrapperPtr>& theList,
                  const EntityWrapperPtr& thePoint)
{
  return hasSamePoint(theList, toPoint(thePoint));
}


PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::CoincidentEntities(
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2)
{
  add(theEntity1);
  add(theEntity2);
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::add(
    const EntityWrapperPtr& theEntity)
{
  bool isAdded = true;
  if (theEntity->type() == ENTITY_POINT) {
    if (theEntity->isExternal()) {
      isAdded = !hasSamePoint(myExternalPoints, theEntity);
      myExternalPoints.insert(theEntity);
    } else
      myPoints.insert(theEntity);
  } else
    myFeatures.insert(theEntity);
  return isAdded;
}

void PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::remove(
    const EntityWrapperPtr& theEntity)
{
  if (theEntity->type() == ENTITY_POINT) {
    if (theEntity->isExternal())
      myExternalPoints.erase(theEntity);
    else
      myPoints.erase(theEntity);
  } else
    myFeatures.erase(theEntity);
}

void PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::merge(
    const CoincidentEntities& theOther)
{
  myExternalPoints.insert(theOther.myExternalPoints.begin(), theOther.myExternalPoints.end());
  myPoints.insert(theOther.myPoints.begin(), theOther.myPoints.end());
  myFeatures.insert(theOther.myFeatures.begin(), theOther.myFeatures.end());
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::isExist(
    const EntityWrapperPtr& theEntity) const
{
  if (theEntity->type() == ENTITY_POINT) {
    if (theEntity->isExternal())
      return myExternalPoints.find(theEntity) != myExternalPoints.end();
    else
      return myPoints.find(theEntity) != myPoints.end();
  }

  return myFeatures.find(theEntity) != myFeatures.end();
}

bool PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::isExist(
    const GCS::Point& thePoint) const
{
  return hasSamePoint(myExternalPoints, thePoint) || hasSamePoint(myPoints, thePoint);
}

EntityWrapperPtr PlaneGCSSolver_UpdateCoincidence::CoincidentEntities::externalPoint() const
{
  return myExternalPoints.empty() ? EntityWrapperPtr() : *myExternalPoints.begin();
}
