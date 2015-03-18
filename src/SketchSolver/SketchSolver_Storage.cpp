// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Storage.cpp
// Created: 18 Mar 2015
// Author:  Artem ZHIDKOV

#include <SketchSolver_Storage.h>

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


Slvs_hConstraint SketchSolver_Storage::addConstraint(const Slvs_Constraint& theConstraint)
{
  if (theConstraint.h > 0 && theConstraint.h <= myConstrMaxID) {
    // Constraint is already used, rewrite it
    return updateConstraint(theConstraint);
  }

  Slvs_Constraint aConstraint = theConstraint;
  if (aConstraint.h > myConstrMaxID)
    myConstrMaxID = aConstraint.h;
  else
    aConstraint.h = ++myConstrMaxID;
  myConstraints.push_back(aConstraint);
  myNeedToResolve = true;
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
    myNeedToResolve = true;
    myRemovedConstraints.insert(theConstraintID);
    // Remove all entities
    Slvs_hEntity anEntities[6] = {aConstraint.ptA, aConstraint.ptB,
        aConstraint.entityA, aConstraint.entityB,
        aConstraint.entityC, aConstraint.entityD};
    for (int i = 0; i < 6; i++)
      if (anEntities[i] != SLVS_E_UNKNOWN) {
        aResult = removeEntity(anEntities[i]) && aResult;
        // remove temporary fixed points, if exists
        std::vector<Slvs_hEntity>::iterator aFPIt = myFixedPoints.begin();
        for (; aFPIt != myFixedPoints.end(); aFPIt++)
          if (*aFPIt == anEntities[i]) {
            myFixedPoints.erase(aFPIt);
            break;
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

void SketchSolver_Storage::addTemporaryConstraint(const Slvs_hConstraint& theConstraintID)
{
  int aPos = Search(theConstraintID, myConstraints);
  myFixedPoints.push_back(myConstraints[aPos].ptA);
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
  theSolver.setConstraints(myConstraints.data(), (int)myConstraints.size());

  // initialize fixed points
  if (!myFixedPoints.empty()) {
    int aPos = Search(myFixedPoints.front(), myEntities);
    theSolver.setDraggedParameters(myEntities[aPos].param);
  }
}


// ========================================================
// =========      Auxiliary functions       ===============
// ========================================================

template<typename T>
int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities)
{
  int aResIndex = theEntityID <= theEntities.size() ? theEntityID - 1 : 0;
  int aVecSize = theEntities.size();
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
