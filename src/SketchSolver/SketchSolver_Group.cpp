// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Group.h"

#include <SketchSolver_Builder.h>
#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>

#include <Events_Error.h>
#include <Events_Loop.h>
#include <GeomAPI_XY.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintRigid.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <math.h>
#include <assert.h>


/// \brief This class is used to give unique index to the groups
class GroupIndexer
{
public:
  /// \brief Return vacant index
  static Slvs_hGroup NEW_GROUP() { return ++myGroupIndex; }
  /// \brief Removes the index
  static void REMOVE_GROUP(const Slvs_hGroup& theIndex) {
    if (myGroupIndex == theIndex)
      myGroupIndex--;
  }

private:
  GroupIndexer() {};

  static Slvs_hGroup myGroupIndex; ///< index of the group
};

Slvs_hGroup GroupIndexer::myGroupIndex = 0;


/** \brief Search the entity/parameter with specified ID in the list of elements
 *  \param[in] theEntityID unique ID of the element
 *  \param[in] theEntities list of elements
 *  \return position of the found element or -1 if the element is not found
 */
template<typename T>
static int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities);

// ========================================================
// =========  SketchSolver_Group  ===============
// ========================================================

SketchSolver_Group::SketchSolver_Group(
    std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane)
    : myID(GroupIndexer::NEW_GROUP())
{
  // Initialize workplane
  myWorkplaneID = SLVS_E_UNKNOWN;
#ifndef NDEBUG
  assert(addWorkplane(theWorkplane));
#else
  addWorkplane(theWorkplane);
#endif
}

SketchSolver_Group::~SketchSolver_Group()
{
  myConstraints.clear();
  GroupIndexer::REMOVE_GROUP(myID);
}

// ============================================================================
//  Function: isBaseWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  verify the group is based on the given workplane
// ============================================================================
bool SketchSolver_Group::isBaseWorkplane(CompositeFeaturePtr theWorkplane) const
{
  return theWorkplane == mySketch;
}

// ============================================================================
//  Function: isInteract
//  Class:    SketchSolver_Group
//  Purpose:  verify are there any entities in the group used by given constraint
// ============================================================================
bool SketchSolver_Group::isInteract(
    std::shared_ptr<SketchPlugin_Feature> theFeature) const
{
  // Empty group interacts with everything
  if (isEmpty()) return true;
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstraint)
    return myFeatureStorage->isInteract(aConstraint);
  return myFeatureStorage->isInteract(theFeature);
}

// ============================================================================
//  Function: getFeatureId
//  Class:    SketchSolver_Group
//  Purpose:  Find the identifier of the feature, if it already exists in the group
// ============================================================================
Slvs_hEntity SketchSolver_Group::getFeatureId(FeaturePtr theFeature) const
{
  if (!myFeatureStorage)
    return SLVS_E_UNKNOWN;
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theFeature);
  if (aConstraints.empty())
    return SLVS_E_UNKNOWN;
  ConstraintConstraintMap::const_iterator aCIter = myConstraints.find(*aConstraints.begin());
  if (aCIter == myConstraints.end())
    return SLVS_E_UNKNOWN;
  return aCIter->second->getId(theFeature);
}

// ============================================================================
//  Function: getAttributeId
//  Class:    SketchSolver_Group
//  Purpose:  Find the identifier of the attribute, if it already exists in the group
// ============================================================================
Slvs_hEntity SketchSolver_Group::getAttributeId(AttributePtr theAttribute) const
{
  if (!myFeatureStorage)
    return SLVS_E_UNKNOWN;
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theAttribute);
  if (aConstraints.empty())
    return SLVS_E_UNKNOWN;
  ConstraintConstraintMap::const_iterator aCIter = myConstraints.find(*aConstraints.begin());
  if (aCIter == myConstraints.end())
    return SLVS_E_UNKNOWN;
  return aCIter->second->getId(theAttribute);
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_Group::changeConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // There is no workplane yet, something wrong
  if (myWorkplaneID == SLVS_E_UNKNOWN)
    return false;

  if (!theConstraint)
    return false;

  if (myConstraints.find(theConstraint) == myConstraints.end()) {
    // Add constraint to the current group
    SolverConstraintPtr aConstraint =
        SketchSolver_Builder::getInstance()->createConstraint(theConstraint);
    if (!aConstraint)
      return false;
    aConstraint->setGroup(this);
    aConstraint->setStorage(myStorage);
    if (!aConstraint->error().empty()) {
      if (aConstraint->error() == SketchSolver_Error::NOT_INITIALIZED())
        return false; // some attribute are not initialized yet, don't show message
      Events_Error::send(aConstraint->error(), this);
    }

    // Additional verification of coincidence of several points
    if (theConstraint->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
      for (; aCIter != myConstraints.end(); aCIter++) {
        std::shared_ptr<SketchSolver_ConstraintCoincidence> aCoincidence =
          std::dynamic_pointer_cast<SketchSolver_ConstraintCoincidence>(aCIter->second);
        if (!aCoincidence)
          continue;
        std::shared_ptr<SketchSolver_ConstraintCoincidence> aCoinc2 =
          std::dynamic_pointer_cast<SketchSolver_ConstraintCoincidence>(aConstraint);
        if (aCoincidence != aCoinc2 && aCoincidence->isCoincide(aCoinc2)) {
          aCoincidence->attach(aCoinc2);
          aConstraint = aCoincidence;
        }
      }
    }
    myConstraints[theConstraint] = aConstraint;
  }
  else
    myConstraints[theConstraint]->update();

  if (!myFeatureStorage)
    myFeatureStorage = FeatureStoragePtr(new SketchSolver_FeatureStorage);
  myFeatureStorage->changeConstraint(theConstraint);

////  if (theConstraint) {
////    if (theConstraint->getKind() == SketchPlugin_ConstraintRigid::ID())
////      return changeRigidConstraint(theConstraint);
////    if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID())
////      return changeMirrorConstraint(theConstraint);
////    if (theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID())
////      return changeFilletConstraint(theConstraint);
////  }
////
////  // Search this constraint in the current group to update it
////  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
////  std::vector<Slvs_Constraint>::iterator aConstrIter;
////  if (aConstrMapIter != myConstraintMap.end()) {
////    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
////    aConstrIter = myConstraints.begin() + aConstrPos;
////  }
////
////  // Get constraint type and verify the constraint parameters are correct
////  SketchSolver_Constraint aConstraint(theConstraint);
////  int aConstrType = aConstraint.getType();
////  if (aConstrType == SLVS_C_UNKNOWN
////      || (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
////    return false;
////  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();
////
////  // Create constraint parameters
////  double aDistance = 0.0;  // scalar value of the constraint
////  AttributeDoublePtr aDistAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
////      theConstraint->data()->attribute(SketchPlugin_Constraint::VALUE()));
////  if (aDistAttr) {
////    aDistance = aDistAttr->value();
////    // Issue #196: checking the positivity of the distance constraint
////    if (aDistance < tolerance &&
////       (aConstrType == SLVS_C_PT_PT_DISTANCE || aConstrType == SLVS_C_PT_LINE_DISTANCE))
////      return false;
////    // SketchPlugin circle defined by its radius, but SolveSpace uses constraint for diameter
////    if (aConstrType == SLVS_C_DIAMETER)
////      aDistance *= 2.0;
////    if (aConstrMapIter != myConstraintMap.end()
////        && fabs(aConstrIter->valA - aDistance) > tolerance) {
////      myNeedToSolve = true;
////      aConstrIter->valA = aDistance;
////    }
////  }
////
////  size_t aNbTmpConstraints = myTempConstraints.size();
////  Slvs_hEntity aConstrEnt[CONSTRAINT_ATTR_SIZE];  // parameters of the constraint
////  for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
////    aConstrEnt[indAttr] = SLVS_E_UNKNOWN;
////    std::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = std::dynamic_pointer_cast<
////        ModelAPI_AttributeRefAttr>(
////        theConstraint->data()->attribute(aConstraintAttributes[indAttr]));
////    if (!aConstrAttr)
////      continue;
////
////    // Convert the object of the attribute to the feature
////    FeaturePtr aFeature;
////    if (aConstrAttr->isObject() && aConstrAttr->object()) {
////      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
////          aConstrAttr->object());
////      if (!aRC)
////        continue;
////      std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
////      aFeature = aDoc->feature(aRC);
////    }
////
////    // For the length constraint the start and end points of the line should be added to the entities list instead of line
////    if (aConstrType == SLVS_C_PT_PT_DISTANCE
////        && theConstraint->getKind().compare(SketchPlugin_ConstraintLength::ID()) == 0) {
////      Slvs_hEntity aLineEnt = changeEntityFeature(aFeature);
////      int aEntPos = Search(aLineEnt, myEntities);
////      aConstrEnt[indAttr++] = myEntities[aEntPos].point[0];
////      aConstrEnt[indAttr++] = myEntities[aEntPos].point[1];
////      while (indAttr < CONSTRAINT_ATTR_SIZE)
////        aConstrEnt[indAttr++] = 0;
////      break;  // there should be no other entities
////    } else if (aConstrAttr->isObject())
////      aConstrEnt[indAttr] = changeEntityFeature(aFeature);
////    else
////      aConstrEnt[indAttr] = changeEntity(aConstrAttr->attr());
////  }
////
////  if (aConstrMapIter == myConstraintMap.end()) { // Add new constraint
////    // Several points may be coincident, it is not necessary to store all constraints between them.
////    // Try to find sequence of coincident points which connects the points of new constraint
////    if (aConstrType == SLVS_C_POINTS_COINCIDENT) {
////      if (aConstrEnt[0] == aConstrEnt[1])  // no need to add self coincidence
////        return false;
////      if (!addCoincidentPoints(aConstrEnt[0], aConstrEnt[1])) {
////        myExtraCoincidence.insert(theConstraint);  // the constraint is stored for further purposes
////        return false;
////      }
////      if (aNbTmpConstraints < myTempConstraints.size()) {
////        // There was added temporary constraint. Check that there is no coincident points which already rigid.
////
////        // Get list of already fixed points
////        std::set<Slvs_hEntity> anAlreadyFixed;
////        std::vector<Slvs_Constraint>::const_iterator aCIter = myConstraints.begin();
////        for (; aCIter != myConstraints.end(); aCIter++)
////          if (aCIter->type == SLVS_C_WHERE_DRAGGED) {
////            std::list<Slvs_hConstraint>::const_iterator aTmpIt = myTempConstraints.begin();
////            for (; aTmpIt != myTempConstraints.end(); aTmpIt++)
////              if (*aTmpIt == aCIter->h)
////                break;
////            if (aTmpIt == myTempConstraints.end())
////              anAlreadyFixed.insert(aCIter->ptA);
////          }
////
////        std::set<Slvs_hConstraint> aTmpConstrToDelete;
////        std::list<Slvs_hConstraint>::reverse_iterator aTmpIter = myTempConstraints.rbegin();
////        size_t aCurSize = myTempConstraints.size();
////        for (; aCurSize > aNbTmpConstraints && aTmpIter != myTempConstraints.rend();
////            aTmpIter++, aCurSize--) {
////          int aConstrPos = Search(*aTmpIter, myConstraints);
////          std::vector<std::set<Slvs_hEntity> >::const_iterator
////            aCoincIter = myCoincidentPoints.begin();
////          for (; aCoincIter != myCoincidentPoints.end(); aCoincIter++)
////            if (aCoincIter->find(myConstraints[aConstrPos].ptA) != aCoincIter->end()) {
////              std::set<Slvs_hEntity>::const_iterator anIt;
////              for (anIt = aCoincIter->begin(); anIt != aCoincIter->end(); anIt++)
////                if (anAlreadyFixed.find(*anIt) != anAlreadyFixed.end()) {
////                  aTmpConstrToDelete.insert(*aTmpIter);
////                  break;
////                }
////              break;
////            }
////        }
////        if (!aTmpConstrToDelete.empty())
////          removeTemporaryConstraints(aTmpConstrToDelete);
////      }
////    }
////    // For the tangency constraints it is necessary to identify which points of entities are coincident
////    int aSlvsOtherFlag = 0;
////    int aSlvsOther2Flag = 0;
////    if (aConstrType == SLVS_C_ARC_LINE_TANGENT || aConstrType == SLVS_C_CURVE_CURVE_TANGENT) {
////      // Search entities used by constraint
////      int anEnt1Pos = Search(aConstrEnt[2], myEntities);
////      int anEnt2Pos = Search(aConstrEnt[3], myEntities);
////      // Obtain start and end points of entities
////      Slvs_hEntity aPointsToFind[4];
////      aPointsToFind[0] = myEntities[anEnt1Pos].point[1];
////      aPointsToFind[1]= myEntities[anEnt1Pos].point[2];
////      bool hasLine = (myEntities[anEnt2Pos].type == SLVS_E_LINE_SEGMENT);
////      aPointsToFind[2]= myEntities[anEnt2Pos].point[hasLine ? 0 : 1];
////      aPointsToFind[3]= myEntities[anEnt2Pos].point[hasLine ? 1 : 2];
////      // Search coincident points
////      bool isPointFound[4];
////      std::vector<std::set<Slvs_hEntity> >::const_iterator aCPIter = myCoincidentPoints.begin();
////      for ( ; aCPIter != myCoincidentPoints.end(); aCPIter++) {
////        for (int i = 0; i < 4; i++)
////          isPointFound[i] = (aCPIter->find(aPointsToFind[i]) != aCPIter->end());
////        if ((isPointFound[0] || isPointFound[1]) && (isPointFound[2] || isPointFound[3])) {
////          // the arc is tangent by end point
////          if (isPointFound[1]) aSlvsOtherFlag = 1;
////          // the second item is an arc and it is tangent by end point too
////          if (!hasLine && isPointFound[3]) aSlvsOther2Flag = 1;
////          break;
////        }
////      }
////      if (aCPIter == myCoincidentPoints.end()) {
////        // There is no coincident points between tangential objects. Generate error message
////        Events_Error::send(SketchSolver_Error::NO_COINCIDENT_POINTS(), this);
////        return false;
////      }
////    }
////
////    // Create SolveSpace constraint structure
////    Slvs_Constraint aSlvsConstr = Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType,
////                                                      myWorkplane.h, aDistance, aConstrEnt[0],
////                                                      aConstrEnt[1], aConstrEnt[2], aConstrEnt[3]);
////    if (aSlvsOtherFlag != 0) aSlvsConstr.other = aSlvsOtherFlag;
////    if (aSlvsOther2Flag != 0) aSlvsConstr.other2 = aSlvsOther2Flag;
////    myConstraints.push_back(aSlvsConstr);
////    myConstraintMap[theConstraint] = std::vector<Slvs_hEntity>(1, aSlvsConstr.h);
////    int aConstrPos = Search(aSlvsConstr.h, myConstraints);
////    aConstrIter = myConstraints.begin() + aConstrPos;
////    myNeedToSolve = true;
////  } else { // Attributes of constraint may be changed => update constraint
////    Slvs_hEntity* aCurrentAttr[] = {&aConstrIter->ptA, &aConstrIter->ptB,
////                                   &aConstrIter->entityA, &aConstrIter->entityB,
////                                   &aConstrIter->entityC, &aConstrIter->entityD};
////    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
////      if (*(aCurrentAttr[indAttr]) != aConstrEnt[indAttr])
////      {
////        *(aCurrentAttr[indAttr]) = aConstrEnt[indAttr];
////        myNeedToSolve = true;
////      }
////    }
////  }
////
////  // Update flags of entities to be used by constraints
////  for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
////    if (aConstrEnt[indAttr] != 0) {
////      int aPos = Search(aConstrEnt[indAttr], myEntities);
////      myEntOfConstr[aPos] = true;
////      // Sub-entities should be used implcitly
////      Slvs_hEntity* aEntPtr = myEntities[aPos].point;
////      while (*aEntPtr != 0) {
////        aPos = Search(*aEntPtr, myEntities);
////        myEntOfConstr[aPos] = true;
////        aEntPtr++;
////      }
////    }
////
////  checkConstraintConsistence(*aConstrIter);
  return true;
}


