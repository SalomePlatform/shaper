// Copyright (C) 2014-2024  CEA, EDF
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

#include <SolveSpaceSolver_Storage.h>
#include <SolveSpaceSolver_ConstraintWrapper.h>
#include <SolveSpaceSolver_EntityWrapper.h>
#include <SolveSpaceSolver_ParameterWrapper.h>
#include <SolveSpaceSolver_Builder.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <math.h>
#include <assert.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintMiddle.h>

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
/// \brief Compare two constraints to be different
static bool IsNotEqual(const Slvs_Constraint& theConstraint1,
                       const Slvs_Constraint& theConstraint2);


SolveSpaceSolver_Storage::SolveSpaceSolver_Storage(const GroupID& theGroup)
  : SketchSolver_Storage(theGroup),
    myWorkplaneID(SLVS_E_UNKNOWN),
    myParamMaxID(SLVS_E_UNKNOWN),
    myEntityMaxID(SLVS_E_UNKNOWN),
    myConstrMaxID(SLVS_C_UNKNOWN),
    myDuplicatedConstraint(false)
{
}

bool SolveSpaceSolver_Storage::update(ConstraintWrapperPtr theConstraint)
{
  bool isUpdated = false;
  std::shared_ptr<SolveSpaceSolver_ConstraintWrapper> aConstraint =
      std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(theConstraint);
  Slvs_Constraint aSlvsConstr = getConstraint((Slvs_hConstraint)aConstraint->id());
  if (aSlvsConstr.h == SLVS_C_UNKNOWN)
    aSlvsConstr = aConstraint->constraint();

  // update value of constraint if exist
  double aCoeff = aSlvsConstr.type == SLVS_C_DIAMETER ? 2.0 : 1.0;
  if (fabs(aSlvsConstr.valA - theConstraint->value() * aCoeff) > tolerance) {
    aSlvsConstr.valA = theConstraint->value() * aCoeff;
    isUpdated = true;
  }

  // update constrained entities
  Slvs_hEntity* aPnts[2] = {&aSlvsConstr.ptA, &aSlvsConstr.ptB};
  Slvs_hEntity* anEnts[4] = {&aSlvsConstr.entityA, &aSlvsConstr.entityB,
                             &aSlvsConstr.entityC, &aSlvsConstr.entityD};
  int aPtInd = 0;
  int aEntInd = 0;

  std::list<EntityWrapperPtr> anEntities = theConstraint->entities();
  std::list<EntityWrapperPtr>::iterator anIt = anEntities.begin();
  for (; anIt != anEntities.end(); ++anIt) {
    isUpdated = update(*anIt) || isUpdated;
    // do not update constrained entities for Multi constraints,
    // and for middle point constraint translated to equal lines
    ConstraintPtr aBaseConstraint = theConstraint->baseConstraint();
    if (aSlvsConstr.type == SLVS_C_MULTI_ROTATION || aSlvsConstr.type == SLVS_C_MULTI_TRANSLATION ||
       (aBaseConstraint && aBaseConstraint->getKind() == SketchPlugin_ConstraintMiddle::ID() &&
        aSlvsConstr.type != SLVS_C_AT_MIDPOINT))
      continue;

    Slvs_hEntity anID = (Slvs_hEntity)(*anIt)->id();
    if ((*anIt)->type() == ENTITY_POINT) {
      if (*(aPnts[aPtInd]) != anID) {
        *(aPnts[aPtInd]) = anID;
        isUpdated = true;
      }
      ++aPtInd;
    } else {
      if (*(anEnts[aEntInd]) != anID) {
        *(anEnts[aEntInd]) = anID;
        isUpdated = true;
      }
      ++aEntInd;
    }
  }

  // update constraint itself (do not update constraints Multi)
  if (aSlvsConstr.type != SLVS_C_MULTI_ROTATION && aSlvsConstr.type != SLVS_C_MULTI_TRANSLATION) {
    if (aSlvsConstr.wrkpl == SLVS_E_UNKNOWN && myWorkplaneID != SLVS_E_UNKNOWN)
      aSlvsConstr.wrkpl = myWorkplaneID;
    if (aSlvsConstr.group == SLVS_G_UNKNOWN)
      aSlvsConstr.group = (Slvs_hGroup)myGroupID;
    bool hasDupConstraints = myDuplicatedConstraint;
    Slvs_hConstraint aConstrID = updateConstraint(aSlvsConstr);
    if (aSlvsConstr.h == SLVS_C_UNKNOWN) {
      aConstraint->changeConstraint() = getConstraint(aConstrID);
      isUpdated = true;
      // check duplicated constraints based on different attributes
      if (myDuplicatedConstraint && findSameConstraint(aConstraint) && !hasDupConstraints)
        myDuplicatedConstraint = false;
    }
  }
  return isUpdated;
}

