// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Storage.cpp
// Created: 18 Mar 2015
// Author:  Artem ZHIDKOV

#include <SketchSolver_Storage.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <math.h>

/** \brief Search the entity/parameter with specified ID in the list of elements
 *  \param[in] theEntityID unique ID of the element
 *  \param[in] theEntities list of elements
 *  \return position of the found element or -1 if the element is not found
 */
template<typename T>
static int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities);

/// \brief Compare two parameters to be different
static bool IsNotEqual(const Slvs_Param& theParam1, const Slvs_Param& theParam2);
/// \brief Compare two entities to be different
static bool IsNotEqual(const Slvs_Entity& theEntity1, const Slvs_Entity& theEntity2);
/// \brief Compare two constriants to be different
static bool IsNotEqual(const Slvs_Constraint& theConstraint1, const Slvs_Constraint& theConstraint2);


SketchSolver_Storage::SketchSolver_Storage()
  : myParamMaxID(SLVS_E_UNKNOWN),
    myEntityMaxID(SLVS_E_UNKNOWN),
    myConstrMaxID(SLVS_C_UNKNOWN),
    myFixed(SLVS_E_UNKNOWN),
    myNeedToResolve(false),
    myDuplicatedConstraint(false)
{
}

Slvs_hParam SketchSolver_Storage::addParameter(const Slvs_Param& theParam)
{
  if (theParam.h > 0 && theParam.h <= myParamMaxID) {
    // parameter is already used, rewrite it
    return updateParameter(theParam);
  }

  Slvs_Param aParam = theParam;
  if (aParam.h > myParamMaxID)
    myParamMaxID = aParam.h;
  else
    aParam.h = ++myParamMaxID;
  myParameters.push_back(aParam);
  myNeedToResolve = true;
  return aParam.h;
}

Slvs_hParam SketchSolver_Storage::updateParameter(const Slvs_Param& theParam)
{
  if (theParam.h > 0 && theParam.h <= myParamMaxID) {
    // parameter already used, rewrite it
    int aPos = Search(theParam.h, myParameters);
    if (aPos >= 0 && aPos < (int)myParameters.size()) {
      if (IsNotEqual(myParameters[aPos], theParam))
        myUpdatedParameters.insert(theParam.h);
      myParameters[aPos] = theParam;
      return theParam.h;
    }
  }

  // Parameter is not found, add new one
  Slvs_Param aParam = theParam;
  aParam.h = 0;
  return addParameter(aParam);
}

bool SketchSolver_Storage::removeParameter(const Slvs_hParam& theParamID)
{
  int aPos = Search(theParamID, myParameters);
  if (aPos >= 0 && aPos < (int)myParameters.size()) {
    // Firstly, search the parametes is not used elsewhere
    std::vector<Slvs_Entity>::const_iterator anEntIter = myEntities.begin();
    for (; anEntIter != myEntities.end(); anEntIter++) {
      for (int i = 0; i < 4; i++)
        if (anEntIter->param[i] == theParamID)
          return false;
    }
    // Remove parameter
    myParameters.erase(myParameters.begin() + aPos);
    myParamMaxID = myParameters.empty() ? SLVS_E_UNKNOWN : myParameters.back().h;
    myNeedToResolve = true;
    myRemovedParameters.insert(theParamID);
    return true;
  }
  return false;
}

const Slvs_Param& SketchSolver_Storage::getParameter(const Slvs_hParam& theParamID) const
{
  int aPos = Search(theParamID, myParameters);
  if (aPos >= 0 && aPos < (int)myParameters.size())
    return myParameters[aPos];

  // Parameter is not found, return empty object
  static Slvs_Param aDummy;
  aDummy.h = 0;
  return aDummy;
}


Slvs_hEntity SketchSolver_Storage::addEntity(const Slvs_Entity& theEntity)
{
  if (theEntity.h > 0 && theEntity.h <= myEntityMaxID) {
    // Entity is already used, rewrite it
    return updateEntity(theEntity);
  }

  Slvs_Entity aEntity = theEntity;
  if (aEntity.h > myEntityMaxID)
    myEntityMaxID = aEntity.h;
  else
    aEntity.h = ++myEntityMaxID;
  myEntities.push_back(aEntity);
  myNeedToResolve = true;
  return aEntity.h;
}

Slvs_hEntity SketchSolver_Storage::updateEntity(const Slvs_Entity& theEntity)
{
  if (theEntity.h > 0 && theEntity.h <= myEntityMaxID) {
    // Entity already used, rewrite it
    int aPos = Search(theEntity.h, myEntities);
    if (aPos >= 0 && aPos < (int)myEntities.size()) {
      myNeedToResolve = myNeedToResolve || IsNotEqual(myEntities[aPos], theEntity);
      myEntities[aPos] = theEntity;
      return theEntity.h;
    }
  }

  // Entity is not found, add new one
  Slvs_Entity aEntity = theEntity;
  aEntity.h = 0;
  return addEntity(aEntity);
}

bool SketchSolver_Storage::removeEntity(const Slvs_hEntity& theEntityID)
{
  bool aResult = true;
  int aPos = Search(theEntityID, myEntities);
  if (aPos >= 0 && aPos < (int)myEntities.size()) {
    // Firstly, check the entity and its attributes is not used elsewhere
    std::set<Slvs_hEntity> anEntAndSubs;
    anEntAndSubs.insert(theEntityID);
    for (int i = 0; i < 4; i++)
      if (myEntities[aPos].point[i] != SLVS_E_UNKNOWN)
        anEntAndSubs.insert(myEntities[aPos].point[i]);

    std::vector<Slvs_Entity>::const_iterator anEntIter = myEntities.begin();
    for (; anEntIter != myEntities.end(); anEntIter++) {
      for (int i = 0; i < 4; i++)
        if (anEntAndSubs.find(anEntIter->point[i]) != anEntAndSubs.end())
          return false;
      if (anEntAndSubs.find(anEntIter->distance) != anEntAndSubs.end())
        return false;
    }
    std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
    for (; aConstrIter != myConstraints.end(); aConstrIter++) {
      Slvs_hEntity anEntIDs[6] = {aConstrIter->ptA, aConstrIter->ptB,
          aConstrIter->entityA, aConstrIter->entityB,
          aConstrIter->entityC, aConstrIter->entityD};
      for (int i = 0; i < 6; i++)
        if (anEntAndSubs.find(anEntIDs[i]) != anEntAndSubs.end())
          return false;
    }
    // The entity is not used, remove it and its parameters
    Slvs_Entity anEntity = myEntities[aPos];
    myEntities.erase(myEntities.begin() + aPos);
    myEntityMaxID = myEntities.empty() ? SLVS_E_UNKNOWN : myEntities.back().h;
    if (anEntity.distance != SLVS_E_UNKNOWN)
      aResult = aResult && removeParameter(anEntity.distance);
    for (int i = 0; i < 4; i++)
      if (anEntity.param[i] != SLVS_E_UNKNOWN)
        aResult = removeParameter(anEntity.param[i]) && aResult;
    for (int i = 0; i < 4; i++)
      if (anEntity.point[i] != SLVS_E_UNKNOWN)
        aResult = removeEntity(anEntity.point[i]) && aResult;
    myNeedToResolve = true;
    myRemovedEntities.insert(theEntityID);
    if (anEntity.type == SLVS_E_POINT_IN_2D || anEntity.type == SLVS_E_POINT_IN_3D)
      removeCoincidentPoint(theEntityID);
  }
  return aResult;
}