bool SketchSolver_Group::updateFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  std::set<ConstraintPtr> aConstraints = myFeatureStorage->getConstraints(theFeature);
  if (aConstraints.empty())
    return false;
  std::set<ConstraintPtr>::iterator aCIter = aConstraints.begin();
  for (; aCIter != aConstraints.end(); aCIter++) {
    ConstraintConstraintMap::iterator aSolConIter = myConstraints.find(*aCIter);
    aSolConIter->second->update();
  }
  return true;
}

void SketchSolver_Group::moveFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  updateFeature(theFeature);
  // Temporary rigid constraint
  SolverConstraintPtr aConstraint =
      SketchSolver_Builder::getInstance()->createRigidConstraint(theFeature);
  if (!aConstraint)
    return;
  aConstraint->setGroup(this);
  aConstraint->setStorage(myStorage);
  myTempConstraints.insert(aConstraint);
}

////// ============================================================================
//////  Function: changeRigidConstraint
//////  Class:    SketchSolver_Group
//////  Purpose:  create/update the "Rigid" constraint in the group
////// ============================================================================
////bool SketchSolver_Group::changeRigidConstraint(
////    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
////{
////  // Search this constraint in the current group to update it
////  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
////  std::vector<Slvs_Constraint>::iterator aConstrIter;
////  if (aConstrMapIter != myConstraintMap.end()) {
////    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
////    aConstrIter = myConstraints.begin() + aConstrPos;
////  }
////
////  // Get constraint type and verify the constraint parameters are correct
////  SketchSolver_Constraint aConstraint(theConstraint);
////  int aConstrType = aConstraint.getType();
////  if (aConstrType == SLVS_C_UNKNOWN
////      || (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
////    return false;
////  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();
////
////  Slvs_hEntity aConstrEnt = SLVS_E_UNKNOWN;
////  std::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = std::dynamic_pointer_cast<
////      ModelAPI_AttributeRefAttr>(
////      theConstraint->data()->attribute(aConstraintAttributes[0]));
////  if (!aConstrAttr)
////    return false;
////
////  // Convert the object of the attribute to the feature
////  FeaturePtr aFeature;
////  if (aConstrAttr->isObject() && aConstrAttr->object()) {
////    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
////        aConstrAttr->object());
////    if (!aRC)
////      return false;
////    std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
////    aFeature = aDoc->feature(aRC);
////  }
////
////  aConstrEnt = aConstrAttr->isObject() ? changeEntityFeature(aFeature) : changeEntity(aConstrAttr->attr());
////
////  if (aConstrMapIter == myConstraintMap.end()) { // Add new constraint
////    // Check the fixed entity is not a point.
////    std::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = std::dynamic_pointer_cast<
////        ModelAPI_AttributeRefAttr>(theConstraint->data()->attribute(aConstraintAttributes[0]));
////    std::shared_ptr<GeomDataAPI_Point> aPoint =
////        std::dynamic_pointer_cast<GeomDataAPI_Point>(aConstrAttr->attr());
////    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
////        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aConstrAttr->attr());
////    if (aPoint || aPoint2D) {
////      // Create SolveSpace constraint structure
////      Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////          ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
////          aConstrEnt, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////      myConstraints.push_back(aConstraint);
////      myConstraintMap[theConstraint] = std::vector<Slvs_hEntity>(1, aConstraint.h);
////      int aConstrPos = Search(aConstraint.h, myConstraints);
////      aConstrIter = myConstraints.begin() + aConstrPos;
////      myNeedToSolve = true;
////    } else {
////      myConstraintMap[theConstraint] = std::vector<Slvs_hConstraint>();
////
////      // To avoid SolveSpace problems:
////      // * if the circle is rigid, we will fix its center and radius;
////      // * if the arc is rigid, we will fix its start and end points and radius.
////      double aRadius = 0.0;
////      bool isArc = false;
////      bool isCircle = false;
////      if (aFeature) {
////        if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
////          std::shared_ptr<GeomDataAPI_Point2D> aCenter =
////              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////              aFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
////          std::shared_ptr<GeomDataAPI_Point2D> aStart =
////              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////              aFeature->data()->attribute(SketchPlugin_Arc::START_ID()));
////          aRadius = aStart->pnt()->distance(aCenter->pnt());
////          isArc = true;
////        } else if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
////          aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
////              aFeature->data()->attribute(SketchPlugin_Circle::RADIUS_ID()))->value();
////          isCircle = true;
////        }
////      }
////
////      // Get list of already fixed points
////      std::set<Slvs_hEntity> anAlreadyFixed;
////      std::vector<Slvs_Constraint>::const_iterator aCIter = myConstraints.begin();
////      for (; aCIter != myConstraints.end(); aCIter++)
////        if (aCIter->type == SLVS_C_WHERE_DRAGGED)
////          anAlreadyFixed.insert(aCIter->ptA);
////
////      // Create constraints to fix the parameters of the entity
////      int aEntPos = Search(aConstrEnt, myEntities);
////      Slvs_hEntity* aPointsPtr = myEntities[aEntPos].point;
////      if (isArc) aPointsPtr++; // avoid to fix center of arc
////      while (*aPointsPtr != 0) {
////        // Avoid to create additional "Rigid" constraints for coincident points
////        bool isCoincAlreadyFixed = false;
////        if (!anAlreadyFixed.empty()) {
////          if (anAlreadyFixed.find(*aPointsPtr) != anAlreadyFixed.end())
////            isCoincAlreadyFixed = true;
////
////          std::vector<std::set<Slvs_hEntity> >::const_iterator aCoincIter =
////              myCoincidentPoints.begin();
////          for (; !isCoincAlreadyFixed && aCoincIter != myCoincidentPoints.end(); aCoincIter++) {
////            if (aCoincIter->find(*aPointsPtr) == aCoincIter->end())
////              continue;
////            std::set<Slvs_hEntity>::const_iterator anIter = anAlreadyFixed.begin();
////            for (; !isCoincAlreadyFixed && anIter != anAlreadyFixed.end(); anIter++)
////              if (aCoincIter->find(*anIter) != aCoincIter->end())
////                isCoincAlreadyFixed = true;
////          }
////        }
////
////        if (!isCoincAlreadyFixed) {
////          Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////              ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
////              *aPointsPtr, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////          myConstraints.push_back(aConstraint);
////          myConstraintMap[theConstraint].push_back(aConstraint.h);
////        }
////        aPointsPtr++;
////      }
////
////      if (isArc || isCircle) { // add radius constraint
////        Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////            ++myConstrMaxID, myID, SLVS_C_DIAMETER, myWorkplane.h, 2.0 * aRadius,
////            SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aConstrEnt, SLVS_E_UNKNOWN);
////        myConstraints.push_back(aConstraint);
////        myConstraintMap[theConstraint].push_back(aConstraint.h);
////      }
////
////      // The object is already rigid, so there is no constraints added
////      if (myConstraintMap[theConstraint].empty()) {
////        myConstraintMap.erase(theConstraint);
////        myNeedToSolve = false;
////      }
////      else
////        myNeedToSolve = true;
////    }
////  }
////  return true;
////}
////
////// ============================================================================
//////  Function: changeMirrorConstraint
//////  Class:    SketchSolver_Group
//////  Purpose:  create/update the "Mirror" constraint in the group
////// ============================================================================
////bool SketchSolver_Group::changeMirrorConstraint(
////    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
////{
////  DataPtr aConstrData = theConstraint->data();
////
////  // Search this constraint in the current group to update it
////  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
////  std::vector<Slvs_Constraint>::iterator aConstrIter;
////  if (aConstrMapIter != myConstraintMap.end()) {
////    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
////    aConstrIter = myConstraints.begin() + aConstrPos;
////  }
////
////  // Get constraint type and verify the constraint parameters are correct
////  SketchSolver_Constraint aConstraint(theConstraint);
////  int aConstrType = aConstraint.getType();
////  if (aConstrType == SLVS_C_UNKNOWN
////      || (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
////    return false;
////  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();
////
////  Slvs_hEntity aMirrorLineEnt = SLVS_E_UNKNOWN;
////  AttributeRefAttrPtr aConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////      aConstrData->attribute(aConstraintAttributes[0]));
////  if (!aConstrAttr)
////    return false;
////
////  // Convert the object of the attribute to the feature
////  FeaturePtr aMirrorLineFeat;
////  if (aConstrAttr->isObject() && aConstrAttr->object()) {
////    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
////        aConstrAttr->object());
////    if (!aRC)
////      return false;
////    std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
////    aMirrorLineFeat = aDoc->feature(aRC);
////  }
////  aMirrorLineEnt = aConstrAttr->isObject() ?
////      changeEntityFeature(aMirrorLineFeat) : changeEntity(aConstrAttr->attr());
////
////  if (aConstrMapIter == myConstraintMap.end()) { // Add new constraint
////    // Append symmetric constraint for each point of mirroring features
////    AttributeRefListPtr aBaseRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
////        aConstrData->attribute(aConstraintAttributes[1]));
////    AttributeRefListPtr aMirroredRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
////        aConstrData->attribute(aConstraintAttributes[2]));
////    if (!aBaseRefList || !aMirroredRefList)
////      return false;
////
////    std::list<ObjectPtr> aBaseList = aBaseRefList->list();
////    std::list<ObjectPtr> aMirroredList = aMirroredRefList->list();
////    if (aBaseList.size() != aMirroredList.size())
////      return false;
////
////    myConstraintMap[theConstraint] = std::vector<Slvs_hConstraint>();
////
////    FeaturePtr aBaseFeature, aMirrorFeature;
////    ResultConstructionPtr aRC;
////    std::list<ObjectPtr>::iterator aBaseIter = aBaseList.begin();
////    std::list<ObjectPtr>::iterator aMirIter = aMirroredList.begin();
////    for ( ; aBaseIter != aBaseList.end(); aBaseIter++, aMirIter++) {
////      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aBaseIter);
////      aBaseFeature = aRC ? aRC->document()->feature(aRC) :
////          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aBaseIter);
////      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aMirIter);
////      aMirrorFeature = aRC ? aRC->document()->feature(aRC) :
////          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aMirIter);
////
////      if (!aBaseFeature || !aMirrorFeature || 
////          aBaseFeature->getKind() != aMirrorFeature->getKind())
////        return false;
////      Slvs_hEntity aBaseEnt = changeEntityFeature(aBaseFeature);
////      Slvs_hEntity aMirrorEnt = changeEntityFeature(aMirrorFeature);
////      // Make aMirrorEnt parameters to be symmetric with aBaseEnt
////      makeMirrorEntity(aBaseEnt, aMirrorEnt, aMirrorLineEnt);
////
////      if (aBaseFeature->getKind() == SketchPlugin_Point::ID()) {
////        Slvs_Constraint aConstraint = Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType,
////            myWorkplane.h, 0.0, aBaseEnt, aMirrorEnt, aMirrorLineEnt, SLVS_E_UNKNOWN);
////        myConstraints.push_back(aConstraint);
////        myConstraintMap[theConstraint].push_back(aConstraint.h);
////      } else {
////        int aBasePos = Search(aBaseEnt, myEntities);
////        int aMirrorPos = Search(aMirrorEnt, myEntities);
////        if (aBaseFeature->getKind() == SketchPlugin_Line::ID()) {
////          for (int ind = 0; ind < 2; ind++) {
////            Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////                ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
////                myEntities[aBasePos].point[ind], myEntities[aMirrorPos].point[ind],
////                aMirrorLineEnt, SLVS_E_UNKNOWN);
////            myConstraints.push_back(aConstraint);
////            myConstraintMap[theConstraint].push_back(aConstraint.h);
////          }
////        } else if (aBaseFeature->getKind() == SketchPlugin_Circle::ID()) {
////          Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////              ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
////              myEntities[aBasePos].point[0], myEntities[aMirrorPos].point[0],
////              aMirrorLineEnt, SLVS_E_UNKNOWN);
////          myConstraints.push_back(aConstraint);
////          myConstraintMap[theConstraint].push_back(aConstraint.h);
////          // Additional constraint for equal radii
////          Slvs_Constraint anEqRadConstr = Slvs_MakeConstraint(
////              ++myConstrMaxID, myID, SLVS_C_EQUAL_RADIUS, myWorkplane.h, 0.0,
////              SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aBaseEnt, aMirrorEnt);
////          myConstraints.push_back(anEqRadConstr);
////          myConstraintMap[theConstraint].push_back(anEqRadConstr.h);
////        } else if (aBaseFeature->getKind() == SketchPlugin_Arc::ID()) {
////          // Workaround to avoid problems in SolveSpace.
////          // The symmetry of two arcs will be done using symmetry of three points on these arcs:
////          // start point, end point, and any other point on the arc
////          Slvs_hEntity aBaseArcPoints[3] = {
////              myEntities[aBasePos].point[1],
////              myEntities[aBasePos].point[2],
////              SLVS_E_UNKNOWN};
////          Slvs_hEntity aMirrorArcPoints[3] = { // indices of points of arc, center corresponds center, first point corresponds last point
////              myEntities[aMirrorPos].point[2],
////              myEntities[aMirrorPos].point[1],
////              SLVS_E_UNKNOWN};
////          Slvs_hEntity aBothArcs[2] = {aBaseEnt, aMirrorEnt};
////          Slvs_hEntity aBothMiddlePoints[2];
////          for (int i = 0; i < 2; i++) {
////            double x, y;
////            calculateMiddlePoint(aBothArcs[i], x, y);
////            std::vector<Slvs_Param>::iterator aParamIter = myParams.end();
////            Slvs_hParam u = changeParameter(x, aParamIter);
////            Slvs_hParam v = changeParameter(y, aParamIter);
////            Slvs_Entity aPoint = Slvs_MakePoint2d(++myEntityMaxID, myID, myWorkplane.h, u, v);
////            myEntities.push_back(aPoint);
////            aBothMiddlePoints[i] = aPoint.h;
////            // additional constraint point-on-curve
////            Slvs_Constraint aPonCircConstr = Slvs_MakeConstraint(
////                ++myConstrMaxID, myID, SLVS_C_PT_ON_CIRCLE, myWorkplane.h, 0.0,
////                aPoint.h, SLVS_E_UNKNOWN, aBothArcs[i], SLVS_E_UNKNOWN);
////            myConstraints.push_back(aPonCircConstr);
////            myConstraintMap[theConstraint].push_back(aPonCircConstr.h);
////          }
////
////          aBaseArcPoints[2] = aBothMiddlePoints[0];
////          aMirrorArcPoints[2] = aBothMiddlePoints[1];
////          for (int ind = 0; ind < 3; ind++) {
////            Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////                ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
////                aBaseArcPoints[ind], aMirrorArcPoints[ind], aMirrorLineEnt, SLVS_E_UNKNOWN);
////            myConstraints.push_back(aConstraint);
////            myConstraintMap[theConstraint].push_back(aConstraint.h);
////          }
////        }
////      }
////    }
////
////    // Set the mirror line unchanged during constraint recalculation
////    int aMirrorLinePos = Search(aMirrorLineEnt, myEntities);
////    Slvs_Constraint aRigidStart = Slvs_MakeConstraint(
////        ++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED, myWorkplane.h, 0,
////        myEntities[aMirrorLinePos].point[0], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////    myConstraints.push_back(aRigidStart);
////    myConstraintMap[theConstraint].push_back(aRigidStart.h);
////    Slvs_Constraint aRigidEnd = Slvs_MakeConstraint(
////        ++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED, myWorkplane.h, 0,
////        myEntities[aMirrorLinePos].point[1], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////    myConstraints.push_back(aRigidEnd);
////    myConstraintMap[theConstraint].push_back(aRigidEnd.h);
////
////    // Add temporary constraints for initial objects to be unchanged
////    for (aBaseIter = aBaseList.begin(); aBaseIter != aBaseList.end(); aBaseIter++) {
////      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aBaseIter);
////      aBaseFeature = aRC ? aRC->document()->feature(aRC) :
////          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aBaseIter);
////      if (!aBaseFeature) continue;
////      std::list<AttributePtr> aPoints = aBaseFeature->data()->attributes(GeomDataAPI_Point2D::type());
////      std::list<AttributePtr>::iterator anIt = aPoints.begin();
////      for ( ; anIt != aPoints.end(); anIt++) {
////        // Arcs are fixed by center and start points only (to avoid solving errors in SolveSpace)
////        if (aBaseFeature->getKind() == SketchPlugin_Arc::ID() &&
////            (*anIt)->id() == SketchPlugin_Arc::END_ID())
////          continue;
////        addTemporaryConstraintWhereDragged(*anIt);
////      }
////    }
////  }
////  return true;
////}
////
////// ============================================================================
//////  Function: changeFilletConstraint
//////  Class:    SketchSolver_Group
//////  Purpose:  create/update the "Fillet" constraint in the group
////// ============================================================================
////bool SketchSolver_Group::changeFilletConstraint(
////    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
////{
////  DataPtr aConstrData = theConstraint->data();
////
////  // Search this constraint in the current group to update it
////  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
////  std::vector<Slvs_Constraint>::iterator aConstrIter;
////  if (aConstrMapIter != myConstraintMap.end()) {
////    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
////    aConstrIter = myConstraints.begin() + aConstrPos;
////  }
////
////  // Get constraint type and verify the constraint parameters are correct
////  SketchSolver_Constraint aConstraint(theConstraint);
////  int aConstrType = aConstraint.getType();
////  if (aConstrType == SLVS_C_UNKNOWN)
////    return false;
////  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();
////
////  // Obtain hEntity for basic objects of fillet
////  Slvs_hEntity aBaseObject[2];
////  FeaturePtr aBaseFeature[2];
////  for (unsigned int indAttr = 0; indAttr < 2; indAttr++) {
////    AttributeRefAttrPtr aConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////        aConstrData->attribute(aConstraintAttributes[indAttr]));
////    if (!aConstrAttr)
////      return false;
////    if (aConstrAttr->isObject() && aConstrAttr->object()) {
////      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
////          aConstrAttr->object());
////      if (!aRC)
////        return false;
////      std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
////      aBaseFeature[indAttr] = aDoc->feature(aRC);
////    }
////    aBaseObject[indAttr] = aConstrAttr->isObject() ?
////        changeEntityFeature(aBaseFeature[indAttr]) : changeEntity(aConstrAttr->attr());
////  }
////  // Check the base entities have a coincident point
////  int aBaseObjInd[2] = {
////      Search(aBaseObject[0], myEntities),
////      Search(aBaseObject[1], myEntities)
////    };
////  int aShift[2] = { // shift for calculating correct start and end points for different types of objects
////      myEntities[aBaseObjInd[0]].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0,
////      myEntities[aBaseObjInd[1]].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0,
////    };
////  Slvs_hEntity aFirstObjPoints[2] = { // indices of start and end point of first object
////      myEntities[aBaseObjInd[0]].point[aShift[0]],
////      myEntities[aBaseObjInd[0]].point[1+aShift[0]]
////    };
////  Slvs_hEntity aSecondObjPoints[2] = { // indices of start and end point of second object
////      myEntities[aBaseObjInd[1]].point[aShift[1]],
////      myEntities[aBaseObjInd[1]].point[1+aShift[1]]
////    };
////  bool isCoincidentFound = false;
////  int aBaseCoincInd[2] = {0, 0}; // indices in aFirstObjPoint and aSecondObjPoint identifying coincident points
////  std::vector<std::set<Slvs_hEntity> >::iterator aCPIter = myCoincidentPoints.begin();
////  for ( ; aCPIter != myCoincidentPoints.end() && !isCoincidentFound; aCPIter++)
////    for (int ind1 = 0; ind1 < 2 && !isCoincidentFound; ind1++)
////      for (int ind2 = 0; ind2 < 2 && !isCoincidentFound; ind2++)
////        if (aCPIter->find(aFirstObjPoints[ind1]) != aCPIter->end() &&
////            aCPIter->find(aSecondObjPoints[ind2]) != aCPIter->end()) {
////          aBaseCoincInd[0] = ind1;
////          aBaseCoincInd[1] = ind2;
////          isCoincidentFound = true;
////        }
////  if (!isCoincidentFound) {
////    // There is no coincident points between objects. Generate error message
////    Events_Error::send(SketchSolver_Error::NO_COINCIDENT_POINTS(), this);
////    return false;
////  }
////
////  // Create fillet entities
////  // - first object is placed on the first base 
////  // - second object is on the second base 
////  // - third object is a filleting arc
////  static const int aNbFilletEnt = 3;
////  Slvs_hEntity aFilletEnt[aNbFilletEnt];
////  int aFilletObjInd[aNbFilletEnt];
////  AttributeRefListPtr aFilletRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
////      aConstrData->attribute(aConstraintAttributes[2]));
////  if (!aFilletRefList)
////    return false;
////  std::list<ObjectPtr> aFilletList = aFilletRefList->list();
////  if (aFilletList.size() < aNbFilletEnt)
////    return false;
////  FeaturePtr aFilletFeature;
////  ResultConstructionPtr aRC;
////  std::list<ObjectPtr>::iterator aFilIter = aFilletList.begin();
////  for (int indEnt = 0; aFilIter != aFilletList.end(); aFilIter++, indEnt++) {
////    aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aFilIter);
////    aFilletFeature = aRC ? aRC->document()->feature(aRC) :
////        std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFilIter);
////    if (!aFilletFeature)
////      return false;
////    aFilletEnt[indEnt] = changeEntityFeature(aFilletFeature);
////    aFilletObjInd[indEnt] = Search(aFilletEnt[indEnt], myEntities);
////  }
////  // At first time, for correct result, move floating points of fillet on the middle points of base objects
////  if (myConstraintMap.find(theConstraint) == myConstraintMap.end()) {
////    double anArcPoints[6];
////    for (int indEnt = 0; indEnt < aNbFilletEnt - 1; indEnt++) {
////      int anIndShift = myEntities[aFilletObjInd[indEnt]].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0;
////      int aPointsPos[2] = {
////          Search(myEntities[aFilletObjInd[indEnt]].point[anIndShift], myEntities),
////          Search(myEntities[aFilletObjInd[indEnt]].point[1+anIndShift], myEntities)
////        };
////      int aParamPos[2] = {
////          Search(myEntities[aPointsPos[0]].param[0], myParams),
////          Search(myEntities[aPointsPos[1]].param[0], myParams)
////        };
////      int anIndex = aParamPos[aBaseCoincInd[indEnt]];
////      if (anIndShift == 0) {
////        myParams[anIndex].val =
////            0.5 * (myParams[aParamPos[0]].val + myParams[aParamPos[1]].val);
////        myParams[1 + anIndex].val =
////            0.5 * (myParams[1 + aParamPos[0]].val + myParams[1 + aParamPos[1]].val);
////      } else { // place the changed point on the arc
////        double x = 0, y = 0;
////        calculateMiddlePoint(aFilletEnt[indEnt], x, y);
////        myParams[anIndex].val = x;
////        myParams[1 + anIndex].val = y;
////      }
////      anArcPoints[indEnt*2+2] = myParams[anIndex].val;
////      anArcPoints[indEnt*2+3] = myParams[1 + anIndex].val;
////    }
////    anArcPoints[0] = 0.5 * (anArcPoints[2] + anArcPoints[4]);
////    anArcPoints[1] = 0.5 * (anArcPoints[3] + anArcPoints[5]);
////    for (int indArcPt = 0; indArcPt < 3; indArcPt++) {
////      int aPtPos = Search(myEntities[aFilletObjInd[2]].point[indArcPt], myEntities);
////      int aParamPos = Search(myEntities[aPtPos].param[0], myParams);
////      myParams[aParamPos].val = anArcPoints[indArcPt * 2];
////      myParams[aParamPos + 1].val = anArcPoints[indArcPt * 2 + 1];
////    }
////  }
////
////  // Check the fillet arc which point to be connected to
////  bool isArcInversed = false; // indicates that start and end points of arc should be connected to second and first object respectively
////  Slvs_hEntity hEnt = myEntities[aFilletObjInd[2]].point[1];
////  int aPos = Search(hEnt, myEntities);
////  Slvs_hParam anArcStartPoint = myEntities[aPos].param[0];
////  aPos = Search(anArcStartPoint, myParams);
////  double anArcPtCoord[2] = {myParams[aPos].val, myParams[aPos+1].val};
////  double aSqDistances[2];
////  int aPtInd;
////  for (int indEnt = 0; indEnt < aNbFilletEnt - 1; indEnt++) {
////    aPtInd = aBaseCoincInd[indEnt]+aShift[indEnt];
////    hEnt = myEntities[aFilletObjInd[indEnt]].point[aPtInd];
////    aPos = Search(hEnt, myEntities);
////    Slvs_hParam anObjectPoint = myEntities[aPos].param[0];
////    aPos = Search(anObjectPoint, myParams);
////    double aPtCoord[2] = {myParams[aPos].val, myParams[aPos+1].val};
////    aSqDistances[indEnt] = 
////        (anArcPtCoord[0] - aPtCoord[0]) * (anArcPtCoord[0] - aPtCoord[0]) +
////        (anArcPtCoord[1] - aPtCoord[1]) * (anArcPtCoord[1] - aPtCoord[1]);
////  }
////  if (aSqDistances[1] < aSqDistances[0])
////    isArcInversed = true;
////
////  // Create list of constraints to generate fillet
////  std::vector<Slvs_hConstraint> aConstrList;
////  bool isExists = myConstraintMap.find(theConstraint) != myConstraintMap.end(); // constraint already exists
////  std::vector<Slvs_hConstraint>::iterator aCMapIter =
////    isExists ? myConstraintMap[theConstraint].begin() : aConstrList.begin();
////  int aCurConstrPos = isExists ? Search(*aCMapIter, myConstraints) : 0;
////  for (int indEnt = 0; indEnt < aNbFilletEnt - 1; indEnt++) {
////    // one point of fillet object should be coincident with the point on base, non-coincident with another base object
////    aPtInd = 1-aBaseCoincInd[indEnt]+aShift[indEnt]; // (1-aBaseCoincInd[indEnt]) = index of non-coincident point, aShift is used to process all types of shapes
////    Slvs_hEntity aPtBase = myEntities[aBaseObjInd[indEnt]].point[aPtInd];
////    Slvs_hEntity aPtFillet = myEntities[aFilletObjInd[indEnt]].point[aPtInd];
////    if (isExists) {
////      myConstraints[aCurConstrPos].ptA = aPtBase;
////      myConstraints[aCurConstrPos].ptB = aPtFillet;
////      aCMapIter++;
////      aCurConstrPos = Search(*aCMapIter, myConstraints);
////    } else {
////      Slvs_Constraint aCoincConstr = Slvs_MakeConstraint(
////          ++myConstrMaxID, myID, SLVS_C_POINTS_COINCIDENT, myWorkplane.h,
////          0, aPtBase, aPtFillet, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////      myConstraints.push_back(aCoincConstr);
////      aConstrList.push_back(aCoincConstr.h);
////    }
////
////    // another point of fillet object should be placed on the base object
////    Slvs_Constraint aPonCurveConstr;
////    int aTangentType;
////    if (myEntities[aFilletObjInd[indEnt]].type == SLVS_E_ARC_OF_CIRCLE) {
////      // centers of arcs should be coincident
////      aPtBase = myEntities[aBaseObjInd[indEnt]].point[0];
////      aPtFillet = myEntities[aFilletObjInd[indEnt]].point[0];
////      if (isExists) {
////        myConstraints[aCurConstrPos].ptA = aPtBase;
////        myConstraints[aCurConstrPos].ptB = aPtFillet;
////        aCMapIter++;
////        aCurConstrPos = Search(*aCMapIter, myConstraints);
////      } else {
////        aPonCurveConstr = Slvs_MakeConstraint(
////            ++myConstrMaxID, myID, SLVS_C_POINTS_COINCIDENT, myWorkplane.h,
////            0, aPtBase, aPtFillet, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////      }
////      aPtFillet = myEntities[aFilletObjInd[indEnt]].point[1+aBaseCoincInd[indEnt]]; // !!! will be used below
////      aTangentType = SLVS_C_CURVE_CURVE_TANGENT;
////    } else {
////      aPtInd = aBaseCoincInd[indEnt];
////      aPtFillet = myEntities[aFilletObjInd[indEnt]].point[aPtInd];
////      if (isExists) {
////        myConstraints[aCurConstrPos].ptA = aPtFillet;
////        aCMapIter++;
////        aCurConstrPos = Search(*aCMapIter, myConstraints);
////      } else {
////        aPonCurveConstr = Slvs_MakeConstraint(
////            ++myConstrMaxID, myID, SLVS_C_PT_ON_LINE, myWorkplane.h,
////            0, aPtFillet, SLVS_E_UNKNOWN, aBaseObject[indEnt], SLVS_E_UNKNOWN);
////      }
////      aTangentType = SLVS_C_ARC_LINE_TANGENT;
////    }
////    if (!isExists) {
////      myConstraints.push_back(aPonCurveConstr);
////      aConstrList.push_back(aPonCurveConstr.h);
////    }
////
////    // Bound point of fillet arc should be tangently coincident with a bound point of fillet object
////    aPtInd = 1 + (isArcInversed ? 1-indEnt : indEnt);
////    Slvs_hEntity aPtArc = myEntities[aFilletObjInd[2]].point[aPtInd];
////    if (isExists) {
////      myConstraints[aCurConstrPos].ptA = aPtArc;
////      myConstraints[aCurConstrPos].ptB = aPtFillet;
////      aCMapIter++;
////      aCurConstrPos = Search(*aCMapIter, myConstraints);
////      myConstraints[aCurConstrPos].entityA = aFilletEnt[2];
////      myConstraints[aCurConstrPos].entityB = aFilletEnt[indEnt];
////      myConstraints[aCurConstrPos].other = (isArcInversed ? 1-indEnt : indEnt);
////      aCMapIter++;
////      aCurConstrPos = Search(*aCMapIter, myConstraints);
////    } else {
////      Slvs_Constraint aCoincConstr = Slvs_MakeConstraint(
////          ++myConstrMaxID, myID, SLVS_C_POINTS_COINCIDENT, myWorkplane.h,
////          0, aPtArc, aPtFillet, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////      myConstraints.push_back(aCoincConstr);
////      aConstrList.push_back(aCoincConstr.h);
////      Slvs_Constraint aTangency = Slvs_MakeConstraint(
////          ++myConstrMaxID, myID, aTangentType, myWorkplane.h,
////          0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aFilletEnt[2], aFilletEnt[indEnt]);
////      aTangency.other = (isArcInversed ? 1-indEnt : indEnt);
////      aTangency.other2 = aTangentType == SLVS_C_CURVE_CURVE_TANGENT ? aBaseCoincInd[indEnt] : 0;
////      myConstraints.push_back(aTangency);
////      aConstrList.push_back(aTangency.h);
////    }
////  }
////
////  // Additional constraint for fillet diameter
////  double aRadius = 0.0;  // scalar value of the constraint
////  AttributeDoublePtr aDistAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
////      aConstrData->attribute(SketchPlugin_Constraint::VALUE()));
////  aRadius = aDistAttr->value();
////  if (isExists) {
////    myConstraints[aCurConstrPos].entityA = aFilletEnt[2];
////    myConstraints[aCurConstrPos].valA = aRadius * 2.0;
////    aCMapIter++;
////  } else {
////    Slvs_Constraint aDiamConstr = Slvs_MakeConstraint(
////        ++myConstrMaxID, myID, SLVS_C_DIAMETER, myWorkplane.h, aRadius * 2.0,
////        SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aFilletEnt[2], SLVS_E_UNKNOWN);
////    myConstraints.push_back(aDiamConstr);
////    aConstrList.push_back(aDiamConstr.h);
////
////    myConstraintMap[theConstraint] = aConstrList;
////  }
////
////  // Additional temporary constraints for base objects to be fixed
////  for (unsigned int indAttr = 0; indAttr < 2; indAttr++) {
////    if (!aBaseFeature[indAttr]) {
////      AttributeRefAttrPtr aConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
////          aConstrData->attribute(aConstraintAttributes[indAttr]));
////      addTemporaryConstraintWhereDragged(aConstrAttr->attr());
////      continue;
////    }
////    std::list<AttributePtr> anAttributes =
////        aBaseFeature[indAttr]->data()->attributes(GeomDataAPI_Point2D::type());
////    std::list<AttributePtr>::iterator anIt = anAttributes.begin();
////    for ( ; anIt != anAttributes.end(); anIt++) {
////      // Arc should be fixed by center and start points only (to avoid "conflicting constraints" message)
////      if (aBaseFeature[indAttr]->getKind() == SketchPlugin_Arc::ID() &&
////          (*anIt)->id() == SketchPlugin_Arc::END_ID())
////        continue;
////      addTemporaryConstraintWhereDragged(*anIt);
////    }
////  }
////  return true;
////}
////
////// ============================================================================
//////  Function: changeEntity
//////  Class:    SketchSolver_Group
//////  Purpose:  create/update the element affected by any constraint
////// ============================================================================
////Slvs_hEntity SketchSolver_Group::changeEntity(
////    std::shared_ptr<ModelAPI_Attribute> theEntity)
////{
////  // If the entity is already in the group, try to find it
////  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator aEntIter =
////      myEntityAttrMap.find(theEntity);
////  int aEntPos;
////  std::vector<Slvs_Param>::const_iterator aParamIter;  // looks at first parameter of already existent entity or at the end of vector otherwise
////  if (aEntIter == myEntityAttrMap.end())  // no such entity => should be created
////    aParamIter = myParams.end();
////  else {  // the entity already exists
////    aEntPos = Search(aEntIter->second, myEntities);
////    int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
////    aParamIter = myParams.begin() + aParamPos;
////  }
////  const bool isEntExists = (aEntIter != myEntityAttrMap.end());  // defines that the entity already exists
////  const bool isNeedToSolve = myNeedToSolve;
////  myNeedToSolve = false;
////
////  if (isEntExists) {
////    // Verify that the entity is not used by "Rigid" constraint.
////    // If it is used, the object should not move.
////    std::vector<std::set<Slvs_hEntity> >::iterator aCoincIter = myCoincidentPoints.begin();
////    for (; aCoincIter != myCoincidentPoints.end(); aCoincIter++)
////      if (aCoincIter->find(aEntIter->second) != aCoincIter->end())
////        break;
////    std::set<Slvs_hEntity> aCoincident;
////    if (aCoincIter != myCoincidentPoints.end()) {
////      aCoincident = *aCoincIter;
////      aCoincident.erase(aEntIter->second);
////
////      std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
////      for (; aConstrIter != myConstraints.end(); aConstrIter++)
////        if (aConstrIter->type == SLVS_C_WHERE_DRAGGED &&
////            aCoincident.find(aConstrIter->ptA) != aCoincident.end()) {
////          myNeedToSolve = true;
////          return aEntIter->second;
////        }
////    }
////  }
////
////  // Look over supported types of entities
////  Slvs_Entity aNewEntity;
////  aNewEntity.h = SLVS_E_UNKNOWN;
////
////  // Point in 3D
////  std::shared_ptr<GeomDataAPI_Point> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point>(
////      theEntity);
////  if (aPoint) {
////    Slvs_hParam aX = changeParameter(aPoint->x(), aParamIter);
////    Slvs_hParam aY = changeParameter(aPoint->y(), aParamIter);
////    Slvs_hParam aZ = changeParameter(aPoint->z(), aParamIter);
////    if (!isEntExists) // New entity
////      aNewEntity = Slvs_MakePoint3d(++myEntityMaxID, myID, aX, aY, aZ);
////  } else {
////    // All entities except 3D points are created on workplane. So, if there is no workplane yet, then error
////    if (myWorkplane.h == SLVS_E_UNKNOWN)
////      return SLVS_E_UNKNOWN;
////
////    // Point in 2D
////    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
////        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
////    if (aPoint2D) {
////      Slvs_hParam aU = changeParameter(aPoint2D->x(), aParamIter);
////      Slvs_hParam aV = changeParameter(aPoint2D->y(), aParamIter);
////      if (!isEntExists) // New entity
////        aNewEntity = Slvs_MakePoint2d(++myEntityMaxID, myID, myWorkplane.h, aU, aV);
////    } else {
////      // Scalar value (used for the distance entities)
////      AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
////      if (aScalar) {
////        Slvs_hParam aValue = changeParameter(aScalar->value(), aParamIter);
////        if (!isEntExists) // New entity
////          aNewEntity = Slvs_MakeDistance(++myEntityMaxID, myID, myWorkplane.h, aValue);
////      }
////    }
////  }
////  /// \todo Other types of entities
////
////  Slvs_hEntity aResult = SLVS_E_UNKNOWN; // Unsupported or wrong entity type
////
////  if (isEntExists) {
////    myNeedToSolve = myNeedToSolve || isNeedToSolve;
////    aResult = aEntIter->second;
////  } else if (aNewEntity.h != SLVS_E_UNKNOWN) {
////    myEntities.push_back(aNewEntity);
////    myEntOfConstr.push_back(false);
////    myEntityAttrMap[theEntity] = aNewEntity.h;
////    aResult = aNewEntity.h;
////  }
////
////  // If the attribute was changed by the user, we need to fix it before solving
////  if (myNeedToSolve && theEntity->isImmutable())
////    addTemporaryConstraintWhereDragged(theEntity, false);
////
////  return aResult;
////}
////
////// ============================================================================
//////  Function: changeEntity
//////  Class:    SketchSolver_Group
//////  Purpose:  create/update the element defined by the feature affected by any constraint
////// ============================================================================
////Slvs_hEntity SketchSolver_Group::changeEntityFeature(FeaturePtr theEntity)
////{
////  if (!theEntity->data()->isValid())
////    return SLVS_E_UNKNOWN;
////  // If the entity is already in the group, try to find it
////  std::map<FeaturePtr, Slvs_hEntity>::const_iterator aEntIter = myEntityFeatMap.find(theEntity);
////  // defines that the entity already exists
////  const bool isEntExists = (myEntityFeatMap.find(theEntity) != myEntityFeatMap.end());
////  
////  Slvs_Entity aNewEntity;
////  aNewEntity.h = SLVS_E_UNKNOWN;
////
////  // SketchPlugin features
////  std::shared_ptr<SketchPlugin_Feature> aFeature = std::dynamic_pointer_cast<
////      SketchPlugin_Feature>(theEntity);
////  if (aFeature) {  // Verify the feature by its kind
////    const std::string& aFeatureKind = aFeature->getKind();
////    AttributePtr anAttribute;
////
////    // Line
////    if (aFeatureKind.compare(SketchPlugin_Line::ID()) == 0) {
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Line::START_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aStart = changeEntity(anAttribute);
////
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Line::END_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aEnd = changeEntity(anAttribute);
////
////      if (!isEntExists) // New entity
////        aNewEntity = Slvs_MakeLineSegment(++myEntityMaxID, myID, myWorkplane.h, aStart, aEnd);
////    }
////    // Circle
////    else if (aFeatureKind.compare(SketchPlugin_Circle::ID()) == 0) {
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aCenter = changeEntity(anAttribute);
////
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Circle::RADIUS_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aRadius = changeEntity(anAttribute);
////
////      if (!isEntExists) // New entity
////        aNewEntity = Slvs_MakeCircle(++myEntityMaxID, myID, myWorkplane.h, aCenter,
////                                     myWorkplane.normal, aRadius);
////    }
////    // Arc
////    else if (aFeatureKind.compare(SketchPlugin_Arc::ID()) == 0) {
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aCenter = changeEntity(anAttribute);
////
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Arc::START_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aStart = changeEntity(anAttribute);
////
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Arc::END_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aEnd = changeEntity(anAttribute);
////
////      if (!isEntExists)
////        aNewEntity = Slvs_MakeArcOfCircle(++myEntityMaxID, myID, myWorkplane.h,
////                                          myWorkplane.normal, aCenter, aStart, aEnd);
////    }
////    // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
////    else if (aFeatureKind.compare(SketchPlugin_Point::ID()) == 0) {
////      anAttribute = aFeature->data()->attribute(SketchPlugin_Point::COORD_ID());
////      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
////      Slvs_hEntity aPoint = changeEntity(anAttribute);
////
////      if (isEntExists)
////        return aEntIter->second;
////
////      // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
////      myEntityFeatMap[theEntity] = aPoint;
////      myNeedToSolve = true;
////      return aPoint;
////    }
////  }
////  /// \todo Other types of features
////
////  if (isEntExists)
////    return aEntIter->second;
////
////  if (aNewEntity.h != SLVS_E_UNKNOWN) {
////    myEntities.push_back(aNewEntity);
////    myEntOfConstr.push_back(false);
////    myEntityFeatMap[theEntity] = aNewEntity.h;
////    myNeedToSolve = true;
////    return aNewEntity.h;
////  }
////
////  // Unsupported or wrong entity type
////  return SLVS_E_UNKNOWN;
////}