bool SolveSpaceSolver_Storage::update(EntityWrapperPtr theEntity)
{
  bool isUpdated = false;
  std::shared_ptr<SolveSpaceSolver_EntityWrapper> anEntity =
      std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(theEntity);
  Slvs_Entity aSlvsEnt = getEntity((Slvs_hEntity)anEntity->id());
  if (aSlvsEnt.h == SLVS_E_UNKNOWN)
    aSlvsEnt = anEntity->entity();

  std::list<ParameterWrapperPtr> aParams = theEntity->parameters();
  std::list<ParameterWrapperPtr>::iterator aPIt;
  // if the entity is an attribute, need to update its coordinates
  if (anEntity->baseAttribute()) {
    BuilderPtr aBuilder = SolveSpaceSolver_Builder::getInstance();
    EntityWrapperPtr anUpdAttr = aBuilder->createAttribute(anEntity->baseAttribute(), GID_UNKNOWN);
    if (anUpdAttr) {
      std::list<ParameterWrapperPtr> anUpdParams = anUpdAttr->parameters();
      std::list<ParameterWrapperPtr>::iterator anUpdIt = anUpdParams.begin();
      for (aPIt = aParams.begin(); aPIt != aParams.end() && anUpdIt != anUpdParams.end();
          ++aPIt, ++anUpdIt) {
        (*aPIt)->update(*anUpdIt);
      }
    }
  }

  // update parameters
  int anInd = 0;
  for (aPIt = aParams.begin(); aPIt != aParams.end(); ++aPIt, ++anInd) {
    assert(anInd < 4);
    isUpdated = update(*aPIt) || isUpdated;
    if (aSlvsEnt.param[anInd] != (Slvs_hEntity)(*aPIt)->id()) {
      isUpdated = true;
      aSlvsEnt.param[anInd] = (Slvs_hEntity)(*aPIt)->id();
    }
  }

  // update sub-entities
  std::list<EntityWrapperPtr> aSubEntities = theEntity->subEntities();
  std::list<EntityWrapperPtr>::iterator aSIt = aSubEntities.begin();
  for (anInd = 0; aSIt != aSubEntities.end(); ++aSIt, ++anInd) {
    assert(anInd < 4);
    isUpdated = update(*aSIt) || isUpdated;

    Slvs_hEntity anID = Slvs_hEntity((*aSIt)->id());
    if ((*aSIt)->type() == ENTITY_NORMAL)
      aSlvsEnt.normal = anID;
    else if ((*aSIt)->type() == ENTITY_SCALAR)
      aSlvsEnt.distance = anID;
    else if (aSlvsEnt.point[anInd] != anID) {
      aSlvsEnt.point[anInd] = anID;
      if ((*aSIt)->baseAttribute())
        SketchSolver_Storage::addEntity((*aSIt)->baseAttribute(), *aSIt);
      isUpdated = true;
    }
  }
  if (theEntity->type() == ENTITY_POINT && aSubEntities.size() == 1) {
    // theEntity is based on SketchPlugin_Point => need to substitute its attribute instead
    bool isNew = (aSlvsEnt.h == SLVS_E_UNKNOWN);
    aSlvsEnt = getEntity(aSlvsEnt.point[0]);
    if (isNew) {
      anEntity->changeEntity() = aSlvsEnt;
      isUpdated = true;
    }
  }

  // update entity itself
  if (aSlvsEnt.wrkpl == SLVS_E_UNKNOWN && myWorkplaneID != SLVS_E_UNKNOWN)
    aSlvsEnt.wrkpl = myWorkplaneID;
  if (aSlvsEnt.group == SLVS_G_UNKNOWN)
    aSlvsEnt.group = (Slvs_hGroup)myGroupID;
  Slvs_hEntity anEntID = updateEntity(aSlvsEnt);
  if (aSlvsEnt.h == SLVS_E_UNKNOWN || isUpdated) {
    anEntity->changeEntity() = getEntity(anEntID);
    isUpdated = true;

    if (anEntity->type() == ENTITY_SKETCH)
      storeWorkplane(anEntity);
  }

  return isUpdated;
}

bool SolveSpaceSolver_Storage::update(ParameterWrapperPtr theParameter)
{
  std::shared_ptr<SolveSpaceSolver_ParameterWrapper> aParameter =
      std::dynamic_pointer_cast<SolveSpaceSolver_ParameterWrapper>(theParameter);
  const Slvs_Param& aParam = getParameter((Slvs_hParam)aParameter->id());
  if (aParam.h != SLVS_E_UNKNOWN && fabs(aParam.val - aParameter->value()) < tolerance)
    return false;
  Slvs_Param aParamToUpd = aParameter->parameter();
  if (aParamToUpd.group == SLVS_G_UNKNOWN)
    aParamToUpd.group = aParameter->isParametric() ? (Slvs_hGroup)GID_OUTOFGROUP :
                                                     (Slvs_hGroup)myGroupID;
  Slvs_hParam anID = updateParameter(aParamToUpd);
  if (aParam.h == SLVS_E_UNKNOWN) // new parameter
    aParameter->changeParameter() = getParameter(anID);
  return true;
}

void SolveSpaceSolver_Storage::storeWorkplane(EntityWrapperPtr theSketch)
{
  mySketchID = theSketch->id();
  myWorkplaneID = (Slvs_hEntity)mySketchID;

  // Update sub-entities of the sketch
  std::list<EntityWrapperPtr> aSubEntities = theSketch->subEntities();
  std::list<EntityWrapperPtr>::iterator aSIt = aSubEntities.begin();
  for (; aSIt != aSubEntities.end(); ++aSIt) {
    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aSub =
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(*aSIt);
    aSub->changeEntity().wrkpl = myWorkplaneID;
  }

  // Update all stored entities
  std::vector<Slvs_Entity>::iterator anIt = myEntities.begin();
  for (; anIt != myEntities.end(); ++anIt)
    anIt->wrkpl = myWorkplaneID;
}

void SolveSpaceSolver_Storage::changeGroup(EntityWrapperPtr theEntity, const GroupID& theGroup)
{
  std::list<ParameterWrapperPtr> aParams = theEntity->parameters();
  std::list<ParameterWrapperPtr>::iterator aPIt = aParams.begin();
  for (; aPIt != aParams.end(); ++aPIt)
    changeGroup(*aPIt, theGroup);

  std::list<EntityWrapperPtr> aSubs = theEntity->subEntities();
  std::list<EntityWrapperPtr>::iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++aSIt)
    changeGroup(*aSIt, theGroup);

  theEntity->setGroup(theGroup);
  int aPos = Search((Slvs_hEntity)theEntity->id(), myEntities);
  if (aPos >= 0 && aPos < (int)myEntities.size()) {
    if (myEntities[aPos].group != (Slvs_hGroup)theGroup)
      setNeedToResolve(true);
    myEntities[aPos].group = (Slvs_hGroup)theGroup;
  }
}