void SketchSolver_Storage::removeUnusedEntities()
{
  std::set<Slvs_hEntity> anUnusedEntities;
  std::vector<Slvs_Entity>::const_iterator aEIt = myEntities.begin();
  for (; aEIt != myEntities.end(); ++aEIt) {
    if (aEIt->h == aEIt->wrkpl) {
      // don't remove workplane
      anUnusedEntities.erase(aEIt->point[0]);
      anUnusedEntities.erase(aEIt->normal);
      continue;
    }
    anUnusedEntities.insert(aEIt->h);
  }

  std::vector<Slvs_Constraint>::const_iterator aCIt = myConstraints.begin();
  for (; aCIt != myConstraints.end(); ++aCIt) {
    Slvs_hEntity aSubs[6] = {
        aCIt->entityA, aCIt->entityB,
        aCIt->entityC, aCIt->entityD,
        aCIt->ptA,     aCIt->ptB};
    for (int i = 0; i < 6; i++) {
      if (aSubs[i] != SLVS_E_UNKNOWN) {
        anUnusedEntities.erase(aSubs[i]);
        int aPos = Search(aSubs[i], myEntities);
        if (aPos >= 0 && aPos < (int)myEntities.size()) {
          for (int j = 0; j < 4; j++)
            if (myEntities[aPos].point[j] != SLVS_E_UNKNOWN)
              anUnusedEntities.erase(myEntities[aPos].point[j]);
          if (myEntities[aPos].distance != SLVS_E_UNKNOWN)
            anUnusedEntities.erase(myEntities[aPos].distance);
        }
      }
    }
  }

  std::set<Slvs_hEntity>::const_iterator anEntIt = anUnusedEntities.begin();
  while (anEntIt != anUnusedEntities.end()) {
    int aPos = Search(*anEntIt, myEntities);
    if (aPos < 0 && aPos >= (int)myEntities.size())
      continue;
    Slvs_Entity anEntity = myEntities[aPos];
    // Remove entity if and only if all its parameters unused
    bool isUsed = false;
    if (anEntity.distance != SLVS_E_UNKNOWN && 
      anUnusedEntities.find(anEntity.distance) == anUnusedEntities.end())
      isUsed = true;
    for (int i = 0; i < 4 && !isUsed; i++)
      if (anEntity.point[i] != SLVS_E_UNKNOWN &&
          anUnusedEntities.find(anEntity.point[i]) == anUnusedEntities.end())
        isUsed = true;
    if (isUsed) {
      anUnusedEntities.erase(anEntity.distance);
      for (int i = 0; i < 4; i++)
        if (anEntity.point[i] != SLVS_E_UNKNOWN)
          anUnusedEntities.erase(anEntity.point[i]);
      std::set<Slvs_hEntity>::iterator aRemoveIt = anEntIt++;
      anUnusedEntities.erase(aRemoveIt);
      continue;
    }
    ++anEntIt;
  }

  for (anEntIt = anUnusedEntities.begin(); anEntIt != anUnusedEntities.end(); ++anEntIt) {
    int aPos = Search(*anEntIt, myEntities);
    if (aPos >= 0 && aPos < (int)myEntities.size()) {
      // Remove entity and its parameters
      Slvs_Entity anEntity = myEntities[aPos];
      myEntities.erase(myEntities.begin() + aPos);
      myEntityMaxID = myEntities.empty() ? SLVS_E_UNKNOWN : myEntities.back().h;
      if (anEntity.distance != SLVS_E_UNKNOWN)
        removeParameter(anEntity.distance);
      for (int i = 0; i < 4; i++)
        if (anEntity.param[i] != SLVS_E_UNKNOWN)
          removeParameter(anEntity.param[i]);
      for (int i = 0; i < 4; i++)
        if (anEntity.point[i] != SLVS_E_UNKNOWN)
          removeEntity(anEntity.point[i]);
      myRemovedEntities.insert(*anEntIt);
      if (anEntity.type == SLVS_E_POINT_IN_2D || anEntity.type == SLVS_E_POINT_IN_3D)
        removeCoincidentPoint(*anEntIt);
    }
  }

  if (!anUnusedEntities.empty())
    myNeedToResolve = true;
}

bool SketchSolver_Storage::isUsedByConstraints(const Slvs_hEntity& theEntityID) const
{
  std::vector<Slvs_Constraint>::const_iterator aCIt = myConstraints.begin();
  for (; aCIt != myConstraints.end(); ++aCIt) {
    Slvs_hEntity aSubs[6] = {
        aCIt->entityA, aCIt->entityB,
        aCIt->entityC, aCIt->entityD,
        aCIt->ptA,     aCIt->ptB};
    for (int i = 0; i < 6; i++)
      if (aSubs[i] != SLVS_E_UNKNOWN && aSubs[i] == theEntityID)
        return true;
  }
  return false;
}

const Slvs_Entity& SketchSolver_Storage::getEntity(const Slvs_hEntity& theEntityID) const
{
  int aPos = Search(theEntityID, myEntities);
  if (aPos >= 0 && aPos < (int)myEntities.size())
    return myEntities[aPos];

  // Entity is not found, return empty object
  static Slvs_Entity aDummy;
  aDummy.h = SLVS_E_UNKNOWN;
  return aDummy;
}

Slvs_hEntity SketchSolver_Storage::copyEntity(const Slvs_hEntity& theCopied)
{
  int aPos = Search(theCopied, myEntities);
  if (aPos < 0 || aPos >= (int)myEntities.size())
    return SLVS_E_UNKNOWN;

  Slvs_Entity aCopy = myEntities[aPos];
  aCopy.h = SLVS_E_UNKNOWN;
  int i = 0;
  while (aCopy.point[i] != SLVS_E_UNKNOWN) {
    aCopy.point[i] = copyEntity(aCopy.point[i]);
    i++;
  }
  if (aCopy.param[0] != SLVS_E_UNKNOWN) {
    aPos = Search(aCopy.param[0], myParameters);
    i = 0;
    while (aCopy.param[i] != SLVS_E_UNKNOWN) {
      Slvs_Param aNewParam = myParameters[aPos];
      aNewParam.h = SLVS_E_UNKNOWN;
      aCopy.param[i] = addParameter(aNewParam);
      i++;
      aPos++;
    }
  }
  return addEntity(aCopy);
}

