// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Storage.cpp
// Created: 18 Mar 2015
// Author:  Artem ZHIDKOV

#include <SketchSolver_Storage.h>

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
    myNeedToResolve(false)
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
      myNeedToResolve = myNeedToResolve || IsNotEqual(myParameters[aPos], theParam);
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
    // Firstly, check the entity is not used elsewhere
    std::vector<Slvs_Entity>::const_iterator anEntIter = myEntities.begin();
    for (; anEntIter != myEntities.end(); anEntIter++) {
      for (int i = 0; i < 4; i++)
        if (anEntIter->point[i] == theEntityID)
          return false;
      if (anEntIter->distance == theEntityID)
        return false;
    }
    std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
    for (; aConstrIter != myConstraints.end(); aConstrIter++) {
      Slvs_hEntity anEntIDs[6] = {aConstrIter->ptA, aConstrIter->ptB,
          aConstrIter->entityA, aConstrIter->entityB,
          aConstrIter->entityC, aConstrIter->entityD};
      for (int i = 0; i < 6; i++)
        if (anEntIDs[i] == theEntityID)
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

const Slvs_Entity& SketchSolver_Storage::getEntity(const Slvs_hEntity& theEntityID) const
{
  int aPos = Search(theEntityID, myEntities);
  if (aPos >= 0 && aPos < (int)myEntities.size())
    return myEntities[aPos];

  // Entity is not found, return empty object
  static Slvs_Entity aDummy;
  aDummy.h = 0;
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


Slvs_hConstraint SketchSolver_Storage::isPointFixed(const Slvs_hEntity& thePointID) const
{
  // Search the set of coincident points
  std::vector< std::set<Slvs_hEntity> >::const_iterator aCPIter = myCoincidentPoints.begin();
  for (; aCPIter != myCoincidentPoints.end(); aCPIter++)
    if (aCPIter->find(thePointID) != aCPIter->end())
      break;
  if (aCPIter == myCoincidentPoints.end()) {
    std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
    for (; aConstrIter != myConstraints.end(); aConstrIter++)
      if (aConstrIter->type == SLVS_C_WHERE_DRAGGED &&
          aConstrIter->ptA == thePointID)
        return aConstrIter->h;
    return SLVS_E_UNKNOWN;
  }

  // Search the Rigid constraint
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for (; aConstrIter != myConstraints.end(); aConstrIter++)
    if (aConstrIter->type == SLVS_C_WHERE_DRAGGED &&
        aCPIter->find(aConstrIter->ptA) != aCPIter->end())
      return aConstrIter->h;
  return SLVS_E_UNKNOWN;
}


Slvs_hConstraint SketchSolver_Storage::addConstraint(const Slvs_Constraint& theConstraint)
{
  if (theConstraint.h > 0 && theConstraint.h <= myConstrMaxID) {
    // Constraint is already used, rewrite it
    return updateConstraint(theConstraint);
  }

  Slvs_Constraint aConstraint = theConstraint;

  // Find a constraint with same type uses same arguments
  std::vector<Slvs_Constraint>::iterator aCIt = myConstraints.begin();
  for (; aCIt != myConstraints.end(); aCIt++) {
    if (aConstraint.type != aCIt->type)
      continue;
    if (aConstraint.ptA == aCIt->ptA && aConstraint.ptB == aCIt->ptB &&
        aConstraint.entityA == aCIt->entityA && aConstraint.entityB == aCIt->entityB &&
        aConstraint.entityC == aCIt->entityC && aConstraint.entityD == aCIt->entityD) {
      aConstraint.h = aCIt->h;
      return updateConstraint(aConstraint);
    }
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
        aConstraints.erase(anIt);
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
      }
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

bool SketchSolver_Storage::isCoincident(
    const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2) const
{
  std::vector< std::set<Slvs_hEntity> >::const_iterator aCIter = myCoincidentPoints.begin();
  for (; aCIter != myCoincidentPoints.end(); aCIter++)
    if (aCIter->find(thePoint1) != aCIter->end() && aCIter->find(thePoint2) != aCIter->end())
      return true;
  return false;
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
  if (aResIndex == -1)
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
