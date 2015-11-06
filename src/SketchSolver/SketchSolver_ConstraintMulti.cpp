#include <SketchSolver_ConstraintMulti.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Arc.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <math.h>

void SketchSolver_ConstraintMulti::processEntities(const std::vector< std::vector<Slvs_hEntity> >& theEntAndCopies)
{
  // Keep all objects unchanged (only initial object may be changed by user)
  myCircsAndCopies.clear();
  std::vector<std::vector<Slvs_hEntity> >::const_iterator anEntIt = theEntAndCopies.begin();
  std::vector<Slvs_hEntity>::const_iterator aCpIt;
  for (; anEntIt != theEntAndCopies.end(); ++anEntIt) {
    std::vector<Slvs_hEntity> aCircs;
    aCpIt = anEntIt->begin();
    // Obtain initial points
    Slvs_Entity anInitial = myStorage->getEntity(*aCpIt);
    if (anInitial.type == SLVS_E_POINT_IN_2D || anInitial.type == SLVS_E_POINT_IN_3D)
      myInitialPoints.insert(anInitial.h);
    else {
      for (int i = 0; i < 4 && anInitial.point[i] != SLVS_E_UNKNOWN; i++)
        myInitialPoints.insert(anInitial.point[i]);
    }

    // Fix the copies
    for (++aCpIt; aCpIt != anEntIt->end(); ++aCpIt) {
      const Slvs_Entity& anEntity = myStorage->getEntity(*aCpIt);
      std::vector<Slvs_hConstraint> aNewConstr;
      if (anEntity.type == SLVS_E_CIRCLE) {
        aCircs.push_back(anEntity.distance);
        // for circles we fix only center
        aNewConstr = myStorage->fixEntity(anEntity.point[0]);
      } else
        aNewConstr = myStorage->fixEntity(*aCpIt);
      if (anEntity.type == SLVS_E_ARC_OF_CIRCLE)
        aCircs.push_back(anEntity.h);
      mySlvsConstraints.insert(mySlvsConstraints.end(), aNewConstr.begin(), aNewConstr.end());
    }

    if (!aCircs.empty()) {
      if (anInitial.type == SLVS_E_CIRCLE)
        aCircs.insert(aCircs.begin(), anInitial.distance);
      else
        aCircs.insert(aCircs.begin(), anInitial.h);
      myCircsAndCopies.push_back(aCircs);
    }
  }
}

void SketchSolver_ConstraintMulti::update(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (!theConstraint || theConstraint == myBaseConstraint) {
    AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeIntegerPtr aNbObjects = myBaseConstraint->integer(nameNbObjects());
    if (anInitialRefList->size() != myNumberOfObjects || aNbObjects->value()-1 != myNumberOfCopies) {
      remove(myBaseConstraint);
      process();
      return;
    }
  }

  updateLocal();
  SketchSolver_Constraint::update();
}

bool SketchSolver_ConstraintMulti::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  bool isFullyRemoved = true;
  std::vector<Slvs_hEntity>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++)
   isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;
  mySlvsConstraints.clear();

  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.begin();
  for (; aFeatIt != myFeatureMap.end(); aFeatIt++)
    myStorage->removeEntity(aFeatIt->second);
  myStorage->removeUnusedEntities();

  std::map<FeaturePtr, Slvs_hEntity> aFeatureMapCopy = myFeatureMap;

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();

  // Restore initial features
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFIter = aFeatureMapCopy.begin();
  for (; aFIter != aFeatureMapCopy.end(); ++aFIter)
  {
    if (myFeatureMap.find(aFIter->first) != myFeatureMap.end())
      continue; // the feature was not removed
    Slvs_hEntity anEntity = myGroup->getFeatureId(aFIter->first);
    if (anEntity != SLVS_E_UNKNOWN)
      myFeatureMap[aFIter->first] = anEntity;
  }

  // Clear list of rotated points
  myPointsAndCopies.clear();
  myInitialPoints.clear();

  return true;
}

void SketchSolver_ConstraintMulti::addFeature(FeaturePtr theFeature)
{
  SketchSolver_Constraint::addFeature(theFeature);

  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.find(theFeature);
  if (aFeatIt == myFeatureMap.end())
    return;

  // store list of points of the feature
  const Slvs_Entity& theEntity = myStorage->getEntity(aFeatIt->second);
  for (int i = 0; i < 4; i++)
    if (theEntity.point[i] != SLVS_E_UNKNOWN)
      myPointsJustUpdated.insert(theEntity.point[i]);
}