void SketchSolver_Storage::copyEntity(const Slvs_hEntity& theFrom, const Slvs_hEntity& theTo)
{
  int aPosFrom = Search(theFrom, myEntities);
  int aPosTo = Search(theTo, myEntities);
  if (aPosFrom < 0 || aPosFrom >= (int)myEntities.size() || 
      aPosTo < 0 || aPosTo >= (int)myEntities.size())
    return;

  Slvs_Entity aEntFrom = myEntities[aPosFrom];
  Slvs_Entity aEntTo = myEntities[aPosTo];
  int i = 0;
  while (aEntFrom.point[i] != SLVS_E_UNKNOWN) {
    copyEntity(aEntFrom.point[i], aEntTo.point[i]);
    i++;
  }
  if (aEntFrom.param[0] != SLVS_E_UNKNOWN) {
    aPosFrom = Search(aEntFrom.param[0], myParameters);
    aPosTo = Search(aEntTo.param[0], myParameters);
    i = 0;
    while (aEntFrom.param[i] != SLVS_E_UNKNOWN) {
      myParameters[aPosTo++].val = myParameters[aPosFrom++].val;
      i++;
    }
  }
}


bool SketchSolver_Storage::isPointFixed(
    const Slvs_hEntity& thePointID, Slvs_hConstraint& theFixed, bool theAccurate) const
{
  // Search the set of coincident points
  std::set<Slvs_hEntity> aCoincident;
  aCoincident.insert(thePointID);
  std::vector< std::set<Slvs_hEntity> >::const_iterator aCPIter = myCoincidentPoints.begin();
  for (; aCPIter != myCoincidentPoints.end(); aCPIter++)
    if (aCPIter->find(thePointID) != aCPIter->end()) {
      aCoincident = *aCPIter;
      break;
    }

  // Search the Rigid constraint
  theFixed = SLVS_C_UNKNOWN;
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for (; aConstrIter != myConstraints.end(); aConstrIter++)
    if (aConstrIter->type == SLVS_C_WHERE_DRAGGED &&
        aCoincident.find(aConstrIter->ptA) != aCoincident.end()) {
      theFixed = aConstrIter->h;
      if (aConstrIter->ptA == thePointID)
        return true;
    }
  if (theFixed != SLVS_C_UNKNOWN)
    return true;

  if (theAccurate) {
    // Try to find the fixed entity which uses such point or its coincidence
    std::vector<Slvs_Entity>::const_iterator anEntIter = myEntities.begin();
    for (; anEntIter != myEntities.end(); anEntIter++) {
      for (int i = 0; i < 4; i++) {
        Slvs_hEntity aPt = anEntIter->point[i];
        if (aPt != SLVS_E_UNKNOWN &&
            (aPt == thePointID || aCoincident.find(aPt) != aCoincident.end())) {
          if (isEntityFixed(anEntIter->h, true))
            return true;
        }
      }
    }
  }
  return SLVS_E_UNKNOWN;
}

bool SketchSolver_Storage::isEntityFixed(const Slvs_hEntity& theEntityID, bool theAccurate) const
{
  int aPos = Search(theEntityID, myEntities);
  if (aPos < 0 || aPos >= (int)myEntities.size())
    return false;

  // Firstly, find how many points are under Rigid constraint
  int aNbFixed = 0;
  for (int i = 0; i < 4; i++) {
    Slvs_hEntity aPoint = myEntities[aPos].point[i];
    if (aPoint == SLVS_E_UNKNOWN)
      continue;

    std::set<Slvs_hEntity> aCoincident;
    aCoincident.insert(aPoint);
    std::vector< std::set<Slvs_hEntity> >::const_iterator aCPIter = myCoincidentPoints.begin();
    for (; aCPIter != myCoincidentPoints.end(); aCPIter++)
      if (aCPIter->find(aPoint) != aCPIter->end()) {
        aCoincident = *aCPIter;
        break;
      }

    // Search the Rigid constraint
    std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
    for (; aConstrIter != myConstraints.end(); aConstrIter++)
      if (aConstrIter->type == SLVS_C_WHERE_DRAGGED &&
          aCoincident.find(aConstrIter->ptA) != aCoincident.end())
        aNbFixed++;
  }

  std::list<Slvs_Constraint> aList;
  std::list<Slvs_Constraint>::iterator anIt;
  Slvs_hConstraint aTempID; // used in isPointFixed() method

  if (myEntities[aPos].type == SLVS_E_LINE_SEGMENT) {
    if (aNbFixed == 2)
      return true;
    else if (aNbFixed == 0 || !theAccurate)
      return false;
    // Additional check (the line may be fixed if it is used by different constraints):
    // 1. The line is used in Equal constraint, another entity is fixed and there is a fixed point on line
    aList = getConstraintsByType(SLVS_C_PT_ON_LINE);
    for (anIt = aList.begin(); anIt != aList.end(); anIt++)
      if (anIt->entityA == theEntityID && isPointFixed(anIt->ptA, aTempID))
        break;
    if (anIt != aList.end()) {
      aList = getConstraintsByType(SLVS_C_EQUAL_LENGTH_LINES);
      aList.splice(aList.end(), getConstraintsByType(SLVS_C_EQUAL_LINE_ARC_LEN));
      for (anIt = aList.begin(); anIt != aList.end(); anIt++)
        if (anIt->entityA == theEntityID || anIt->entityB == theEntityID) {
          Slvs_hEntity anOther = anIt->entityA == theEntityID ? anIt->entityB : anIt->entityA;
          if (isEntityFixed(anOther, false))
            return true;
        }
    }
    // 2. The line is used in Parallel/Perpendicular/Vertical/Horizontal and Length constraints
    aList = getConstraintsByType(SLVS_C_PARALLEL);
    aList.splice(aList.end(), getConstraintsByType(SLVS_C_PERPENDICULAR));
    aList.splice(aList.end(), getConstraintsByType(SLVS_C_VERTICAL));
    aList.splice(aList.end(), getConstraintsByType(SLVS_C_HORIZONTAL));
    for (anIt = aList.begin(); anIt != aList.end(); anIt++)
      if (anIt->entityA == theEntityID || anIt->entityB == theEntityID) {
        Slvs_hEntity anOther = anIt->entityA == theEntityID ? anIt->entityB : anIt->entityA;
        if (isEntityFixed(anOther, false))
          break;
      }
    if (anIt != aList.end()) {
      aList = getConstraintsByType(SLVS_C_PT_PT_DISTANCE);
      for (anIt = aList.begin(); anIt != aList.end(); anIt++)
        if ((anIt->ptA == myEntities[aPos].point[0] && anIt->ptB == myEntities[aPos].point[1]) ||
            (anIt->ptA == myEntities[aPos].point[1] && anIt->ptB == myEntities[aPos].point[0]))
          return true;
    }
    // 3. Another verifiers ...
  } else if (myEntities[aPos].type == SLVS_E_CIRCLE) {
    if (aNbFixed == 0)
      return false;
    // Search for Diameter constraint
    aList = getConstraintsByType(SLVS_C_DIAMETER);
    for (anIt = aList.begin(); anIt != aList.end(); anIt++)
      if (anIt->entityA == theEntityID)
        return true;
    if (!theAccurate)
      return false;
    // Additional check (the circle may be fixed if it is used by different constraints):
    // 1. The circle is used in Equal constraint and another entity is fixed
    aList = getConstraintsByType(SLVS_C_EQUAL_RADIUS);
    for (anIt = aList.begin(); anIt != aList.end(); anIt++)
      if (anIt->entityA == theEntityID || anIt->entityB == theEntityID) {
        Slvs_hEntity anOther = anIt->entityA == theEntityID ? anIt->entityB : anIt->entityA;
        if (isEntityFixed(anOther, false))
          return true;
      }
    // 2. Another verifiers ...
  } else if (myEntities[aPos].type == SLVS_E_ARC_OF_CIRCLE) {
    if (aNbFixed > 2)
      return true;
    else if (aNbFixed <= 1)
      return false;
    // Search for Diameter constraint
    aList = getConstraintsByType(SLVS_C_DIAMETER);
    for (anIt = aList.begin(); anIt != aList.end(); anIt++)
      if (anIt->entityA == theEntityID)
        return true;
    if (!theAccurate)
      return false;
    // Additional check (the arc may be fixed if it is used by different constraints):
    // 1. The arc is used in Equal constraint and another entity is fixed
    aList = getConstraintsByType(SLVS_C_EQUAL_RADIUS);
    aList.splice(aList.end(), getConstraintsByType(SLVS_C_EQUAL_LINE_ARC_LEN));
    for (anIt = aList.begin(); anIt != aList.end(); anIt++)
      if (anIt->entityA == theEntityID || anIt->entityB == theEntityID) {
        Slvs_hEntity anOther = anIt->entityA == theEntityID ? anIt->entityB : anIt->entityA;
        if (isEntityFixed(anOther, false))
          return true;
      }
    // 2. Another verifiers ...
  }
  return false;
}