void SolveSpaceSolver_Storage::changeGroup(ParameterWrapperPtr theParam, const GroupID& theGroup)
{
  GroupID aGroup = theGroup;
  if (theParam->isParametric())
    aGroup = GID_OUTOFGROUP;
  if (theParam->group() == aGroup)
    return;

  theParam->setGroup(aGroup);
  int aPos = Search((Slvs_hParam)theParam->id(), myParameters);
  if (aPos >= 0 && aPos < (int)myParameters.size()) {
    myParameters[aPos].group = (Slvs_hGroup)aGroup;
    setNeedToResolve(true);
  }
}

void SolveSpaceSolver_Storage::addCoincidentPoints(
    EntityWrapperPtr theMaster, EntityWrapperPtr theSlave)
{
  if (theMaster->type() != ENTITY_POINT || theSlave->type() != ENTITY_POINT)
    return;
  if (!theMaster->subEntities().empty() || !theSlave->subEntities().empty()) {
    EntityWrapperPtr aSubMaster = theMaster->subEntities().empty() ?
        theMaster : theMaster->subEntities().front();
    EntityWrapperPtr aSubSlave = theSlave->subEntities().empty() ?
        theSlave : theSlave->subEntities().front();
    return addCoincidentPoints(aSubMaster, aSubSlave);
  }

  // Search available coincidence
  CoincidentPointsMap::iterator aMasterFound = myCoincidentPoints.find(theMaster);
  CoincidentPointsMap::iterator aSlaveFound = myCoincidentPoints.find(theSlave);
  if (aMasterFound == myCoincidentPoints.end() || aSlaveFound == myCoincidentPoints.end()) {
    // try to find master and slave points in the lists of slaves of already existent coincidences
    CoincidentPointsMap::iterator anIt = myCoincidentPoints.begin();
    for (; anIt != myCoincidentPoints.end(); ++anIt) {
      if (anIt->second.find(theMaster) != anIt->second.end())
        aMasterFound = anIt;
      else if (anIt->second.find(theSlave) != anIt->second.end())
        aSlaveFound = anIt;

      if (aMasterFound != myCoincidentPoints.end() &&  aSlaveFound != myCoincidentPoints.end())
        break;
    }
  }

  if (aMasterFound == myCoincidentPoints.end()) {
    // create new group
    myCoincidentPoints[theMaster] = std::set<EntityWrapperPtr>();
    aMasterFound = myCoincidentPoints.find(theMaster);
  } else if (aMasterFound == aSlaveFound)
    return; // already coincident

  if (aSlaveFound != myCoincidentPoints.end()) {
    // A slave has been found, we need to attach all points coincident with it to the new master
    std::set<EntityWrapperPtr> aNewSlaves = aSlaveFound->second;
    aNewSlaves.insert(aSlaveFound->first);
    myCoincidentPoints.erase(aSlaveFound);

    std::set<EntityWrapperPtr>::const_iterator aSlIt = aNewSlaves.begin();
    for (; aSlIt != aNewSlaves.end(); ++aSlIt)
      addCoincidentPoints(theMaster, *aSlIt);
  } else {
    // Update the slave if it was used in constraints and features
    replaceInFeatures(theSlave, theMaster);
    replaceInConstraints(theSlave, theMaster);

    // Remove slave entity (if the IDs are equal no need to remove slave entity, just update it)
    if (theMaster->id() != theSlave->id())
      removeEntity((Slvs_hEntity)theSlave->id());

    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aPointMaster =
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(theMaster);
    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aPointSlave =
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(theSlave);
    aPointSlave->changeEntity() = aPointMaster->entity();
    aPointSlave->setParameters(aPointMaster->parameters());

    aMasterFound->second.insert(theSlave);
  }
}

void SolveSpaceSolver_Storage::replaceInFeatures(
    EntityWrapperPtr theSource, EntityWrapperPtr theDest)
{
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator anIt = myFeatureMap.begin();
  for (; anIt != myFeatureMap.end(); ++anIt) {
    if (!anIt->second)
      continue;
    bool isUpdated = false;
    std::list<EntityWrapperPtr> aSubs = anIt->second->subEntities();
    std::list<EntityWrapperPtr>::iterator aSubIt = aSubs.begin();
    for (; aSubIt != aSubs.end(); ++aSubIt)
      if ((*aSubIt)->id() == theSource->id()) {
        (*aSubIt)->update(theDest);
        isUpdated = true;
      }

    if (!isUpdated)
      continue;

    std::shared_ptr<SolveSpaceSolver_EntityWrapper> aWrapper =
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(anIt->second);
    // update SolveSpace entity
    Slvs_Entity anEnt = aWrapper->entity();
    for (int i = 0; i < 4; ++i)
      if (anEnt.point[i] == (Slvs_hEntity)theSource->id())
        anEnt.point[i] = (Slvs_hEntity)theDest->id();
    anEnt.h = updateEntity(anEnt);
    aWrapper->changeEntity() = anEnt;

    // update sub-entities
    aWrapper->setSubEntities(aSubs);
  }
}