void SketchSolver_ConstraintMulti::adjustConstraint()
{
  if (myAdjusted)
    return; // constraint already adjusted, don't do it once again

  double aRelCoord[2]  = {0.0, 0.0}; // relative coordinates of point
  double anAbsCoord[2] = {0.0, 0.0}; // absolute coordinates of point

  std::list<Slvs_Constraint> aCoincident = myStorage->getConstraintsByType(SLVS_C_POINTS_COINCIDENT);
  std::list<Slvs_Constraint>::const_iterator aCoIt;

  // Update positions of all points to satisfy angles
  std::vector< std::vector<Slvs_hEntity> >::const_iterator aPointsIter = myPointsAndCopies.begin();
  std::vector<Slvs_hEntity>::const_iterator aCopyIter;
  for (; aPointsIter != myPointsAndCopies.end(); ++aPointsIter) {
    aCopyIter = aPointsIter->begin();
    const Slvs_Entity& anInitial = myStorage->getEntity(*aCopyIter);
    for (int i = 0; i < 2; i++)
      anAbsCoord[i] = myStorage->getParameter(anInitial.param[i]).val;
    getRelative(anAbsCoord[0], anAbsCoord[1], aRelCoord[0], aRelCoord[1]);

    // if the point is coincident with another one which is temporary fixed (moved by user),
    // we will update its position correspondingly
    Slvs_hConstraint aFixed;
    for (aCoIt = aCoincident.begin(); aCoIt != aCoincident.end(); ++aCoIt) {
      if ((aCoIt->ptA == anInitial.h && myInitialPoints.find(aCoIt->ptB) != myInitialPoints.end()) ||
          (aCoIt->ptB == anInitial.h && myInitialPoints.find(aCoIt->ptA) != myInitialPoints.end())) {
        Slvs_hEntity anOtherId = aCoIt->ptA == anInitial.h ? aCoIt->ptB : aCoIt->ptA;
        if (!myStorage->isTemporary(aFixed) &&
            myPointsJustUpdated.find(anOtherId) == myPointsJustUpdated.end())
          continue; // nothing to change

        const Slvs_Entity& anOtherPnt = myStorage->getEntity(anOtherId);
        for (int i = 0; i < 2; i++) {
          Slvs_Param anInitParam = myStorage->getParameter(anInitial.param[i]);
          const Slvs_Param& anOtherParam = myStorage->getParameter(anOtherPnt.param[i]);
          anInitParam.val = anOtherParam.val;
          myStorage->updateParameter(anInitParam);
          anAbsCoord[i] = anOtherParam.val;
        }
        getRelative(anAbsCoord[0], anAbsCoord[1], aRelCoord[0], aRelCoord[1]);
      }
    }

    // update copied points
    aCopyIter = aPointsIter->begin();
    for (++aCopyIter; aCopyIter != aPointsIter->end(); ++aCopyIter) {
      // transform coordinates
      transformRelative(aRelCoord[0], aRelCoord[1]);
      getAbsolute(aRelCoord[0], aRelCoord[1], anAbsCoord[0], anAbsCoord[1]);

      const Slvs_Entity& aTarget = myStorage->getEntity(*aCopyIter);
      for (int i = 0; i < 2; i++) {
        Slvs_Param aParam = myStorage->getParameter(aTarget.param[i]);
        aParam.val = anAbsCoord[i];
        myStorage->updateParameter(aParam);
      }
    }
  }

  std::list<Slvs_Constraint> aDiamConstr;
  for (aPointsIter = myCircsAndCopies.begin(); aPointsIter != myCircsAndCopies.end(); ++aPointsIter) {
    aCopyIter = aPointsIter->begin();
    const Slvs_Entity& anInitial = myStorage->getEntity(*aCopyIter);
    if (anInitial.type == SLVS_E_DISTANCE) {
      const Slvs_Param& anInitRad = myStorage->getParameter(anInitial.param[0]);
      for (++aCopyIter; aCopyIter != aPointsIter->end(); ++aCopyIter) {
        const Slvs_Entity& aCopy = myStorage->getEntity(*aCopyIter);
        Slvs_Param aCopyRad = myStorage->getParameter(aCopy.param[0]);
        aCopyRad.val = anInitRad.val;
        myStorage->updateParameter(aCopyRad);
      }
    } else if (anInitial.type == SLVS_E_ARC_OF_CIRCLE) {
      const Slvs_Entity& aCenterEnt = myStorage->getEntity(anInitial.point[0]);
      const Slvs_Entity& aStartEnt = myStorage->getEntity(anInitial.point[1]);

      if (aDiamConstr.empty())
        aDiamConstr = myStorage->getConstraintsByType(SLVS_C_DIAMETER);
      // Calculate diameter of initial arc
      double aDiam = 0.0;
      for (int i = 0; i < 2; i++) {
        double d = myStorage->getParameter(aStartEnt.param[i]).val -
                   myStorage->getParameter(aCenterEnt.param[i]).val;
        aDiam += d * d;
      }
      aDiam = sqrt(aDiam) * 2.0;
      // Update the Diameter constraints of copied arcs
      for (++aCopyIter; aCopyIter != aPointsIter->end(); ++aCopyIter) {
        std::list<Slvs_Constraint>::iterator aDCIt = aDiamConstr.begin();
        for (; aDCIt != aDiamConstr.end(); ++aDCIt)
          if (aDCIt->entityA == *aCopyIter) {
            aDCIt->valA = aDiam;
            myStorage->updateConstraint(*aDCIt);
            aDiamConstr.erase(aDCIt);
            break;
          }
      }
    }
  }

  myPointsJustUpdated.clear();
  myAdjusted = true;
}