Slvs_hConstraint SketchSolver_Storage::addConstraint(const Slvs_Constraint& theConstraint)
{
  if (theConstraint.h > 0 && theConstraint.h <= myConstrMaxID) {
    // Constraint is already used, rewrite it
    return updateConstraint(theConstraint);
  }

  Slvs_Constraint aConstraint = theConstraint;

  // Find a constraint with same type uses same arguments to show user overconstraint situation
  std::vector<Slvs_Constraint>::iterator aCIt = myConstraints.begin();
  for (; aCIt != myConstraints.end(); aCIt++) {
    if (aConstraint.type != aCIt->type)
      continue;
    if (aConstraint.ptA == aCIt->ptA && aConstraint.ptB == aCIt->ptB &&
        aConstraint.entityA == aCIt->entityA && aConstraint.entityB == aCIt->entityB &&
        aConstraint.entityC == aCIt->entityC && aConstraint.entityD == aCIt->entityD)
      myDuplicatedConstraint = true;
  }

  if (aConstraint.h > myConstrMaxID)
    myConstrMaxID = aConstraint.h;
  else
    aConstraint.h = ++myConstrMaxID;
  myConstraints.push_back(aConstraint);
  myNeedToResolve = true;
  if (aConstraint.type == SLVS_C_POINTS_COINCIDENT)
    addCoincidentPoints(aConstraint.ptA, aConstraint.ptB);
  return aConstraint.h;
}

Slvs_hConstraint SketchSolver_Storage::updateConstraint(const Slvs_Constraint& theConstraint)
{
  if (theConstraint.h > 0 && theConstraint.h <= myConstrMaxID) {
    // Constraint already used, rewrite it
    int aPos = Search(theConstraint.h, myConstraints);
    if (aPos >= 0 && aPos < (int)myConstraints.size()) {
      myNeedToResolve = myNeedToResolve || IsNotEqual(myConstraints[aPos], theConstraint);
      myConstraints[aPos] = theConstraint;
      if (theConstraint.type == SLVS_C_POINTS_COINCIDENT)
        addCoincidentPoints(theConstraint.ptA, theConstraint.ptB);
      return theConstraint.h;
    }
  }

  // Constraint is not found, add new one
  Slvs_Constraint aConstraint = theConstraint;
  aConstraint.h = 0;
  return addConstraint(aConstraint);
}

bool SketchSolver_Storage::removeConstraint(const Slvs_hConstraint& theConstraintID)
{
  bool aResult = true;
  int aPos = Search(theConstraintID, myConstraints);
  if (aPos >= 0 && aPos < (int)myConstraints.size()) {
    Slvs_Constraint aConstraint = myConstraints[aPos];
    myConstraints.erase(myConstraints.begin() + aPos);
    myConstrMaxID = myConstraints.empty() ? SLVS_E_UNKNOWN : myConstraints.back().h;
    myNeedToResolve = true;
    myRemovedConstraints.insert(theConstraintID);
    if (aConstraint.type == SLVS_C_POINTS_COINCIDENT)
      removeCoincidence(aConstraint);

    // Remove all entities
    Slvs_hEntity anEntities[6] = {aConstraint.ptA, aConstraint.ptB,
        aConstraint.entityA, aConstraint.entityB,
        aConstraint.entityC, aConstraint.entityD};
    for (int i = 0; i < 6; i++)
      if (anEntities[i] != SLVS_E_UNKNOWN)
        aResult = removeEntity(anEntities[i]) && aResult;
    // remove temporary fixed point, if available
    if (myFixed == theConstraintID)
      myFixed = SLVS_E_UNKNOWN;
    if (myDuplicatedConstraint) {
      // Check the duplicated constraints are still available
      myDuplicatedConstraint = false;
      std::vector<Slvs_Constraint>::const_iterator anIt1 = myConstraints.begin();
      std::vector<Slvs_Constraint>::const_iterator anIt2 = myConstraints.begin();
      for (; anIt1 != myConstraints.end() && !myDuplicatedConstraint; anIt1++)
        for (anIt2 = anIt1+1; anIt2 != myConstraints.end() && !myDuplicatedConstraint; anIt2++) {
          if (anIt1->type != anIt2->type)
            continue;
          if (anIt1->ptA == anIt2->ptA && anIt1->ptB == anIt2->ptB &&
              anIt1->entityA == anIt2->entityA && anIt1->entityB == anIt2->entityB &&
              anIt1->entityC == anIt2->entityC && anIt1->entityD == anIt2->entityD)
            myDuplicatedConstraint = true;
        }
    }
  }
  return aResult;
}