void SolveSpaceSolver_Storage::replaceInConstraints(
    EntityWrapperPtr theSource, EntityWrapperPtr theDest)
{
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      anIt = myConstraintMap.begin();
  std::list<ConstraintWrapperPtr>::const_iterator aCIt;
  for (; anIt != myConstraintMap.end(); ++anIt)
    for (aCIt = anIt->second.begin(); aCIt != anIt->second.end(); ++aCIt) {
      // Do not process coincidence between points and "multi" constraints
      // (these constraints are stored to keep the structure of constraints).
      if ((*aCIt)->type() == CONSTRAINT_PT_PT_COINCIDENT ||
          (*aCIt)->type() == CONSTRAINT_MULTI_ROTATION ||
          (*aCIt)->type() == CONSTRAINT_MULTI_TRANSLATION)
        continue;

      bool isUpdated = false;
      std::list<EntityWrapperPtr> aSubs = (*aCIt)->entities();
      std::list<EntityWrapperPtr>::iterator aSubIt = aSubs.begin();
      for (; aSubIt != aSubs.end(); ++aSubIt)
        if ((*aSubIt)->id() == theSource->id()) {
          (*aSubIt)->update(theDest);
          isUpdated = true;
        }

      if (!isUpdated)
        continue;

      std::shared_ptr<SolveSpaceSolver_ConstraintWrapper> aWrapper =
          std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(*aCIt);
      if (theSource->id() == theDest->id()) {
        // No need to update SolveSpace constraint if the entities are the same
        aWrapper->changeConstraint() = getConstraint((Slvs_hConstraint)aWrapper->id());
        aWrapper->setEntities(aSubs);
        continue;
      }

      // change constraint entities
      Slvs_Constraint aConstr = aWrapper->constraint();
      if (aConstr.ptA == (Slvs_hEntity)theSource->id())
        aConstr.ptA = (Slvs_hEntity)theDest->id();
      if (aConstr.ptB == (Slvs_hEntity)theSource->id())
        aConstr.ptB = (Slvs_hEntity)theDest->id();

      // check the constraint is duplicated
      std::vector<Slvs_Constraint>::const_iterator aSlvsCIt = myConstraints.begin();
      for (; aSlvsCIt != myConstraints.end(); ++aSlvsCIt)
        if (aConstr.h != aSlvsCIt->h &&
            aConstr.type == aSlvsCIt->type &&
            aConstr.ptA == aSlvsCIt->ptA && aConstr.ptB == aSlvsCIt->ptB &&
            aConstr.entityA == aSlvsCIt->entityA && aConstr.entityB == aSlvsCIt->entityB &&
            aConstr.entityC == aSlvsCIt->entityC && aConstr.entityD == aSlvsCIt->entityD) {
          Slvs_hConstraint anIDToRemove = aConstr.h;
          aConstr = *aSlvsCIt;
          int aShift = (int)(aSlvsCIt - myConstraints.begin());
          removeConstraint(anIDToRemove);
          aSlvsCIt = myConstraints.begin() + aShift - 1;
          for (; aSlvsCIt != myConstraints.end(); ++aSlvsCIt)
            if (aSlvsCIt->h == aConstr.h)
              break;
          break;
        }

      if (aSlvsCIt != myConstraints.end()) {
        // constraint is duplicated, search its wrapper to add the mapping
        std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
            anIt2 = myConstraintMap.begin();
        std::list<ConstraintWrapperPtr>::const_iterator aCIt2;
        for (; anIt2 != myConstraintMap.end(); ++anIt2)
          for (aCIt2 = anIt2->second.begin(); aCIt2 != anIt2->second.end(); ++aCIt2)
            if ((Slvs_hConstraint)(*aCIt2)->id() == aConstr.h) {
              addSameConstraints(*aCIt2, aWrapper);
              break;
            }
      } else
        aConstr.h = updateConstraint(aConstr);
      aWrapper->changeConstraint() = aConstr;

      // update sub-entities
      aWrapper->setEntities(aSubs);
    }
}

void SolveSpaceSolver_Storage::addSameConstraints(ConstraintWrapperPtr theConstraint1,
                                                  ConstraintWrapperPtr theConstraint2)
{
  SameConstraintMap::iterator anIt = myEqualConstraints.begin();
  for (; anIt != myEqualConstraints.end(); ++anIt) {
    if (anIt->find(theConstraint1) != anIt->end()) {
      anIt->insert(theConstraint2);
      return;
    }
    else if (anIt->find(theConstraint2) != anIt->end()) {
      anIt->insert(theConstraint1);
      return;
    }
  }
  // group not found => create new one
  std::set<ConstraintWrapperPtr> aNewGroup;
  aNewGroup.insert(theConstraint1);
  aNewGroup.insert(theConstraint2);
  myEqualConstraints.push_back(aNewGroup);
}

bool SolveSpaceSolver_Storage::findSameConstraint(ConstraintWrapperPtr theConstraint)
{
  if (theConstraint->type() == CONSTRAINT_PT_PT_COINCIDENT ||
      theConstraint->type() == CONSTRAINT_MULTI_ROTATION ||
      theConstraint->type() == CONSTRAINT_MULTI_TRANSLATION)
    return false;

  const Slvs_Constraint& aCBase =
      std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(theConstraint)->constraint();

  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> >::const_iterator
      aCIt = myConstraintMap.begin();
  std::list<ConstraintWrapperPtr>::const_iterator aCWIt;
  for (; aCIt != myConstraintMap.end(); ++aCIt)
    for (aCWIt = aCIt->second.begin(); aCWIt != aCIt->second.end(); ++aCWIt) {
      if ((*aCWIt)->type() == CONSTRAINT_PT_PT_COINCIDENT ||
          (*aCWIt)->type() == CONSTRAINT_MULTI_ROTATION ||
          (*aCWIt)->type() == CONSTRAINT_MULTI_TRANSLATION)
        continue;
      if ((*aCWIt)->type() == theConstraint->type()) {
        const Slvs_Constraint& aCComp = getConstraint((Slvs_hConstraint)(*aCWIt)->id());

        if (aCBase.ptA == aCComp.ptA && aCBase.ptB == aCComp.ptB &&
            aCBase.entityA == aCComp.entityA && aCBase.entityB == aCComp.entityB &&
            aCBase.entityC == aCComp.entityC && aCBase.entityD == aCComp.entityD &&
            fabs(aCBase.valA -aCComp.valA) < tolerance) {
          addSameConstraints(*aCWIt, theConstraint);
          return true;
        }
      }
    }
  return false;
}