// ============================================================================
//  Function: addWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  create workplane for the group
// ============================================================================
bool SketchSolver_Group::addWorkplane(CompositeFeaturePtr theSketch)
{
  if (myWorkplaneID != SLVS_E_UNKNOWN || theSketch->getKind() != SketchPlugin_Sketch::ID())
    return false;  // the workplane already exists or the function parameter is not Sketch

  mySketch = theSketch;
  updateWorkplane();
  return true;
}

// ============================================================================
//  Function: updateWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  update parameters of workplane
// ============================================================================
bool SketchSolver_Group::updateWorkplane()
{
  if (!myStorage) // Create storage if not exists
    myStorage = StoragePtr(new SketchSolver_Storage);
  SketchSolver_Builder* aBuilder = SketchSolver_Builder::getInstance();

  std::vector<Slvs_Entity> anEntities;
  std::vector<Slvs_Param> aParams;
  if (!aBuilder->createWorkplane(mySketch, anEntities, aParams))
    return false;

  if (myWorkplaneID == SLVS_E_UNKNOWN) {
    myWorkplaneID = anEntities.back().h;
    // Add new workplane elements
    std::vector<Slvs_Param>::iterator aParIter = aParams.begin();
    for (; aParIter != aParams.end(); aParIter++) {
      aParIter->h = SLVS_E_UNKNOWN; // the ID should be generated by storage
      aParIter->group = myID;
      aParIter->h = myStorage->addParameter(*aParIter);
    }
    std::vector<Slvs_Entity>::iterator anEntIter = anEntities.begin();
    for (; anEntIter != anEntities.end(); anEntIter++) {
      anEntIter->h = SLVS_E_UNKNOWN; // the ID should be generated by storage
      anEntIter->group = myID;
      anEntIter->wrkpl = myWorkplaneID;
      for (int i = 0; i < 4; i++)
        if (anEntIter->param[i] != SLVS_E_UNKNOWN)
          anEntIter->param[i] = aParams[anEntIter->param[i]-1].h;
      for (int i = 0; i < 4; i++)
        if (anEntIter->point[i] != SLVS_E_UNKNOWN)
          anEntIter->point[i] = anEntities[anEntIter->point[i]-1].h;
      anEntIter->h = myStorage->addEntity(*anEntIter);
    }
  } else {
    // Update existent workplane
    const Slvs_Entity& aWP = myStorage->getEntity(myWorkplaneID);
    const Slvs_Entity& anOrigin = myStorage->getEntity(aWP.point[0]);
    const Slvs_Entity& aNormal = myStorage->getEntity(aWP.normal);
    // Get parameters and update them
    Slvs_hParam aWPParams[7] = {
        anOrigin.param[0], anOrigin.param[1], anOrigin.param[2],
        aNormal.param[0], aNormal.param[1], aNormal.param[2], aNormal.param[3]
      };
    std::vector<Slvs_Param>::iterator aParIter = aParams.begin();
    for (int i = 0; aParIter != aParams.end(); aParIter++, i++) {
      Slvs_Param aParam = myStorage->getParameter(aWPParams[i]);
      aParam.val = aParIter->val;
      myStorage->updateParameter(aParam);
    }
  }
  return myWorkplaneID > 0;
}