const Slvs_Constraint& SketchSolver_Storage::getConstraint(const Slvs_hConstraint& theConstraintID) const
{
  int aPos = Search(theConstraintID, myConstraints);
  if (aPos >= 0 && aPos < (int)myConstraints.size())
    return myConstraints[aPos];

  // Constraint is not found, return empty object
  static Slvs_Constraint aDummy;
  aDummy.h = 0;
  return aDummy;
}

std::list<Slvs_Constraint> SketchSolver_Storage::getConstraintsByType(int theConstraintType) const
{
  std::list<Slvs_Constraint> aResult;
  std::vector<Slvs_Constraint>::const_iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (aCIter->type == theConstraintType)
      aResult.push_back(*aCIter);
  return aResult;
}


void SketchSolver_Storage::addConstraintWhereDragged(const Slvs_hConstraint& theConstraintID)
{
  if (myFixed != SLVS_E_UNKNOWN)
    return; // the point is already fixed
  int aPos = Search(theConstraintID, myConstraints);
  if (aPos >= 0 && aPos < (int)myConstraints.size())
    myFixed = theConstraintID;
}

void SketchSolver_Storage::addTemporaryConstraint(const Slvs_hConstraint& theConstraintID)
{
  myTemporaryConstraints.insert(theConstraintID);
}

void SketchSolver_Storage::removeTemporaryConstraints()
{
  myTemporaryConstraints.clear();
}

int SketchSolver_Storage::deleteTemporaryConstraint()
{
  if (myTemporaryConstraints.empty())
    return 0;
  // Search the point-on-line or a non-rigid constraint
  std::set<Slvs_hConstraint>::iterator aCIt = myTemporaryConstraints.begin();
  for (; aCIt != myTemporaryConstraints.end(); aCIt++) {
    int aPos = Search(*aCIt, myConstraints);
    if (aPos >= (int)myConstraints.size() || myConstraints[aPos].type != SLVS_C_WHERE_DRAGGED)
      break;
    std::vector<Slvs_Constraint>::iterator anIt = myConstraints.begin();
    for (; anIt != myConstraints.end(); anIt++)
      if (anIt->type == SLVS_C_PT_ON_LINE && anIt->ptA == myConstraints[aPos].ptA)
        break;
    if (anIt != myConstraints.end())
      break;
  }
  if (aCIt == myTemporaryConstraints.end())
    aCIt = myTemporaryConstraints.begin();
  bool aNewFixed = (*aCIt == myFixed);
  removeConstraint(*aCIt);
  myTemporaryConstraints.erase(aCIt);
  if (aNewFixed) {
    for (aCIt = myTemporaryConstraints.begin(); aCIt != myTemporaryConstraints.end(); aCIt++) {
      int aPos = Search(*aCIt, myConstraints);
      if (myConstraints[aPos].type == SLVS_C_WHERE_DRAGGED) {
        myFixed = *aCIt;
        break;
      }
    }
  }
  return (int)myTemporaryConstraints.size();
}

bool SketchSolver_Storage::isTemporary(const Slvs_hConstraint& theConstraintID) const
{
  return myTemporaryConstraints.find(theConstraintID) != myTemporaryConstraints.end();
}


void SketchSolver_Storage::getRemoved(
    std::set<Slvs_hParam>& theParameters,
    std::set<Slvs_hEntity>& theEntities,
    std::set<Slvs_hConstraint>& theConstraints)
{
  theParameters = myRemovedParameters;
  theEntities = myRemovedEntities;
  theConstraints = myRemovedConstraints;

  myRemovedParameters.clear();
  myRemovedEntities.clear();
  myRemovedConstraints.clear();
}

void SketchSolver_Storage::initializeSolver(SketchSolver_Solver& theSolver)
{
  theSolver.setParameters(myParameters.data(), (int)myParameters.size());
  theSolver.setEntities(myEntities.data(), (int)myEntities.size());

  // Copy constraints excluding the fixed one
  std::vector<Slvs_Constraint> aConstraints = myConstraints;
  if (myFixed != SLVS_E_UNKNOWN) {
    Slvs_hEntity aFixedPoint = SLVS_E_UNKNOWN;
    std::vector<Slvs_Constraint>::iterator anIt = aConstraints.begin();
    for (; anIt != aConstraints.end(); anIt++)
      if (anIt->h == myFixed) {
        aFixedPoint = anIt->ptA;
//        aConstraints.erase(anIt);
        break;
      }
    // set dragged parameters
    int aPos = Search(aFixedPoint, myEntities);
    theSolver.setDraggedParameters(myEntities[aPos].param);
  }
  theSolver.setConstraints(aConstraints.data(), (int)aConstraints.size());
}

void SketchSolver_Storage::addCoincidentPoints(
    const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2)
{
  std::vector< std::set<Slvs_hEntity> >::iterator aCIter = myCoincidentPoints.begin();
  std::vector< std::set<Slvs_hEntity> >::iterator aFoundIter = myCoincidentPoints.end(); // already found coincidence
  bool isFound = false;
  for (; aCIter != myCoincidentPoints.end(); aCIter++) {
    bool isFirstFound = aCIter->find(thePoint1) != aCIter->end();
    bool isSecondFound = aCIter->find(thePoint2) != aCIter->end();
    isFound = isFound || isFirstFound || isSecondFound;
    if (isFirstFound && isSecondFound)
      break; // already coincident
    else if (isFirstFound || isSecondFound) {
      if (aFoundIter != myCoincidentPoints.end()) {
        // merge two sets
        aFoundIter->insert(aCIter->begin(), aCIter->end());
        myCoincidentPoints.erase(aCIter);
        break;
      } else
        aFoundIter = aCIter;
      aCIter->insert(thePoint1);
      aCIter->insert(thePoint2);
    }
  }
  // coincident points not found
  if (!isFound) {
    std::set<Slvs_hEntity> aNewSet;
    aNewSet.insert(thePoint1);
    aNewSet.insert(thePoint2);
    myCoincidentPoints.push_back(aNewSet);
  }
}