EntityWrapperPtr SolveSpaceSolver_Storage::calculateMiddlePoint(
    EntityWrapperPtr theBase, double theCoeff)
{
  BuilderPtr aBuilder = SolveSpaceSolver_Builder::getInstance();

  std::shared_ptr<GeomAPI_XY> aMidPoint;
  if (theBase->type() == ENTITY_LINE) {
    std::shared_ptr<GeomAPI_Pnt2d> aPoints[2];
    const std::list<EntityWrapperPtr>& aSubs = theBase->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt = aSubs.begin();
    for (int i = 0; i < 2; ++i, ++anIt)
      aPoints[i] = aBuilder->point(*anIt);
    aMidPoint = aPoints[0]->xy()->multiplied(1.0 - theCoeff)->added(
        aPoints[1]->xy()->multiplied(theCoeff));
  }
  else if (theBase->type() == ENTITY_ARC) {
    double theX, theY;
    double anArcPoint[3][2];
    const std::list<EntityWrapperPtr>& aSubs = theBase->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt = aSubs.begin();
    for (int i = 0; i < 3; ++i, ++anIt) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aBuilder->point(*anIt);
      anArcPoint[i][0] = aPoint->x();
      anArcPoint[i][1] = aPoint->y();
    }
    // project last point of arc on the arc
    double x = anArcPoint[1][0] - anArcPoint[0][0];
    double y = anArcPoint[1][1] - anArcPoint[0][1];
    double aRad = sqrt(x*x + y*y);
    x = anArcPoint[2][0] - anArcPoint[0][0];
    y = anArcPoint[2][1] - anArcPoint[0][1];
    double aNorm = sqrt(x*x + y*y);
    if (aNorm >= tolerance) {
      anArcPoint[2][0] = x * aRad / aNorm;
      anArcPoint[2][1] = y * aRad / aNorm;
    }
    anArcPoint[1][0] -= anArcPoint[0][0];
    anArcPoint[1][1] -= anArcPoint[0][1];
    if (theCoeff < tolerance) {
      theX = anArcPoint[0][0] + anArcPoint[1][0];
      theY = anArcPoint[0][1] + anArcPoint[1][1];
    } else if (1 - theCoeff < tolerance) {
      theX = anArcPoint[0][0] + anArcPoint[2][0];
      theY = anArcPoint[0][1] + anArcPoint[2][1];
    } else {
      std::shared_ptr<GeomAPI_Dir2d>
        aStartDir(new GeomAPI_Dir2d(anArcPoint[1][0], anArcPoint[1][1]));
      std::shared_ptr<GeomAPI_Dir2d>
        aEndDir(new GeomAPI_Dir2d(anArcPoint[2][0], anArcPoint[2][1]));
      double anAngle = aStartDir->angle(aEndDir);
      if (anAngle < 0)
        anAngle += 2.0 * PI;
      anAngle *= theCoeff;
      double aCos = cos(anAngle);
      double aSin = sin(anAngle);
      theX = anArcPoint[0][0] + anArcPoint[1][0] * aCos - anArcPoint[1][1] * aSin;
      theY = anArcPoint[0][1] + anArcPoint[1][0] * aSin + anArcPoint[1][1] * aCos;
    }
    aMidPoint = std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(theX, theY));
  }

  if (!aMidPoint)
    return EntityWrapperPtr();

  std::list<ParameterWrapperPtr> aParameters;
  Slvs_Param aParam1 = Slvs_MakeParam(SLVS_E_UNKNOWN, (Slvs_hGroup)myGroupID, aMidPoint->x());
  aParam1.h = addParameter(aParam1);
  aParameters.push_back(ParameterWrapperPtr(new SolveSpaceSolver_ParameterWrapper(aParam1)));
  Slvs_Param aParam2 = Slvs_MakeParam(SLVS_E_UNKNOWN, (Slvs_hGroup)myGroupID, aMidPoint->y());
  aParam2.h = addParameter(aParam2);
  aParameters.push_back(ParameterWrapperPtr(new SolveSpaceSolver_ParameterWrapper(aParam2)));
  // Create entity (parameters are not filled)
  Slvs_Entity anEntity = Slvs_MakePoint2d(SLVS_E_UNKNOWN, (Slvs_hGroup)myGroupID,
      (Slvs_hEntity)myWorkplaneID, aParam1.h, aParam2.h);
  anEntity.h = addEntity(anEntity);

  EntityWrapperPtr aResult(new SolveSpaceSolver_EntityWrapper(AttributePtr(), anEntity));
  aResult->setParameters(aParameters);
  return aResult;
}






Slvs_hParam SolveSpaceSolver_Storage::addParameter(const Slvs_Param& theParam)
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

Slvs_hParam SolveSpaceSolver_Storage::updateParameter(const Slvs_Param& theParam)
{
  if (theParam.h > 0 && theParam.h <= myParamMaxID) {
    // parameter already used, rewrite it
    int aPos = Search(theParam.h, myParameters);
    if (aPos >= 0 && aPos < (int)myParameters.size()) {
      if (IsNotEqual(myParameters[aPos], theParam))
        setNeedToResolve(true);
      myParameters[aPos] = theParam;
      return theParam.h;
    }
  }

  // Parameter is not found, add new one
  Slvs_Param aParam = theParam;
  aParam.h = 0;
  return addParameter(aParam);
}