////// ============================================================================
//////  Function: changeParameter
//////  Class:    SketchSolver_Group
//////  Purpose:  create/update value of parameter
////// ============================================================================
////Slvs_hParam SketchSolver_Group::changeParameter(
////    const double& theParam, std::vector<Slvs_Param>::const_iterator& thePrmIter)
////{
////  if (thePrmIter != myParams.end()) {  // Parameter should be updated
////    int aParamPos = thePrmIter - myParams.begin();
////    if (fabs(thePrmIter->val - theParam) > tolerance) {
////      myNeedToSolve = true;  // parameter is changed, need to resolve constraints
////      myParams[aParamPos].val = theParam;
////    }
////    thePrmIter++;
////    return myParams[aParamPos].h;
////  }
////
////  // Newly created parameter
////  Slvs_Param aParam = Slvs_MakeParam(++myParamMaxID, myID, theParam);
////  myParams.push_back(aParam);
////  myNeedToSolve = true;
////  // The list of parameters is changed, move iterator to the end of the list to avoid problems
////  thePrmIter = myParams.end();
////  return aParam.h;
////}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_Group
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
bool SketchSolver_Group::resolveConstraints()
{
  bool aResolved = false;
  if (myStorage->isNeedToResolve() && !isEmpty()) {
    myConstrSolver.setGroupID(myID);
    myStorage->initializeSolver(myConstrSolver);

    int aResult = myConstrSolver.solve();
    if (aResult == SLVS_RESULT_OKAY) {  // solution succeeded, store results into correspondent attributes
      myFeatureStorage->blockEvents(true);
      ConstraintConstraintMap::iterator aConstrIter = myConstraints.begin();
      for (; aConstrIter != myConstraints.end(); aConstrIter++)
        aConstrIter->second->refresh();
      myFeatureStorage->blockEvents(false);
    } else if (!myConstraints.empty())
      Events_Error::send(SketchSolver_Error::CONSTRAINTS(), this);

    aResolved = true;
  }
  removeTemporaryConstraints();
  myStorage->setNeedToResolve(false);
  return aResolved;
}