void SketchSolver_Storage::removeCoincidentPoint(const Slvs_hEntity& thePoint)
{
  std::vector< std::set<Slvs_hEntity> >::iterator aCIter = myCoincidentPoints.begin();
  for (; aCIter != myCoincidentPoints.end(); aCIter++)
    if (aCIter->find(thePoint) != aCIter->end()) {
      aCIter->erase(thePoint);
      if (aCIter->size() <= 1)
        myCoincidentPoints.erase(aCIter);
      break;
    }
}

void SketchSolver_Storage::removeCoincidence(const Slvs_Constraint& theCoincidence)
{
  // Find set of coincident points
  std::vector< std::set<Slvs_hEntity> >::iterator aCIt = myCoincidentPoints.begin();
  for (; aCIt != myCoincidentPoints.end(); ++aCIt)
    if (aCIt->find(theCoincidence.ptA) != aCIt->end() ||
        aCIt->find(theCoincidence.ptB) != aCIt->end())
      break;
  if (aCIt == myCoincidentPoints.end())
    return;

  // Leave only the points which are still coincident
  std::set<Slvs_hEntity> aRemainCoincidence;
  std::vector<Slvs_Constraint>::const_iterator aConstrIt = myConstraints.begin();
  for (; aConstrIt != myConstraints.end(); ++aConstrIt) {
    if (aConstrIt->type != SLVS_C_POINTS_COINCIDENT)
      continue;
    if (aCIt->find(aConstrIt->ptA) != aCIt->end() ||
        aCIt->find(aConstrIt->ptB) != aCIt->end()) {
      aRemainCoincidence.insert(aConstrIt->ptA);
      aRemainCoincidence.insert(aConstrIt->ptB);
    }
  }
  if (aRemainCoincidence.size() <= 1)
    myCoincidentPoints.erase(aCIt);
  else
    aCIt->swap(aRemainCoincidence);
}

bool SketchSolver_Storage::isCoincident(
    const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2) const
{
  std::vector< std::set<Slvs_hEntity> >::const_iterator aCIter = myCoincidentPoints.begin();
  for (; aCIter != myCoincidentPoints.end(); aCIter++)
    if (aCIter->find(thePoint1) != aCIter->end() && aCIter->find(thePoint2) != aCIter->end())
      return true;
  return false;
}

bool SketchSolver_Storage::isEqual(
    const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2) const
{
  if (isCoincident(thePoint1, thePoint2))
    return true;

  // Precise checking of coincidence: verify that points have equal coordinates
  int aEnt1Pos = Search(thePoint1, myEntities);
  int aEnt2Pos = Search(thePoint2, myEntities);
  if (aEnt1Pos >= 0 && aEnt1Pos < (int)myEntities.size() &&
      aEnt2Pos >= 0 && aEnt2Pos < (int)myEntities.size()) {
    double aDist[2];
    int aParamPos;
    for (int i = 0; i < 2; i++) {
      aParamPos = Search(myEntities[aEnt1Pos].param[i], myParameters);
      aDist[i] = myParameters[aParamPos].val;
      aParamPos = Search(myEntities[aEnt2Pos].param[i], myParameters);
      aDist[i] -= myParameters[aParamPos].val;
    }
    if (aDist[0] * aDist[0] + aDist[1] * aDist[1] < tolerance * tolerance)
      return true;
  }
  return false;
}


std::vector<Slvs_hConstraint> SketchSolver_Storage::fixEntity(const Slvs_hEntity& theEntity)
{
  std::vector<Slvs_hConstraint> aNewConstraints;

  int aPos = Search(theEntity, myEntities);
  if (aPos >= 0 && aPos < (int)myEntities.size()) {
    switch (myEntities[aPos].type) {
    case SLVS_E_POINT_IN_2D:
    case SLVS_E_POINT_IN_3D:
      fixPoint(myEntities[aPos], aNewConstraints);
      break;
    case SLVS_E_LINE_SEGMENT:
      fixLine(myEntities[aPos], aNewConstraints);
      break;
    case SLVS_E_CIRCLE:
      fixCircle(myEntities[aPos], aNewConstraints);
      break;
    case SLVS_E_ARC_OF_CIRCLE:
      fixArc(myEntities[aPos], aNewConstraints);
      break;
    default:
      break;
    }
  }

  return aNewConstraints;
}

void SketchSolver_Storage::fixPoint(const Slvs_Entity& thePoint,
    std::vector<Slvs_hConstraint>& theCreated)
{
  Slvs_Constraint aConstraint;
  Slvs_hConstraint aConstrID = SLVS_E_UNKNOWN;
  bool isFixed = isPointFixed(thePoint.h, aConstrID, true);
  bool isForceUpdate = (isFixed && isTemporary(aConstrID));
  if (!isForceUpdate) { // create new constraint
    if (isFixed) return;
    aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, thePoint.group, SLVS_C_WHERE_DRAGGED, thePoint.wrkpl,
        0.0, thePoint.h, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = addConstraint(aConstraint);
    theCreated.push_back(aConstraint.h);
  } else { // update already existent constraint
    if (!isFixed || aConstrID == SLVS_E_UNKNOWN)
      return;
    int aPos = Search(aConstrID, myConstraints);
    if (aPos >= 0 && aPos < (int)myConstraints.size())
      myConstraints[aPos].ptA = thePoint.h;
  }
}