bool SolveSpaceSolver_Storage::removeParameter(const Slvs_hParam& theParamID)
{
  int aPos = Search(theParamID, myParameters);
  if (aPos >= 0 && aPos < (int)myParameters.size()) {
    // Firstly, search the parameter is not used elsewhere
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
  }
  return true;
}

const Slvs_Param& SolveSpaceSolver_Storage::getParameter(const Slvs_hParam& theParamID) const
{
  int aPos = Search(theParamID, myParameters);
  if (aPos >= 0 && aPos < (int)myParameters.size())
    return myParameters[aPos];

  // Parameter is not found, return empty object
  static Slvs_Param aDummy;
  aDummy.h = 0;
  return aDummy;
}


Slvs_hEntity SolveSpaceSolver_Storage::addEntity(const Slvs_Entity& theEntity)
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

Slvs_hEntity SolveSpaceSolver_Storage::updateEntity(const Slvs_Entity& theEntity)
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

bool SolveSpaceSolver_Storage::removeEntity(const Slvs_hEntity& theEntityID)
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
      if (anEntAndSubs.find(anEntIter->h) != anEntAndSubs.end())
        continue;
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
      aResult = aResult && removeEntity(anEntity.distance);
    for (int i = 0; i < 4; i++)
      if (anEntity.param[i] != SLVS_E_UNKNOWN)
        aResult = removeParameter(anEntity.param[i]) && aResult;
    myNeedToResolve = true;
  }
  return aResult;
}

const Slvs_Entity& SolveSpaceSolver_Storage::getEntity(const Slvs_hEntity& theEntityID) const
{
  int aPos = Search(theEntityID, myEntities);
  if (aPos >= 0 && aPos < (int)myEntities.size())
    return myEntities[aPos];

  // Entity is not found, return empty object
  static Slvs_Entity aDummy;
  aDummy.h = SLVS_E_UNKNOWN;
  return aDummy;
}


Slvs_hConstraint SolveSpaceSolver_Storage::addConstraint(const Slvs_Constraint& theConstraint)
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
        aConstraint.entityC == aCIt->entityC && aConstraint.entityD == aCIt->entityD) {
      myDuplicatedConstraint = true;
      break;
    }
  }

  if (aConstraint.h > myConstrMaxID)
    myConstrMaxID = aConstraint.h;
  else
    aConstraint.h = ++myConstrMaxID;
  myConstraints.push_back(aConstraint);
  myNeedToResolve = true;
  return aConstraint.h;
}

Slvs_hConstraint SolveSpaceSolver_Storage::updateConstraint(const Slvs_Constraint& theConstraint)
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

  // Firstly, check middle-point constraint conflicts with point-on-line
  if (aConstraint.type == SLVS_C_AT_MIDPOINT) {
    std::vector<Slvs_Constraint>::const_iterator anIt = myConstraints.begin();
    for (; anIt != myConstraints.end(); ++anIt)
      if (anIt->type == SLVS_C_PT_ON_LINE &&
          anIt->ptA == aConstraint.ptA &&
          anIt->entityA == aConstraint.entityA)
        break;
    if (anIt != myConstraints.end()) {
      // change the constraint to the lengths equality to avoid conflicts
      Slvs_Entity aLine = getEntity(aConstraint.entityA);
      Slvs_Entity aNewLine1 = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, (Slvs_hGroup)myGroupID,
          myWorkplaneID, aLine.point[0], aConstraint.ptA);
      aNewLine1.h = addEntity(aNewLine1);
      Slvs_Entity aNewLine2 = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, (Slvs_hGroup)myGroupID,
          myWorkplaneID, aLine.point[1], aConstraint.ptA);
      aNewLine2.h = addEntity(aNewLine2);
      aConstraint = Slvs_MakeConstraint(SLVS_E_UNKNOWN, (Slvs_hGroup)myGroupID,
          SLVS_C_EQUAL_LENGTH_LINES,
          myWorkplaneID, 0.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aNewLine1.h, aNewLine2.h);
    }
  }

  return addConstraint(aConstraint);
}

bool SolveSpaceSolver_Storage::removeConstraint(const Slvs_hConstraint& theConstraintID)
{
  int aPos = Search(theConstraintID, myConstraints);
  if (aPos >= 0 && aPos < (int)myConstraints.size()) {
    Slvs_Constraint aConstraint = myConstraints[aPos];
    myConstraints.erase(myConstraints.begin() + aPos);
    myConstrMaxID = myConstraints.empty() ? SLVS_E_UNKNOWN : myConstraints.back().h;
    myNeedToResolve = true;

    if (myDuplicatedConstraint) {
      // Find a constraint with same type uses same arguments
      std::vector<Slvs_Constraint>::iterator aCIt = myConstraints.begin();
      for (; aCIt != myConstraints.end(); aCIt++) {
        if (aConstraint.type != aCIt->type)
          continue;
        if (aConstraint.ptA == aCIt->ptA && aConstraint.ptB == aCIt->ptB &&
            aConstraint.entityA == aCIt->entityA && aConstraint.entityB == aCIt->entityB &&
            aConstraint.entityC == aCIt->entityC && aConstraint.entityD == aCIt->entityD) {
          myDuplicatedConstraint = false;
          break;
        }
      }
    }
  }
  return true;
}

const Slvs_Constraint& SolveSpaceSolver_Storage::
  getConstraint(const Slvs_hConstraint& theConstraintID) const
{
  int aPos = Search(theConstraintID, myConstraints);
  if (aPos >= 0 && aPos < (int)myConstraints.size())
    return myConstraints[aPos];

  // Constraint is not found, return empty object
  static Slvs_Constraint aDummy;
  aDummy.h = 0;
  return aDummy;
}