// ============================================================================
//  Function: mergeGroups
//  Class:    SketchSolver_Group
//  Purpose:  append specified group to the current group
// ============================================================================
void SketchSolver_Group::mergeGroups(const SketchSolver_Group& theGroup)
{
  // If specified group is empty, no need to merge
  if (theGroup.isEmpty())
    return;
  if (!myFeatureStorage)
    myFeatureStorage = FeatureStoragePtr(new SketchSolver_FeatureStorage);

  ConstraintConstraintMap::const_iterator aConstrIter = theGroup.myConstraints.begin();
  for (; aConstrIter != theGroup.myConstraints.end(); aConstrIter++)
    changeConstraint(aConstrIter->first);
}

// ============================================================================
//  Function: splitGroup
//  Class:    SketchSolver_Group
//  Purpose:  divide the group into several subgroups
// ============================================================================
void SketchSolver_Group::splitGroup(std::vector<SketchSolver_Group*>& theCuts)
{
  // Obtain constraints, which should be separated
  FeatureStoragePtr aNewFeatStorage(new SketchSolver_FeatureStorage);
  std::vector<ConstraintPtr> anUnusedConstraints;
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for ( ; aCIter != myConstraints.end(); aCIter++) {
    std::list<ConstraintPtr> aBaseConstraints = aCIter->second->constraints();
    std::list<ConstraintPtr>::iterator anIter = aBaseConstraints.begin();
    for (; anIter != aBaseConstraints.end(); anIter++)
      if (aNewFeatStorage->isInteract(*anIter)) {
        aNewFeatStorage->changeConstraint(*anIter);
      } else
        anUnusedConstraints.push_back(*anIter);
  }

  std::vector<SketchSolver_Group*>::iterator aCutsIter;
  std::vector<ConstraintPtr>::iterator aUnuseIt = anUnusedConstraints.begin();
  for ( ; aUnuseIt != anUnusedConstraints.end(); aUnuseIt++) {
    // Remove unused constraints
    removeConstraint(*aUnuseIt);
    // Try to append constraint to already existent group
    for (aCutsIter = theCuts.begin(); aCutsIter != theCuts.end(); aCutsIter++)
      if ((*aCutsIter)->isInteract(*aUnuseIt)) {
        (*aCutsIter)->changeConstraint(*aUnuseIt);
        break;
      }
    if (aCutsIter == theCuts.end()) {
      // Add new group
      SketchSolver_Group* aGroup = new SketchSolver_Group(mySketch);
      aGroup->changeConstraint(*aUnuseIt);
      theCuts.push_back(aGroup);
    }
  }
}