void SketchSolver_Storage::fixLine(const Slvs_Entity& theLine,
    std::vector<Slvs_hConstraint>& theCreated)
{
  Slvs_Entity aPoint[2] = {
      getEntity(theLine.point[0]),
      getEntity(theLine.point[1])
  };

  Slvs_Constraint anEqual;
  if (isAxisParallel(theLine.h)) {
    // Fix one point and a line length
    Slvs_hConstraint aFixed;
    if (!isPointFixed(theLine.point[0], aFixed, true) &&
        !isPointFixed(theLine.point[1], aFixed, true))
      fixPoint(aPoint[0], theCreated);
    if (!isUsedInEqual(theLine.h, anEqual)) {
      // Check the distance is not set yet
      std::vector<Slvs_Constraint>::const_iterator aDistIt = myConstraints.begin();
      for (; aDistIt != myConstraints.end(); ++aDistIt)
        if ((aDistIt->type == SLVS_C_PT_PT_DISTANCE) &&
           ((aDistIt->ptA == theLine.point[0] && aDistIt->ptB == theLine.point[1]) ||
            (aDistIt->ptA == theLine.point[1] && aDistIt->ptB == theLine.point[0])))
          return;
      // Calculate distance between points on the line
      double aCoords[4];
      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
          Slvs_Param aParam = getParameter(aPoint[i].param[j]);
          aCoords[2*i+j] = aParam.val;
        }

      double aLength = sqrt((aCoords[2] - aCoords[0]) * (aCoords[2] - aCoords[0]) + 
                            (aCoords[3] - aCoords[1]) * (aCoords[3] - aCoords[1]));
      // fix line length
      Slvs_Constraint aDistance = Slvs_MakeConstraint(SLVS_E_UNKNOWN, theLine.group,
          SLVS_C_PT_PT_DISTANCE, theLine.wrkpl, aLength,
          theLine.point[0], theLine.point[1], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      aDistance.h = addConstraint(aDistance);
      theCreated.push_back(aDistance.h);
    }
    return;
  }
  else if (isUsedInEqual(theLine.h, anEqual)) {
    // Check another entity of Equal is already fixed
    Slvs_hEntity anOtherEntID = anEqual.entityA == theLine.h ? anEqual.entityB : anEqual.entityA;
    if (isEntityFixed(anOtherEntID, true)) {
      // Fix start point of the line (if end point is not fixed yet) ...
      Slvs_hConstraint anEndFixedID = SLVS_E_UNKNOWN;
      bool isFixed = isPointFixed(theLine.point[1], anEndFixedID, true);
      if (isFixed == SLVS_E_UNKNOWN)
        fixPoint(aPoint[0], theCreated);
      // ...  and create fixed point lying on this line
      Slvs_hEntity aPointToCopy = anEndFixedID == SLVS_E_UNKNOWN ? theLine.point[1] : theLine.point[0];
      // Firstly, search already fixed point on line
      bool isPonLineFixed = false;
      Slvs_hEntity aFixedPoint;
      std::vector<Slvs_Constraint>::const_iterator aPLIter = myConstraints.begin();
      for (; aPLIter != myConstraints.end() && !isPonLineFixed; ++aPLIter)
        if (aPLIter->type == SLVS_C_PT_ON_LINE && aPLIter->entityA == theLine.h) {
          isPonLineFixed = isPointFixed(aPLIter->ptA, anEndFixedID);
          aFixedPoint = aPLIter->ptA;
        }

      if (isPonLineFixed) { // update existent constraint
        copyEntity(aPointToCopy, aFixedPoint);
      } else { // create new constraint
        Slvs_hEntity aCopied = copyEntity(aPointToCopy);
        Slvs_Constraint aPonLine = Slvs_MakeConstraint(SLVS_E_UNKNOWN, theLine.group, SLVS_C_PT_ON_LINE,
            theLine.wrkpl, 0.0, aCopied, SLVS_E_UNKNOWN, theLine.h, SLVS_E_UNKNOWN);
        aPonLine.h = addConstraint(aPonLine);
        theCreated.push_back(aPonLine.h);
        fixPoint(getEntity(aCopied), theCreated);
      }
      return;
    }
  }

  // Fix both points
  for (int i = 0; i < 2; i++)
    fixPoint(aPoint[i], theCreated);
}

void SketchSolver_Storage::fixCircle(const Slvs_Entity& theCircle,
    std::vector<Slvs_hConstraint>& theCreated)
{
  bool isFixRadius = true;
  // Verify the arc is under Equal constraint
  Slvs_Constraint anEqual;
  if (isUsedInEqual(theCircle.h, anEqual)) {
    // Check another entity of Equal is already fixed
    Slvs_hEntity anOtherEntID = anEqual.entityA == theCircle.h ? anEqual.entityB : anEqual.entityA;
    if (isEntityFixed(anOtherEntID, true))
      isFixRadius = false;
  }

  fixPoint(getEntity(theCircle.point[0]), theCreated);

  if (isFixRadius) {
    // Search the radius is already fixed
    std::vector<Slvs_Constraint>::const_iterator aDiamIter = myConstraints.begin();
    for (; aDiamIter != myConstraints.end(); ++aDiamIter)
      if (aDiamIter->type == SLVS_C_DIAMETER && aDiamIter->entityA == theCircle.h)
        return;

    // Fix radius of a circle
    const Slvs_Entity& aRadEnt = getEntity(theCircle.distance);
    double aRadius = getParameter(aRadEnt.param[0]).val;
    Slvs_Constraint aFixedR = Slvs_MakeConstraint(SLVS_E_UNKNOWN, theCircle.group, SLVS_C_DIAMETER,
        theCircle.wrkpl, aRadius * 2.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, theCircle.h, SLVS_E_UNKNOWN);
    aFixedR.h = addConstraint(aFixedR);
    theCreated.push_back(aFixedR.h);
  }
}

void SketchSolver_Storage::fixArc(const Slvs_Entity& theArc,
    std::vector<Slvs_hConstraint>& theCreated)
{
  Slvs_Entity aPoint[3] = {
      getEntity(theArc.point[0]),
      getEntity(theArc.point[1]),
      getEntity(theArc.point[2])
  };

  bool isFixRadius = true;
  std::list<Slvs_Entity> aPointsToFix;
  aPointsToFix.push_back(aPoint[1]);
  aPointsToFix.push_back(aPoint[2]);

  // Verify the arc is under Equal constraint
  Slvs_Constraint anEqual;
  if (isUsedInEqual(theArc.h, anEqual)) {
    // Check another entity of Equal is already fixed
    Slvs_hEntity anOtherEntID = anEqual.entityA == theArc.h ? anEqual.entityB : anEqual.entityA;
    if (isEntityFixed(anOtherEntID, true)) {
      isFixRadius = false;
      Slvs_Entity anOtherEntity = getEntity(anOtherEntID);
      if (anOtherEntity.type == SLVS_E_LINE_SEGMENT) {
        aPointsToFix.pop_back();
        aPointsToFix.push_back(aPoint[0]);
      }
    }
  }

  Slvs_hConstraint aConstrID;
  int aNbPointsToFix = 2; // number of fixed points for the arc
  if (isPointFixed(theArc.point[0], aConstrID, true))
    aNbPointsToFix--;

  double anArcPoints[3][2];
  for (int i = 0; i < 3; i++) {
    const Slvs_Entity& aPointOnArc = getEntity(theArc.point[i]);
    for (int j = 0; j < 2; j++)
      anArcPoints[i][j] = getParameter(aPointOnArc.param[j]).val;
  }

  // Radius of the arc
  std::shared_ptr<GeomAPI_Pnt2d> aCenter(new GeomAPI_Pnt2d(anArcPoints[0][0], anArcPoints[0][1]));
  std::shared_ptr<GeomAPI_Pnt2d> aStart(new GeomAPI_Pnt2d(anArcPoints[1][0], anArcPoints[1][1]));
  double aRadius = aCenter->distance(aStart);

  // Update end point of the arc to be on a curve
  std::shared_ptr<GeomAPI_Pnt2d> anEnd(new GeomAPI_Pnt2d(anArcPoints[2][0], anArcPoints[2][1]));
  double aDistance = anEnd->distance(aCenter);
  std::shared_ptr<GeomAPI_XY> aDir = anEnd->xy()->decreased(aCenter->xy());
  if (aDistance < tolerance)
    aDir = aStart->xy()->decreased(aCenter->xy())->multiplied(-1.0);
  else
    aDir = aDir->multiplied(aRadius / aDistance);
  double xy[2] = {aCenter->x() + aDir->x(), aCenter->y() + aDir->y()};
  const Slvs_Entity& aEndPoint = getEntity(theArc.point[2]);
  for (int i = 0; i < 2; i++) {
    Slvs_Param aParam = getParameter(aEndPoint.param[i]);
    aParam.val = xy[i];
    updateParameter(aParam);
  }

  std::list<Slvs_Entity>::iterator aPtIt = aPointsToFix.begin();
  for (; aNbPointsToFix > 0; aPtIt++, aNbPointsToFix--)
    fixPoint(*aPtIt, theCreated);

  if (isFixRadius) {
    // Fix radius of the arc
    bool isExists = false;
    std::vector<Slvs_Constraint>::iterator anIt = myConstraints.begin();
    for (; anIt != myConstraints.end() && !isExists; ++anIt)
      if (anIt->type == SLVS_C_DIAMETER && anIt->entityA == theArc.h)
        isExists = true;
    if (!isExists) {
      Slvs_Constraint aFixedR = Slvs_MakeConstraint(SLVS_E_UNKNOWN, theArc.group, SLVS_C_DIAMETER,
          theArc.wrkpl, aRadius * 2.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, theArc.h, SLVS_E_UNKNOWN);
      aFixedR.h = addConstraint(aFixedR);
      theCreated.push_back(aFixedR.h);
    }
  }
}