void SolveSpaceSolver_Storage::initializeSolver(SolverPtr theSolver)
{
  std::shared_ptr<SolveSpaceSolver_Solver> aSolver =
      std::dynamic_pointer_cast<SolveSpaceSolver_Solver>(theSolver);
  if (!aSolver)
    return;

  if (myExistArc)
    processArcs();

  if (myConstraints.empty()) {
    // Adjust all arc to place their points correctly
    std::vector<Slvs_Entity>::const_iterator anEntIt = myEntities.begin();
    for (; anEntIt != myEntities.end(); ++anEntIt)
      if (anEntIt->type == SLVS_E_ARC_OF_CIRCLE)
        adjustArc(*anEntIt);
  }

  aSolver->setParameters(myParameters.data(), (int)myParameters.size());
  aSolver->setEntities(myEntities.data(), (int)myEntities.size());
  aSolver->setConstraints(myConstraints.data(), (int)myConstraints.size());
}


bool SolveSpaceSolver_Storage::remove(ConstraintWrapperPtr theConstraint)
{
  std::shared_ptr<SolveSpaceSolver_ConstraintWrapper> aConstraint =
      std::dynamic_pointer_cast<SolveSpaceSolver_ConstraintWrapper>(theConstraint);

  // verify whether the constraint has duplicated
  bool hasSameID = false;
  SameConstraintMap::iterator anEqIt = myEqualConstraints.begin();
  for (; anEqIt != myEqualConstraints.end(); ++anEqIt) {
    std::set<ConstraintWrapperPtr>::const_iterator aFound = anEqIt->find(aConstraint);
    if (aFound != anEqIt->end()) {
      // verify there is a constraint with same ID
      std::set<ConstraintWrapperPtr>::const_iterator anIt = anEqIt->begin();
      ConstraintID anID = (*aFound)->id();
      for (++anIt; anIt != anEqIt->end() && !hasSameID; ++anIt)
        if ((*anIt)->id() == anID && aFound != anIt)
          hasSameID = true;
      // erase constraint
      anEqIt->erase(aConstraint);
      break;
    }
  }
  if (anEqIt != myEqualConstraints.end() && hasSameID)
    return true;

  bool isFullyRemoved = removeConstraint((Slvs_hConstraint)aConstraint->id());
  // remove point-point coincidence
  if (aConstraint->type() == CONSTRAINT_PT_PT_COINCIDENT)
    isFullyRemoved = removeCoincidence(theConstraint) && isFullyRemoved;
  return SketchSolver_Storage::remove(theConstraint) && isFullyRemoved;
}

bool SolveSpaceSolver_Storage::remove(EntityWrapperPtr theEntity)
{
  if (!theEntity)
    return false;

  // Additional check for entity to be used in point-point coincidence
  bool isCoincide = false;
  if (theEntity->type() == ENTITY_POINT) {
    CoincidentPointsMap::const_iterator anIt = myCoincidentPoints.begin();
    std::set<EntityWrapperPtr>::const_iterator aCIt;
    for (; anIt != myCoincidentPoints.end(); ++anIt) {
      if (anIt->first == theEntity)
        break;
      for (aCIt = anIt->second.begin(); aCIt != anIt->second.end(); ++aCIt)
        if (*aCIt == theEntity)
          break;
      if (aCIt != anIt->second.end())
        break;
    }
    if (anIt != myCoincidentPoints.end()) {
      if (anIt->first != theEntity && isUsed(anIt->first->baseAttribute()))
        isCoincide = true;
      for (aCIt = anIt->second.begin(); !isCoincide && aCIt != anIt->second.end(); ++aCIt)
        if (*aCIt != theEntity && isUsed((*aCIt)->baseAttribute()))
          isCoincide = true;
    }
  }

  std::shared_ptr<SolveSpaceSolver_EntityWrapper> anEntity =
        std::dynamic_pointer_cast<SolveSpaceSolver_EntityWrapper>(theEntity);
  bool isFullyRemoved = isCoincide ? true : removeEntity((Slvs_hEntity)anEntity->id());
  return (SketchSolver_Storage::remove(theEntity) || isCoincide) && isFullyRemoved;
}

bool SolveSpaceSolver_Storage::remove(ParameterWrapperPtr theParameter)
{
  return removeParameter((Slvs_hParam)theParameter->id());
}