// ============================================================================
//  Function: isConsistent
//  Class:    SketchSolver_Group
//  Purpose:  search removed entities and constraints
// ============================================================================
bool SketchSolver_Group::isConsistent()
{
  if (!myFeatureStorage) // no one constraint is initialized yet
    return true;

  bool aResult = myFeatureStorage->isConsistent();
  if (!aResult) {
    // remove invalid entities
    ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
    while (aCIter != myConstraints.end()) {
      std::list<ConstraintPtr> aConstraints = aCIter->second->constraints();
      std::list<ConstraintPtr>::iterator anIt = aConstraints.begin();
      for (; anIt != aConstraints.end(); anIt++)
        if (!(*anIt)->data() || !(*anIt)->data()->isValid())
          if (aCIter->second->remove(*anIt)) {
            // the constraint is fully removed, detach it from the list
            ConstraintConstraintMap::iterator aTmpIt = aCIter++;
            myFeatureStorage->removeConstraint(aTmpIt->first);
            myConstraints.erase(aTmpIt);
            break;
          }
      if (anIt == aConstraints.end())
        aCIter++;
    }
  }
  return aResult;
}

////// ============================================================================
//////  Function: updateAttribute
//////  Class:    SketchSolver_Group
//////  Purpose:  update features of sketch after resolving constraints
////// ============================================================================
////bool SketchSolver_Group::updateAttribute(
////    std::shared_ptr<ModelAPI_Attribute> theAttribute, const Slvs_hEntity& theEntityID)
////{
////  // Search the position of the first parameter of the entity
////  int anEntPos = Search(theEntityID, myEntities);
////  int aFirstParamPos = Search(myEntities[anEntPos].param[0], myParams);
////
////  // Look over supported types of entities
////
////  // Point in 3D
////  std::shared_ptr<GeomDataAPI_Point> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point>(
////      theAttribute);
////  if (aPoint) {
////    if (fabs(aPoint->x() - myParams[aFirstParamPos].val) > tolerance
////        || fabs(aPoint->y() - myParams[aFirstParamPos + 1].val) > tolerance
////        || fabs(aPoint->z() - myParams[aFirstParamPos + 2].val) > tolerance) {
////      aPoint->setValue(myParams[aFirstParamPos].val, myParams[aFirstParamPos + 1].val,
////                       myParams[aFirstParamPos + 2].val);
////      return true;
////    }
////    return false;
////  }
////
////  // Point in 2D
////  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
////      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
////  if (aPoint2D) {
////    if (fabs(aPoint2D->x() - myParams[aFirstParamPos].val) > tolerance
////        || fabs(aPoint2D->y() - myParams[aFirstParamPos + 1].val) > tolerance) {
////      aPoint2D->setValue(myParams[aFirstParamPos].val, myParams[aFirstParamPos + 1].val);
////      return true;
////    }
////    return false;
////  }
////
////  // Scalar value
////  AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
////  if (aScalar) {
////    if (fabs(aScalar->value() - myParams[aFirstParamPos].val) > tolerance) {
////      aScalar->setValue(myParams[aFirstParamPos].val);
////      return true;
////    }
////    return false;
////  }
////
////  /// \todo Support other types of entities
////  return false;
////}
////
////// ============================================================================
//////  Function: updateEntityIfPossible
//////  Class:    SketchSolver_Group
//////  Purpose:  search the entity in this group and update it
////// ============================================================================
////void SketchSolver_Group::updateEntityIfPossible(
////    std::shared_ptr<ModelAPI_Attribute> theEntity)
////{
////  if (myEntityAttrMap.find(theEntity) != myEntityAttrMap.end()) {
////    // If the attribute is a point and it is changed (the group needs to rebuild),
////    // probably user has dragged this point into this position,
////    // so it is necessary to add constraint which will guarantee the point will not change
////
////    // Store myNeedToSolve flag to verify the entity is really changed
////    bool aNeedToSolveCopy = myNeedToSolve;
////    myNeedToSolve = false;
////
////    changeEntity(theEntity);
////
////    if (myNeedToSolve)  // the entity is changed
////    {
////      // Verify the entity is a point and add temporary constraint of permanency
////      std::shared_ptr<GeomDataAPI_Point> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point>(
////          theEntity);
////      std::shared_ptr<GeomDataAPI_Point2D> aPoint2D = std::dynamic_pointer_cast<
////          GeomDataAPI_Point2D>(theEntity);
////      if (aPoint || aPoint2D)
////        addTemporaryConstraintWhereDragged(theEntity);
////    }
////
////    // Restore flag of changes
////    myNeedToSolve = myNeedToSolve || aNeedToSolveCopy;
////
////    if (myNeedToSolve)
////      updateRelatedConstraints(theEntity);
////  }
////}
////
////// ============================================================================
//////  Function: addTemporaryConstraintWhereDragged
//////  Class:    SketchSolver_Group
//////  Purpose:  add transient constraint SLVS_C_WHERE_DRAGGED for the entity, 
//////            which was moved by user
////// ============================================================================
////void SketchSolver_Group::addTemporaryConstraintWhereDragged(
////    std::shared_ptr<ModelAPI_Attribute> theEntity,
////    bool theAllowToFit)
////{
////  // Find identifier of the entity
////  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator anEntIter =
////      myEntityAttrMap.find(theEntity);
////  if (anEntIter == myEntityAttrMap.end())
////    return;
////
////  // Get identifiers of all dragged points
////  std::set<Slvs_hEntity> aDraggedPntID;
////  aDraggedPntID.insert(myTempPointWDrgdID);
////  std::list<Slvs_hConstraint>::const_iterator aTmpCoIter = myTempConstraints.begin();
////  for (; aTmpCoIter != myTempConstraints.end(); aTmpCoIter++) {
////    unsigned int aConstrPos = Search(*aTmpCoIter, myConstraints);
////    if (aConstrPos < myConstraints.size())
////      aDraggedPntID.insert(myConstraints[aConstrPos].ptA);
////  }
////  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
////  for (; aConstrIter != myConstraints.end(); aConstrIter++)
////    if (aConstrIter->type == SLVS_C_WHERE_DRAGGED)
////      aDraggedPntID.insert(aConstrIter->ptA);
////  // Find whether there is a point coincident with theEntity, which already has SLVS_C_WHERE_DRAGGED
////  std::vector<std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
////  for (; aCoPtIter != myCoincidentPoints.end(); aCoPtIter++) {
////    if (aCoPtIter->find(anEntIter->second) == aCoPtIter->end())
////      continue;  // the entity was not found in current set
////
////    // Find one of already created SLVS_C_WHERE_DRAGGED constraints in current set of coincident points
////    std::set<Slvs_hEntity>::const_iterator aDrgIter = aDraggedPntID.begin();
////    for (; aDrgIter != aDraggedPntID.end(); aDrgIter++)
////      if (aCoPtIter->find(*aDrgIter) != aCoPtIter->end())
////        return;  // the SLVS_C_WHERE_DRAGGED constraint already exists
////  }
////  if (aDraggedPntID.find(anEntIter->second) != aDraggedPntID.end())
////    return;
////
////  // If this is a first dragged point, its parameters should be placed 
////  // into Slvs_System::dragged field to avoid system inconsistense
////  if (myTempPointWhereDragged.empty() && theAllowToFit) {
////    int anEntPos = Search(anEntIter->second, myEntities);
////    Slvs_hParam* aDraggedParam = myEntities[anEntPos].param;
////    for (int i = 0; i < 4; i++, aDraggedParam++)
////      if (*aDraggedParam != 0)
////        myTempPointWhereDragged.push_back(*aDraggedParam);
////    myTempPointWDrgdID = myEntities[anEntPos].h;
////    return;
////  }
////
////  // Create additional SLVS_C_WHERE_DRAGGED constraint if myTempPointWhereDragged field is not empty
////  Slvs_Constraint aWDConstr = Slvs_MakeConstraint(++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED,
////                                                  myWorkplane.h, 0.0, anEntIter->second, 0, 0, 0);
////  myConstraints.push_back(aWDConstr);
////  myTempConstraints.push_back(aWDConstr.h);
////}