bool SketchSolver_Storage::isAxisParallel(const Slvs_hEntity& theEntity) const
{
  std::vector<Slvs_Constraint>::const_iterator anIter = myConstraints.begin();
  for (; anIter != myConstraints.end(); anIter++)
    if ((anIter->type == SLVS_C_HORIZONTAL || anIter->type == SLVS_C_VERTICAL) && 
        anIter->entityA == theEntity)
      return true;
  return false;
}

bool SketchSolver_Storage::isUsedInEqual(
    const Slvs_hEntity& theEntity, Slvs_Constraint& theEqual) const
{
  // Check the entity is used in Equal constraint
  std::vector<Slvs_Constraint>::const_iterator anEqIter = myConstraints.begin();
  for (; anEqIter != myConstraints.end(); anEqIter++)
    if ((anEqIter->type == SLVS_C_EQUAL_LENGTH_LINES ||
         anEqIter->type == SLVS_C_EQUAL_LINE_ARC_LEN ||
         anEqIter->type == SLVS_C_EQUAL_RADIUS) &&
       (anEqIter->entityA == theEntity || anEqIter->entityB == theEntity)) {
      theEqual = *anEqIter;
      return true;
    }
  return false;
}

bool SketchSolver_Storage::isNeedToResolve()
{
  if (myConstraints.empty())
    return false;

  if (!myNeedToResolve) {
    // Verify the updated parameters are used in constraints
    std::set<Slvs_hEntity> aPoints;
    std::vector<Slvs_Entity>::const_iterator anEntIt = myEntities.begin();
    for (; anEntIt != myEntities.end(); ++anEntIt) {
      for (int i = 0; i < 4 && anEntIt->param[i] != SLVS_E_UNKNOWN; ++i)
        if (myUpdatedParameters.find(anEntIt->param[i]) != myUpdatedParameters.end()) {
          aPoints.insert(anEntIt->h);
          break;
        }
    }
    std::set<Slvs_hEntity> anEntities = aPoints;
    for (anEntIt = myEntities.begin(); anEntIt != myEntities.end(); ++anEntIt) {
      for (int i = 0; i < 4 && anEntIt->point[i] != SLVS_E_UNKNOWN; ++i)
        if (aPoints.find(anEntIt->point[i]) != aPoints.end()) {
          anEntities.insert(anEntIt->h);
          break;
        }
    }

    std::vector<Slvs_Constraint>::const_iterator aCIt = myConstraints.begin();
    for (; aCIt != myConstraints.end() && !myNeedToResolve; ++aCIt) {
      Slvs_hEntity anAttrs[6] =
        {aCIt->ptA, aCIt->ptB, aCIt->entityA, aCIt->entityB, aCIt->entityC, aCIt->entityD};
      for (int i = 0; i < 6; i++)
        if (anAttrs[i] != SLVS_E_UNKNOWN && anEntities.find(anAttrs[i]) != anEntities.end()) {
          myNeedToResolve = true;
          break;
        }
    }
  }

  myUpdatedParameters.clear();
  return myNeedToResolve;
}






// ========================================================
// =========      Auxiliary functions       ===============
// ========================================================

template<typename T>
int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities)
{
  int aResIndex = theEntityID <= theEntities.size() ? theEntityID - 1 : 0;
  int aVecSize = theEntities.size();
  if (theEntities.empty())
    return 1;
  while (aResIndex >= 0 && theEntities[aResIndex].h > theEntityID)
    aResIndex--;
  while (aResIndex < aVecSize && aResIndex >= 0 && theEntities[aResIndex].h < theEntityID)
    aResIndex++;
  if (aResIndex == -1 || (aResIndex < aVecSize && theEntities[aResIndex].h != theEntityID))
    aResIndex = aVecSize;
  return aResIndex;
}

bool IsNotEqual(const Slvs_Param& theParam1, const Slvs_Param& theParam2)
{
  return fabs(theParam1.val - theParam2.val) > tolerance;
}

bool IsNotEqual(const Slvs_Entity& theEntity1, const Slvs_Entity& theEntity2)
{
  int i = 0;
  for (; theEntity1.param[i] != 0 && i < 4; i++)
    if (theEntity1.param[i] != theEntity2.param[i])
      return true;
  i = 0;
  for (; theEntity1.point[i] != 0 && i < 4; i++)
    if (theEntity1.point[i] != theEntity2.point[i])
      return true;
  return false;
}

bool IsNotEqual(const Slvs_Constraint& theConstraint1, const Slvs_Constraint& theConstraint2)
{
  return theConstraint1.ptA != theConstraint2.ptA ||
         theConstraint1.ptB != theConstraint2.ptB ||
         theConstraint1.entityA != theConstraint2.entityA ||
         theConstraint1.entityB != theConstraint2.entityB ||
         theConstraint1.entityC != theConstraint2.entityC ||
         theConstraint1.entityD != theConstraint2.entityD ||
         fabs(theConstraint1.valA - theConstraint2.valA) > tolerance;
}