void SolveSpaceSolver_Storage::refresh(bool theFixedOnly) const
{
  //blockEvents(true);

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anIt = myAttributeMap.begin();
  std::list<ParameterWrapperPtr> aParams;
  std::list<ParameterWrapperPtr>::const_iterator aParIt;
  for (; anIt != myAttributeMap.end(); ++anIt) {
    if (!anIt->second)
      continue;
    // the external feature always should keep the up to date values, so,
    // refresh from the solver is never needed
    if (anIt->first.get()) {
      std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(anIt->first->owner());
      if (aSketchFeature.get() && aSketchFeature->isExternal())
        continue;
      // not need to refresh here sketch's origin and normal vector
      CompositeFeaturePtr aSketch =
          std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(anIt->first->owner());
      if (aSketch)
        continue;
    }

    // update parameter wrappers and obtain values of attributes
    aParams = anIt->second->parameters();
    double aCoords[3];
    bool isUpd[3] = {false};
    int i = 0;
    for (aParIt = aParams.begin(); i < 3 && aParIt != aParams.end(); ++aParIt, ++i) {
      std::shared_ptr<SolveSpaceSolver_ParameterWrapper> aWrapper =
          std::dynamic_pointer_cast<SolveSpaceSolver_ParameterWrapper>(*aParIt);
      if (!theFixedOnly || aWrapper->group() == GID_OUTOFGROUP || aWrapper->isParametric()) {
        aWrapper->changeParameter().val = getParameter((Slvs_hParam)aWrapper->id()).val;
        aCoords[i] = aWrapper->value();
        isUpd[i] = true;
      }
    }
    if (!isUpd[0] && !isUpd[1] && !isUpd[2])
      continue; // nothing is updated

    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anIt->first);
    if (aPoint2D) {
      if ((isUpd[0] && fabs(aPoint2D->x() - aCoords[0]) > tolerance) ||
          (isUpd[1] && fabs(aPoint2D->y() - aCoords[1]) > tolerance)) {
        if (!isUpd[0]) aCoords[0] = aPoint2D->x();
        if (!isUpd[1]) aCoords[1] = aPoint2D->y();
        aPoint2D->setValue(aCoords[0], aCoords[1]);
        // Find points coincident with this one (probably not in GID_OUTOFGROUP)
        std::map<AttributePtr, EntityWrapperPtr>::const_iterator aLocIt;
        if (theFixedOnly)
          aLocIt = myAttributeMap.begin();
        else {
          aLocIt = anIt;
          ++aLocIt;
        }
        for (; aLocIt != myAttributeMap.end(); ++aLocIt) {
          if (!aLocIt->second)
            continue;
          std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aLocIt->first->owner());
          if (aSketchFeature && aSketchFeature->isExternal())
            continue;
          if (anIt->second->id() == aLocIt->second->id()) {
            aPoint2D = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aLocIt->first);
            aPoint2D->setValue(aCoords[0], aCoords[1]);
          }
        }
      }
      continue;
    }
    AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anIt->first);
    if (aScalar) {
      if (isUpd[0] && fabs(aScalar->value() - aCoords[0]) > tolerance)
        aScalar->setValue(aCoords[0]);
      continue;
    }
    std::shared_ptr<GeomDataAPI_Point> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(anIt->first);
    if (aPoint) {
      if ((isUpd[0] && fabs(aPoint->x() - aCoords[0]) > tolerance) ||
          (isUpd[1] && fabs(aPoint->y() - aCoords[1]) > tolerance) ||
          (isUpd[2] && fabs(aPoint->z() - aCoords[2]) > tolerance))
        if (!isUpd[0]) aCoords[0] = aPoint->x();
        if (!isUpd[1]) aCoords[1] = aPoint->y();
        if (!isUpd[2]) aCoords[2] = aPoint->z();
        aPoint->setValue(aCoords[0], aCoords[1], aCoords[2]);
      continue;
    }
  }

  //blockEvents(false);
}

void SolveSpaceSolver_Storage::verifyFixed()
{
  std::map<AttributePtr, EntityWrapperPtr>::iterator anAttrIt = myAttributeMap.begin();
  for (; anAttrIt != myAttributeMap.end(); ++anAttrIt) {
    if (!anAttrIt->second)
      continue;
    if (anAttrIt->second->group() == GID_OUTOFGROUP) {
      Slvs_Entity anEnt = getEntity((Slvs_hEntity)anAttrIt->second->id());
      if (anEnt.group != (Slvs_hEntity)GID_OUTOFGROUP)
        anEnt.group = (Slvs_hEntity)GID_OUTOFGROUP;
      updateEntity(anEnt);
    }

    const std::list<ParameterWrapperPtr>& aParameters = anAttrIt->second->parameters();
    std::list<ParameterWrapperPtr>::const_iterator aParIt = aParameters.begin();
    for (; aParIt != aParameters.end(); ++aParIt)
      if (anAttrIt->second->group() == GID_OUTOFGROUP || (*aParIt)->group() == GID_OUTOFGROUP) {
        Slvs_Param aParam = getParameter((Slvs_hParam)(*aParIt)->id());
        if (aParam.group != (Slvs_hParam)GID_OUTOFGROUP) {
          aParam.group = (Slvs_hParam)GID_OUTOFGROUP;
          updateParameter(aParam);
        }
      }
  }
}


void SolveSpaceSolver_Storage::adjustArc(const Slvs_Entity& theArc)
{
  double anArcPoints[3][2];
  double aDist[3] = {0.0};
  bool isFixed[3] = {false};
  for (int i = 0; i < 3; ++i) {
    Slvs_Entity aPoint = getEntity(theArc.point[i]);
    isFixed[i] = (aPoint.group != (Slvs_hGroup)myGroupID);
    anArcPoints[i][0] = getParameter(aPoint.param[0]).val;
    anArcPoints[i][1] = getParameter(aPoint.param[1]).val;
    if (i > 0) {
      anArcPoints[i][0] -= anArcPoints[0][0];
      anArcPoints[i][1] -= anArcPoints[0][1];
      aDist[i] = sqrt(anArcPoints[i][0] * anArcPoints[i][0] +
                      anArcPoints[i][1] * anArcPoints[i][1]);
    }
  }

  if (fabs(aDist[1] - aDist[2]) < tolerance)
    return;

  int anInd = 2;
  while (anInd > 0 && isFixed[anInd])
    --anInd;
  if (anInd < 1)
    return; // adjust only start or end point of the arc

  anArcPoints[anInd][0] /= aDist[anInd];
  anArcPoints[anInd][1] /= aDist[anInd];

  Slvs_Entity aPoint = getEntity(theArc.point[anInd]);
  for (int i = 0; i < 2; ++i) {
    Slvs_Param aParam = getParameter(aPoint.param[i]);
    aParam.val = anArcPoints[0][i] + aDist[3-anInd] * anArcPoints[anInd][i];
    updateParameter(aParam);
  }
}







// ========================================================
// =========      Auxiliary functions       ===============
// ========================================================

template<typename T>
int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities)
{
  int aResIndex = theEntityID <= theEntities.size() ? theEntityID - 1 : 0;
  int aVecSize = (int)theEntities.size();
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