void SketchSolver_ConstraintMulti::checkCoincidence()
{
  std::vector< std::vector<Slvs_hEntity> > aFilteredPoints; // points are filtered by their positions

  std::vector< std::vector<Slvs_hEntity> >::const_iterator aPCIt = myPointsAndCopies.begin();
  std::vector<Slvs_hEntity>::const_iterator aCIt;
  for (; aPCIt != myPointsAndCopies.end(); ++aPCIt) {
    aCIt = aPCIt->begin();
    // Skip first element, focus the copies only
    for (++aCIt; aCIt != aPCIt->end(); ++aCIt) {
      std::vector< std::vector<Slvs_hEntity> >::iterator aFilterIt = aFilteredPoints.begin();
      for (; aFilterIt != aFilteredPoints.end(); ++aFilterIt)
        if (myStorage->isEqual(*aCIt, aFilterIt->front())) {
          aFilterIt->push_back(*aCIt);
          break;
        }
      if (aFilterIt == aFilteredPoints.end()) {
        std::vector<Slvs_hEntity> aNewFilter(1, *aCIt);
        aFilteredPoints.push_back(aNewFilter);
      }
    }
  }

  // Check the coicidence of filtered points and remove extra fixation.
  // Also check separated points which are not fixed.
  std::vector< std::vector<Slvs_hEntity> >::iterator aFPIt = aFilteredPoints.begin();
  for (; aFPIt != aFilteredPoints.end(); ++aFPIt) {
    if (aFPIt->size() <= 1)
      continue;
    std::vector<Slvs_hEntity>::iterator anIt1, anIt2;
    for (anIt1 = aFPIt->begin(); anIt1 != aFPIt->end(); ++anIt1) {
      for (anIt2 = anIt1 + 1; anIt2 != aFPIt->end(); ++anIt2) {
        Slvs_hConstraint aFixed1, aFixed2;
        bool isFixed1 = myStorage->isPointFixed(*anIt1, aFixed1);
        bool isFixed2 = myStorage->isPointFixed(*anIt2, aFixed2);
        if (myStorage->isCoincident(*anIt1, *anIt2)) {
          if (!isFixed1 && isFixed2) {
            Slvs_hEntity aTmp = *anIt1;
            *anIt1 = *anIt2;
            *anIt2 = aTmp;
          } else if (isFixed1 && isFixed2) {
            // remove fixing of the second point
            myStorage->removeConstraint(aFixed2);
            std::vector<Slvs_hConstraint>::iterator aRemoveIt = mySlvsConstraints.begin();
            for (; aRemoveIt != mySlvsConstraints.end(); ++aRemoveIt)
              if (*aRemoveIt == aFixed2) {
                mySlvsConstraints.erase(aRemoveIt);
                break;
              }
          }
        } else {
          bool isFixed[2] = {
              myStorage->isPointFixed(*anIt1, aFixed1, true),
              myStorage->isPointFixed(*anIt2, aFixed2, true)
          };

          Slvs_hEntity aPoint[2] = {*anIt1, *anIt2};
          for (int i = 0; i < 2; i++)
            if (!isFixed[i]) {
              Slvs_Constraint aConstraint = Slvs_MakeConstraint(SLVS_C_UNKNOWN, myGroup->getId(),
                  SLVS_C_WHERE_DRAGGED, myGroup->getWorkplaneId(), 0.0,
                  aPoint[i], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
              aConstraint.h = myStorage->addConstraint(aConstraint);
              mySlvsConstraints.push_back(aConstraint.h);
            }
        }
      }
    }
  }
}