// ============================================================================
//  Function: removeTemporaryConstraints
//  Class:    SketchSolver_Group
//  Purpose:  remove all transient SLVS_C_WHERE_DRAGGED constraints after
//            resolving the set of constraints
// ============================================================================
void SketchSolver_Group::removeTemporaryConstraints()
{
  myTempConstraints.clear();
  // Clean lists of removed entities in the storage
  std::set<Slvs_hParam> aRemPar;
  std::set<Slvs_hEntity> aRemEnt;
  std::set<Slvs_hConstraint> aRemCon;
  myStorage->getRemoved(aRemPar, aRemEnt, aRemCon);
}

// ============================================================================
//  Function: removeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  remove constraint and all unused entities
// ============================================================================
void SketchSolver_Group::removeConstraint(ConstraintPtr theConstraint)
{
  myFeatureStorage->removeConstraint(theConstraint);
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (aCIter->second->hasConstraint(theConstraint)) {
      if (!aCIter->second->remove(theConstraint)) // the constraint is not fully removed
        aCIter = myConstraints.end();
      break;
    }
  if (aCIter != myConstraints.end())
    myConstraints.erase(aCIter);
}

////// ============================================================================
//////  Function: removeEntitiesById
//////  Class:    SketchSolver_Group
//////  Purpose:  Removes specified entities and their parameters
////// ============================================================================
////void SketchSolver_Group::removeEntitiesById(const std::set<Slvs_hEntity>& theEntities)
////{
////  std::set<Slvs_hEntity>::const_reverse_iterator aRemIter = theEntities.rbegin();
////  for (; aRemIter != theEntities.rend(); aRemIter++) {
////    unsigned int anEntPos = Search(*aRemIter, myEntities);
////    if (anEntPos >= myEntities.size())
////      continue;
////    if (myEntities[anEntPos].param[0] != 0) {
////      unsigned int aParamPos = Search(myEntities[anEntPos].param[0], myParams);
////      if (aParamPos >= myParams.size())
////        continue;
////      int aNbParams = 0;
////      while (myEntities[anEntPos].param[aNbParams] != 0)
////        aNbParams++;
////      if (myEntities[anEntPos].param[aNbParams - 1] == myParamMaxID)
////        myParamMaxID -= aNbParams;
////      myParams.erase(myParams.begin() + aParamPos, myParams.begin() + aParamPos + aNbParams);
////      if (*aRemIter == myEntityMaxID)
////        myEntityMaxID--;
////    }
////    myEntities.erase(myEntities.begin() + anEntPos);
////    myEntOfConstr.erase(myEntOfConstr.begin() + anEntPos);
////
////    // Remove entity's ID from the lists of conincident points
////    std::vector<std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
////    for (; aCoPtIter != myCoincidentPoints.end(); aCoPtIter++)
////      aCoPtIter->erase(*aRemIter);
////  }
////}
////
////// ============================================================================
//////  Function: addCoincidentPoints
//////  Class:    SketchSolver_Group
//////  Purpose:  add coincident point the appropriate list of such points
////// ============================================================================
////bool SketchSolver_Group::addCoincidentPoints(const Slvs_hEntity& thePoint1,
////                                                       const Slvs_hEntity& thePoint2)
////{
////  std::vector<std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
////  std::vector<std::set<Slvs_hEntity> >::iterator aFirstFound = myCoincidentPoints.end();
////  while (aCoPtIter != myCoincidentPoints.end()) {
////    bool isFound[2] = {  // indicate which point ID was already in coincidence constraint
////        aCoPtIter->find(thePoint1) != aCoPtIter->end(), aCoPtIter->find(thePoint2)
////            != aCoPtIter->end(), };
////    if (isFound[0] && isFound[1])  // points are already connected by coincidence constraints => no need additional one
////      return false;
////    if ((isFound[0] && !isFound[1]) || (!isFound[0] && isFound[1])) {
////      if (aFirstFound != myCoincidentPoints.end()) {  // there are two groups of coincident points connected by created constraint => merge them
////        int aFirstFoundShift = aFirstFound - myCoincidentPoints.begin();
////        int aCurrentShift = aCoPtIter - myCoincidentPoints.begin();
////        aFirstFound->insert(aCoPtIter->begin(), aCoPtIter->end());
////        myCoincidentPoints.erase(aCoPtIter);
////        aFirstFound = myCoincidentPoints.begin() + aFirstFoundShift;
////        aCoPtIter = myCoincidentPoints.begin() + aCurrentShift;
////        continue;
////      } else {
////        aCoPtIter->insert(isFound[0] ? thePoint2 : thePoint1);
////        aFirstFound = aCoPtIter;
////      }
////    }
////    aCoPtIter++;
////  }
////  // No points were found, need to create new set
////  if (aFirstFound == myCoincidentPoints.end()) {
////    std::set<Slvs_hEntity> aNewSet;
////    aNewSet.insert(thePoint1);
////    aNewSet.insert(thePoint2);
////    myCoincidentPoints.push_back(aNewSet);
////  }
////
////  return true;
////}
////
////// ============================================================================
//////  Function: updateRelatedConstraints
//////  Class:    SketchSolver_Group
//////  Purpose:  emit the signal to update constraints
////// ============================================================================
////void SketchSolver_Group::updateRelatedConstraints(
////    std::shared_ptr<ModelAPI_Attribute> theEntity) const
////{
////  ConstraintMap::const_iterator aConstrIter = myConstraintMap.begin();
////  for (; aConstrIter != myConstraintMap.end(); aConstrIter++) {
////    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttributes = aConstrIter->first->data()
////        ->attributes(std::string());
////
////    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrIter = anAttributes.begin();
////    for (; anAttrIter != anAttributes.end(); anAttrIter++) {
////      bool isUpd = (*anAttrIter == theEntity);
////      std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = std::dynamic_pointer_cast<
////          ModelAPI_AttributeRefAttr>(*anAttrIter);
////      if (aRefAttr && !aRefAttr->isObject() && aRefAttr->attr() == theEntity)
////        isUpd = true;
////
////      if (isUpd) {
////        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
////        ModelAPI_EventCreator::get()->sendUpdated(aConstrIter->first, anEvent);
////        break;
////      }
////    }
////  }
////}
////
////void SketchSolver_Group::updateRelatedConstraintsFeature(
////    std::shared_ptr<ModelAPI_Feature> theFeature) const
////{
////  ConstraintMap::const_iterator aConstrIter = myConstraintMap.begin();
////  for (; aConstrIter != myConstraintMap.end(); aConstrIter++) {
////    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttributes = aConstrIter->first->data()
////        ->attributes(std::string());
////
////    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrIter = anAttributes.begin();
////    for (; anAttrIter != anAttributes.end(); anAttrIter++) {
////      std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = std::dynamic_pointer_cast<
////          ModelAPI_AttributeRefAttr>(*anAttrIter);
////      if (aRefAttr && aRefAttr->isObject() && aRefAttr->object() == theFeature) {
////        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
////        ModelAPI_EventCreator::get()->sendUpdated(aConstrIter->first, anEvent);
////        break;
////      }
////    }
////  }
////}
////
////// ============================================================================
//////  Function: updateFilletConstraints
//////  Class:    SketchSolver_Group
//////  Purpose:  change fillet arc to be less than 180 degree
////// ============================================================================
////void SketchSolver_Group::updateFilletConstraints()
////{
////  ConstraintMap::const_iterator aConstrIter = myConstraintMap.begin();
////  for (; aConstrIter != myConstraintMap.end(); aConstrIter++)
////    if (aConstrIter->first->getKind() == SketchPlugin_ConstraintFillet::ID()) {
////      AttributeRefListPtr aFilletRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
////          aConstrIter->first->data()->attribute(SketchPlugin_ConstraintFillet::ENTITY_C()));
////      if (!aFilletRefList)
////        return;
////      ObjectPtr anArcObj = aFilletRefList->object(2);
////      std::shared_ptr<GeomDataAPI_Point2D> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////          anArcObj->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
////      std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////          anArcObj->data()->attribute(SketchPlugin_Arc::START_ID()));
////      std::shared_ptr<GeomDataAPI_Point2D> aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
////          anArcObj->data()->attribute(SketchPlugin_Arc::END_ID()));
////      double aCosA = aStart->x() - aCenter->x();
////      double aSinA = aStart->y() - aCenter->y();
////      double aCosB = aEnd->x() - aCenter->x();
////      double aSinB = aEnd->y() - aCenter->y();
////      if (aCosA * aSinB - aSinA * aCosB <= 0.0) {
////        anArcObj->data()->blockSendAttributeUpdated(true);
////        double x = aStart->x();
////        double y = aStart->y();
////        aStart->setValue(aEnd->x(), aEnd->y());
////        aEnd->setValue(x, y);
////        // Update constraint data
////        changeFilletConstraint(aConstrIter->first);
////        anArcObj->data()->blockSendAttributeUpdated(false);
////      }
////    }
////}
////
////// ============================================================================
//////  Function: makeMirrorEntity
//////  Class:    SketchSolver_Group
//////  Purpose:  change entities parameters to make them symmetric relating to the mirror line
////// ============================================================================
////void SketchSolver_Group::makeMirrorEntity(const Slvs_hEntity& theBase,
////                                                    const Slvs_hEntity& theMirror,
////                                                    const Slvs_hEntity& theMirrorLine)
////{
////  Slvs_Entity aBase = myEntities[Search(theBase, myEntities)];
////  Slvs_Entity aMirror = myEntities[Search(theMirror, myEntities)];
////  int i = 0;
////  while (aBase.point[i] != 0 && aMirror.point[i] != 0) {
////    makeMirrorEntity(aBase.point[i], aMirror.point[i], theMirrorLine);
////    i++;
////  }
////  if (aBase.param[0] != 0 && aMirror.param[0] != 0) { // this is a point, copy it
////    Slvs_Entity aMirrorLine = myEntities[Search(theMirrorLine, myEntities)];
////    std::shared_ptr<GeomAPI_XY> aLinePoints[2];
////    for (i = 0; i < 2; i++) {
////      Slvs_Entity aPoint = myEntities[Search(aMirrorLine.point[i], myEntities)];
////      int aParamPos = Search(aPoint.param[0], myParams);
////      aLinePoints[i] = std::shared_ptr<GeomAPI_XY>(
////          new GeomAPI_XY(myParams[aParamPos].val, myParams[1+aParamPos].val));
////    }
////    int aBaseParamPos = Search(aBase.param[0], myParams);
////    int aMirrorParamPos = Search(aMirror.param[0], myParams);
////    std::shared_ptr<GeomAPI_XY> aPoint = std::shared_ptr<GeomAPI_XY>(
////        new GeomAPI_XY(myParams[aBaseParamPos].val, myParams[1+aBaseParamPos].val));
////
////    // direction of a mirror line
////    std::shared_ptr<GeomAPI_Dir2d> aDir = std::shared_ptr<GeomAPI_Dir2d>(
////        new GeomAPI_Dir2d(aLinePoints[1]->added(aLinePoints[0]->multiplied(-1.0))));
////    // orthogonal direction
////    aDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir->y(), -aDir->x()));
////
////    std::shared_ptr<GeomAPI_XY> aVec = std::shared_ptr<GeomAPI_XY>(
////        new GeomAPI_XY(aPoint->x() - aLinePoints[0]->x(), aPoint->y() - aLinePoints[0]->y()));
////    double aDist = aVec->dot(aDir->xy());
////    std::shared_ptr<GeomAPI_XY> aMirrorPoint = aPoint->added(aDir->xy()->multiplied(-2.0 * aDist));
////
////    myParams[aMirrorParamPos].val = aMirrorPoint->x();
////    myParams[1+aMirrorParamPos].val = aMirrorPoint->y();
////  }
////}
////
////// ============================================================================
//////  Function: calculateMiddlePoint
//////  Class:    SketchSolver_Group
//////  Purpose:  calculates middle point on line or arc
////// ============================================================================
////void SketchSolver_Group::calculateMiddlePoint(
////    const Slvs_hEntity& theEntity,
////    double& theX,
////    double& theY) const
////{
////  int anInd = Search(theEntity, myEntities);
////  if (myEntities[anInd].type == SLVS_E_LINE_SEGMENT) {
////    int aLineParams[2];
////    for (int i = 0; i < 2; i++) {
////      int aPtPos = Search(myEntities[anInd].point[i], myEntities);
////      aLineParams[i] = Search(myEntities[aPtPos].param[0], myParams);
////    }
////    theX = 0.5 * (myParams[aLineParams[0]].val + myParams[aLineParams[1]].val);
////    theY = 0.5 * (myParams[1 + aLineParams[0]].val + myParams[1 + aLineParams[1]].val);
////  } else if (myEntities[anInd].type == SLVS_E_ARC_OF_CIRCLE) {
////    double anArcPoint[3][2];
////    for (int i = 0; i < 3; i++) {
////      int aPtPos = Search(myEntities[anInd].point[i], myEntities);
////      int anArcParam = Search(myEntities[aPtPos].param[0], myParams);
////      anArcPoint[i][0] = myParams[anArcParam].val;
////      anArcPoint[i][1] = myParams[1 + anArcParam].val;
////    }
////    // project last point of arc on the arc
////    double x = anArcPoint[1][0] - anArcPoint[0][0];
////    double y = anArcPoint[1][1] - anArcPoint[0][1];
////    double aRad = sqrt(x*x + y*y);
////    x = anArcPoint[2][0] - anArcPoint[0][0];
////    y = anArcPoint[2][1] - anArcPoint[0][1];
////    double aNorm = sqrt(x*x + y*y);
////    if (aNorm >= tolerance) {
////      anArcPoint[2][0] = anArcPoint[0][0] + x * aRad / aNorm;
////      anArcPoint[2][1] = anArcPoint[0][1] + y * aRad / aNorm;
////    }
////
////    x = anArcPoint[1][0] + anArcPoint[2][0] - 2.0 * anArcPoint[0][0];
////    y = anArcPoint[1][1] + anArcPoint[2][1] - 2.0 * anArcPoint[0][1];
////    aNorm = sqrt(x*x + y*y);
////    if (aNorm >= tolerance) {
////      x *= aRad / aNorm;
////      y *= aRad / aNorm;
////    } else { // obtain orthogonal direction
////      x = 0.5 * (anArcPoint[2][1] - anArcPoint[1][1]);
////      y = -0.5 * (anArcPoint[2][0] - anArcPoint[1][0]);
////    }
////    theX = anArcPoint[0][0] + x;
////    theY = anArcPoint[0][1] + y;
////  }
////}


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
