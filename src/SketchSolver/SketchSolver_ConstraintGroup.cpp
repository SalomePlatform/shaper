// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintGroup.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintGroup.h"

#include <SketchSolver_Constraint.h>

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

/// Tolerance for value of parameters
const double tolerance = 1.e-10;

/**
 * Collects all sketch solver error' codes
 * as inline static functions
 */
 // TODO: Move this class into a separate file
class SketchSolver_Error
{
 public:
  /// The value parameter for the constraint
  inline static const std::string& CONSTRAINTS()
  {
    static const std::string MY_ERROR_VALUE("Conflicting constraints");
    return MY_ERROR_VALUE;
  }
  /// The entities need to have shared point, but they have not
  inline static const std::string& NO_COINCIDENT_POINTS()
  {
    static const std::string MY_ERROR_VALUE("Objects should have coincident point");
    return MY_ERROR_VALUE;
  }
};

/// This value is used to give unique index to the groups
static Slvs_hGroup myGroupIndexer = 0;

/** \brief Search the entity/parameter with specified ID in the list of elements
 *  \param[in] theEntityID unique ID of the element
 *  \param[in] theEntities list of elements
 *  \return position of the found element or -1 if the element is not found
 */
template<typename T>
static int Search(const uint32_t& theEntityID, const std::vector<T>& theEntities);

// ========================================================
// =========  SketchSolver_ConstraintGroup  ===============
// ========================================================

SketchSolver_ConstraintGroup::SketchSolver_ConstraintGroup(
    std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane)
    : myID(++myGroupIndexer),
      myParamMaxID(0),
      myEntityMaxID(0),
      myConstrMaxID(0),
      myConstraintMap(),
      myNeedToSolve(false),
      myConstrSolver()
{
  myParams.clear();
  myEntities.clear();
  myEntOfConstr.clear();
  myConstraints.clear();

  myTempConstraints.clear();
  myTempPointWhereDragged.clear();
  myTempPointWDrgdID = 0;

  // Initialize workplane
  myWorkplane.h = SLVS_E_UNKNOWN;
#ifndef NDEBUG
  assert(addWorkplane(theWorkplane));
#else
  addWorkplane(theWorkplane);
#endif
}

SketchSolver_ConstraintGroup::~SketchSolver_ConstraintGroup()
{
  myParams.clear();
  myEntities.clear();
  myEntOfConstr.clear();
  myConstraints.clear();
  myConstraintMap.clear();
  myTempConstraints.clear();
  myTempPointWhereDragged.clear();

  // If the group with maximal identifier is deleted, decrease the indexer
  if (myID == myGroupIndexer)
    myGroupIndexer--;
}

// ============================================================================
//  Function: isBaseWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verify the group is based on the given workplane
// ============================================================================
bool SketchSolver_ConstraintGroup::isBaseWorkplane(
    std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane) const
{
  return theWorkplane == mySketch;
}

// ============================================================================
//  Function: isInteract
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verify are there any entities in the group used by given constraint
// ============================================================================
bool SketchSolver_ConstraintGroup::isInteract(
    std::shared_ptr<SketchPlugin_Feature> theFeature) const
{
  // Check the group is empty
  if (isEmpty())
    return true;

  // Check if the feature is already in the group
  if (myEntityFeatMap.find(theFeature) != myEntityFeatMap.end())
    return true;
  std::shared_ptr<SketchPlugin_Constraint> aConstr =
      std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (aConstr && myConstraintMap.find(aConstr) != myConstraintMap.end())
    return true;

  // Go through the attributes and verify if some of them already in the group
  std::list<std::shared_ptr<ModelAPI_Attribute>> 
      anAttrList = theFeature->data()->attributes(std::string());
  std::list<std::shared_ptr<ModelAPI_Attribute>>::const_iterator
      anAttrIter = anAttrList.begin();
  for ( ; anAttrIter != anAttrList.end(); anAttrIter++) {
    AttributeRefListPtr aCAttrRefList = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anAttrIter);
    if (aCAttrRefList) {
      std::list<ObjectPtr> anObjList = aCAttrRefList->list();
      std::list<ObjectPtr>::iterator anIt = anObjList.begin();
      for ( ; anIt != anObjList.end(); anIt++) {
        FeaturePtr aFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
        if (aFeature && myEntityFeatMap.find(aFeature) != myEntityFeatMap.end())
          return true;
      }
      continue;
    }
    AttributeRefAttrPtr aCAttrRef =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIter);
    if (!aCAttrRef || !aCAttrRef->isObject()) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aCAttrRef ? aCAttrRef->attr() : *anAttrIter;
      if (myEntityAttrMap.find(anAttr) != myEntityAttrMap.end())
        return true;
    } else {
      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
          aCAttrRef->object());
      if (!aRC)
        continue;
      std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
      FeaturePtr aFeature = aDoc->feature(aRC);
      if (myEntityFeatMap.find(aFeature) != myEntityFeatMap.end())
        return true;
      // search attributes of a feature to be parameters of constraint
      std::list<std::shared_ptr<ModelAPI_Attribute> > aFeatAttrList =
          aFeature->data()->attributes(std::string());
      std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aFAIter = aFeatAttrList
          .begin();
      for (; aFAIter != aFeatAttrList.end(); aFAIter++)
        if (myEntityAttrMap.find(*aFAIter) != myEntityAttrMap.end())
          return true;
    }
  }

  // Entities did not found
  return false;
}

// ============================================================================
//  Function: checkConstraintConsistence
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  verifies and changes parameters of the constraint
// ============================================================================
void SketchSolver_ConstraintGroup::checkConstraintConsistence(Slvs_Constraint& theConstraint)
{
  if (theConstraint.type == SLVS_C_PT_LINE_DISTANCE) {
    // Get constraint parameters and check the sign of constraint value

    // point coordinates
    int aPtPos = Search(theConstraint.ptA, myEntities);
    int aPtParamPos = Search(myEntities[aPtPos].param[0], myParams);
    std::shared_ptr<GeomAPI_XY> aPoint(
        new GeomAPI_XY(myParams[aPtParamPos].val, myParams[aPtParamPos + 1].val));

    // line coordinates
    int aLnPos = Search(theConstraint.entityA, myEntities);
    aPtPos = Search(myEntities[aLnPos].point[0], myEntities);
    aPtParamPos = Search(myEntities[aPtPos].param[0], myParams);
    std::shared_ptr<GeomAPI_XY> aStart(
        new GeomAPI_XY(-myParams[aPtParamPos].val, -myParams[aPtParamPos + 1].val));
    aPtPos = Search(myEntities[aLnPos].point[1], myEntities);
    aPtParamPos = Search(myEntities[aPtPos].param[0], myParams);
    std::shared_ptr<GeomAPI_XY> aEnd(
        new GeomAPI_XY(myParams[aPtParamPos].val, myParams[aPtParamPos + 1].val));

    aEnd = aEnd->added(aStart);
    aPoint = aPoint->added(aStart);
    if (aPoint->cross(aEnd) * theConstraint.valA < 0.0)
      theConstraint.valA *= -1.0;
  }
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_ConstraintGroup::changeConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // There is no workplane yet, something wrong
  if (myWorkplane.h == SLVS_E_UNKNOWN)
    return false;

  if (theConstraint) {
    if (theConstraint->getKind() == SketchPlugin_ConstraintRigid::ID())
      return changeRigidConstraint(theConstraint);
    if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID())
      return changeMirrorConstraint(theConstraint);
    if (theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID())
      return changeFilletConstraint(theConstraint);
  }

  // Search this constraint in the current group to update it
  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
  std::vector<Slvs_Constraint>::iterator aConstrIter;
  if (aConstrMapIter != myConstraintMap.end()) {
    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
    aConstrIter = myConstraints.begin() + aConstrPos;
  }

  // Get constraint type and verify the constraint parameters are correct
  SketchSolver_Constraint aConstraint(theConstraint);
  int aConstrType = aConstraint.getType();
  if (aConstrType == SLVS_C_UNKNOWN
      || (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
    return false;
  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();

  // Create constraint parameters
  double aDistance = 0.0;  // scalar value of the constraint
  AttributeDoublePtr aDistAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      theConstraint->data()->attribute(SketchPlugin_Constraint::VALUE()));
  if (aDistAttr) {
    aDistance = aDistAttr->value();
    // Issue #196: checking the positivity of the distance constraint
    if (aDistance < tolerance &&
       (aConstrType == SLVS_C_PT_PT_DISTANCE || aConstrType == SLVS_C_PT_LINE_DISTANCE))
      return false;
    // SketchPlugin circle defined by its radius, but SolveSpace uses constraint for diameter
    if (aConstrType == SLVS_C_DIAMETER)
      aDistance *= 2.0;
    if (aConstrMapIter != myConstraintMap.end()
        && fabs(aConstrIter->valA - aDistance) > tolerance) {
      myNeedToSolve = true;
      aConstrIter->valA = aDistance;
    }
  }

  size_t aNbTmpConstraints = myTempConstraints.size();
  Slvs_hEntity aConstrEnt[CONSTRAINT_ATTR_SIZE];  // parameters of the constraint
  for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
    aConstrEnt[indAttr] = SLVS_E_UNKNOWN;
    std::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeRefAttr>(
        theConstraint->data()->attribute(aConstraintAttributes[indAttr]));
    if (!aConstrAttr)
      continue;

    // Convert the object of the attribute to the feature
    FeaturePtr aFeature;
    if (aConstrAttr->isObject() && aConstrAttr->object()) {
      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
          aConstrAttr->object());
      if (!aRC)
        continue;
      std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
      aFeature = aDoc->feature(aRC);
    }

    // For the length constraint the start and end points of the line should be added to the entities list instead of line
    if (aConstrType == SLVS_C_PT_PT_DISTANCE
        && theConstraint->getKind().compare(SketchPlugin_ConstraintLength::ID()) == 0) {
      Slvs_hEntity aLineEnt = changeEntityFeature(aFeature);
      int aEntPos = Search(aLineEnt, myEntities);
      aConstrEnt[indAttr++] = myEntities[aEntPos].point[0];
      aConstrEnt[indAttr++] = myEntities[aEntPos].point[1];
      while (indAttr < CONSTRAINT_ATTR_SIZE)
        aConstrEnt[indAttr++] = 0;
      break;  // there should be no other entities
    } else if (aConstrAttr->isObject())
      aConstrEnt[indAttr] = changeEntityFeature(aFeature);
    else
      aConstrEnt[indAttr] = changeEntity(aConstrAttr->attr());
  }

  if (aConstrMapIter == myConstraintMap.end()) { // Add new constraint
    // Several points may be coincident, it is not necessary to store all constraints between them.
    // Try to find sequence of coincident points which connects the points of new constraint
    if (aConstrType == SLVS_C_POINTS_COINCIDENT) {
      if (aConstrEnt[0] == aConstrEnt[1])  // no need to add self coincidence
        return false;
      if (!addCoincidentPoints(aConstrEnt[0], aConstrEnt[1])) {
        myExtraCoincidence.insert(theConstraint);  // the constraint is stored for further purposes
        return false;
      }
      if (aNbTmpConstraints < myTempConstraints.size()) {
        // There was added temporary constraint. Check that there is no coincident points which already rigid.

        // Get list of already fixed points
        std::set<Slvs_hEntity> anAlreadyFixed;
        std::vector<Slvs_Constraint>::const_iterator aCIter = myConstraints.begin();
        for (; aCIter != myConstraints.end(); aCIter++)
          if (aCIter->type == SLVS_C_WHERE_DRAGGED) {
            std::list<Slvs_hConstraint>::const_iterator aTmpIt = myTempConstraints.begin();
            for (; aTmpIt != myTempConstraints.end(); aTmpIt++)
              if (*aTmpIt == aCIter->h)
                break;
            if (aTmpIt == myTempConstraints.end())
              anAlreadyFixed.insert(aCIter->ptA);
          }

        std::set<Slvs_hConstraint> aTmpConstrToDelete;
        std::list<Slvs_hConstraint>::reverse_iterator aTmpIter = myTempConstraints.rbegin();
        size_t aCurSize = myTempConstraints.size();
        for (; aCurSize > aNbTmpConstraints && aTmpIter != myTempConstraints.rend();
            aTmpIter++, aCurSize--) {
          int aConstrPos = Search(*aTmpIter, myConstraints);
          std::vector<std::set<Slvs_hEntity> >::const_iterator
            aCoincIter = myCoincidentPoints.begin();
          for (; aCoincIter != myCoincidentPoints.end(); aCoincIter++)
            if (aCoincIter->find(myConstraints[aConstrPos].ptA) != aCoincIter->end()) {
              std::set<Slvs_hEntity>::const_iterator anIt;
              for (anIt = aCoincIter->begin(); anIt != aCoincIter->end(); anIt++)
                if (anAlreadyFixed.find(*anIt) != anAlreadyFixed.end()) {
                  aTmpConstrToDelete.insert(*aTmpIter);
                  break;
                }
              break;
            }
        }
        if (!aTmpConstrToDelete.empty())
          removeTemporaryConstraints(aTmpConstrToDelete);
      }
    }
    // For the tangency constraints it is necessary to identify which points of entities are coincident
    int aSlvsOtherFlag = 0;
    int aSlvsOther2Flag = 0;
    if (aConstrType == SLVS_C_ARC_LINE_TANGENT || aConstrType == SLVS_C_CURVE_CURVE_TANGENT) {
      // Search entities used by constraint
      int anEnt1Pos = Search(aConstrEnt[2], myEntities);
      int anEnt2Pos = Search(aConstrEnt[3], myEntities);
      // Obtain start and end points of entities
      Slvs_hEntity aPointsToFind[4];
      aPointsToFind[0] = myEntities[anEnt1Pos].point[1];
      aPointsToFind[1]= myEntities[anEnt1Pos].point[2];
      bool hasLine = (myEntities[anEnt2Pos].type == SLVS_E_LINE_SEGMENT);
      aPointsToFind[2]= myEntities[anEnt2Pos].point[hasLine ? 0 : 1];
      aPointsToFind[3]= myEntities[anEnt2Pos].point[hasLine ? 1 : 2];
      // Search coincident points
      bool isPointFound[4];
      std::vector<std::set<Slvs_hEntity> >::const_iterator aCPIter = myCoincidentPoints.begin();
      for ( ; aCPIter != myCoincidentPoints.end(); aCPIter++) {
        for (int i = 0; i < 4; i++)
          isPointFound[i] = (aCPIter->find(aPointsToFind[i]) != aCPIter->end());
        if ((isPointFound[0] || isPointFound[1]) && (isPointFound[2] || isPointFound[3])) {
          // the arc is tangent by end point
          if (isPointFound[1]) aSlvsOtherFlag = 1;
          // the second item is an arc and it is tangent by end point too
          if (!hasLine && isPointFound[3]) aSlvsOther2Flag = 1;
          break;
        }
      }
      if (aCPIter == myCoincidentPoints.end()) {
        // There is no coincident points between tangential objects. Generate error message
        Events_Error::send(SketchSolver_Error::NO_COINCIDENT_POINTS(), this);
        return false;
      }
    }

    // Create SolveSpace constraint structure
    Slvs_Constraint aSlvsConstr = Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType,
                                                      myWorkplane.h, aDistance, aConstrEnt[0],
                                                      aConstrEnt[1], aConstrEnt[2], aConstrEnt[3]);
    if (aSlvsOtherFlag != 0) aSlvsConstr.other = aSlvsOtherFlag;
    if (aSlvsOther2Flag != 0) aSlvsConstr.other2 = aSlvsOther2Flag;
    myConstraints.push_back(aSlvsConstr);
    myConstraintMap[theConstraint] = std::vector<Slvs_hEntity>(1, aSlvsConstr.h);
    int aConstrPos = Search(aSlvsConstr.h, myConstraints);
    aConstrIter = myConstraints.begin() + aConstrPos;
    myNeedToSolve = true;
  } else { // Attributes of constraint may be changed => update constraint
    Slvs_hEntity* aCurrentAttr[] = {&aConstrIter->ptA, &aConstrIter->ptB,
                                   &aConstrIter->entityA, &aConstrIter->entityB,
                                   &aConstrIter->entityC, &aConstrIter->entityD};
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      if (*(aCurrentAttr[indAttr]) != aConstrEnt[indAttr])
      {
        *(aCurrentAttr[indAttr]) = aConstrEnt[indAttr];
        myNeedToSolve = true;
      }
    }
  }

  // Update flags of entities to be used by constraints
  for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    if (aConstrEnt[indAttr] != 0) {
      int aPos = Search(aConstrEnt[indAttr], myEntities);
      myEntOfConstr[aPos] = true;
      // Sub-entities should be used implcitly
      Slvs_hEntity* aEntPtr = myEntities[aPos].point;
      while (*aEntPtr != 0) {
        aPos = Search(*aEntPtr, myEntities);
        myEntOfConstr[aPos] = true;
        aEntPtr++;
      }
    }

  checkConstraintConsistence(*aConstrIter);
  return true;
}

// ============================================================================
//  Function: changeRigidConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the "Rigid" constraint in the group
// ============================================================================
bool SketchSolver_ConstraintGroup::changeRigidConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // Search this constraint in the current group to update it
  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
  std::vector<Slvs_Constraint>::iterator aConstrIter;
  if (aConstrMapIter != myConstraintMap.end()) {
    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
    aConstrIter = myConstraints.begin() + aConstrPos;
  }

  // Get constraint type and verify the constraint parameters are correct
  SketchSolver_Constraint aConstraint(theConstraint);
  int aConstrType = aConstraint.getType();
  if (aConstrType == SLVS_C_UNKNOWN
      || (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
    return false;
  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();

  Slvs_hEntity aConstrEnt = SLVS_E_UNKNOWN;
  std::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(
      theConstraint->data()->attribute(aConstraintAttributes[0]));
  if (!aConstrAttr)
    return false;

  // Convert the object of the attribute to the feature
  FeaturePtr aFeature;
  if (aConstrAttr->isObject() && aConstrAttr->object()) {
    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
        aConstrAttr->object());
    if (!aRC)
      return false;
    std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
    aFeature = aDoc->feature(aRC);
  }

  aConstrEnt = aConstrAttr->isObject() ? changeEntityFeature(aFeature) : changeEntity(aConstrAttr->attr());

  if (aConstrMapIter == myConstraintMap.end()) { // Add new constraint
    // Check the fixed entity is not a point.
    std::shared_ptr<ModelAPI_AttributeRefAttr> aConstrAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeRefAttr>(theConstraint->data()->attribute(aConstraintAttributes[0]));
    std::shared_ptr<GeomDataAPI_Point> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(aConstrAttr->attr());
    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aConstrAttr->attr());
    if (aPoint || aPoint2D) {
      // Create SolveSpace constraint structure
      Slvs_Constraint aConstraint = Slvs_MakeConstraint(
          ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
          aConstrEnt, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      myConstraints.push_back(aConstraint);
      myConstraintMap[theConstraint] = std::vector<Slvs_hEntity>(1, aConstraint.h);
      int aConstrPos = Search(aConstraint.h, myConstraints);
      aConstrIter = myConstraints.begin() + aConstrPos;
      myNeedToSolve = true;
    } else {
      myConstraintMap[theConstraint] = std::vector<Slvs_hConstraint>();

      // To avoid SolveSpace problems:
      // * if the circle is rigid, we will fix its center and radius;
      // * if the arc is rigid, we will fix its start and end points and radius.
      double aRadius = 0.0;
      bool isArc = false;
      bool isCircle = false;
      if (aFeature) {
        if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
          std::shared_ptr<GeomDataAPI_Point2D> aCenter =
              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
              aFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
          std::shared_ptr<GeomDataAPI_Point2D> aStart =
              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
              aFeature->data()->attribute(SketchPlugin_Arc::START_ID()));
          aRadius = aStart->pnt()->distance(aCenter->pnt());
          isArc = true;
        } else if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
          aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
              aFeature->data()->attribute(SketchPlugin_Circle::RADIUS_ID()))->value();
          isCircle = true;
        }
      }

      // Get list of already fixed points
      std::set<Slvs_hEntity> anAlreadyFixed;
      std::vector<Slvs_Constraint>::const_iterator aCIter = myConstraints.begin();
      for (; aCIter != myConstraints.end(); aCIter++)
        if (aCIter->type == SLVS_C_WHERE_DRAGGED)
          anAlreadyFixed.insert(aCIter->ptA);

      // Create constraints to fix the parameters of the entity
      int aEntPos = Search(aConstrEnt, myEntities);
      Slvs_hEntity* aPointsPtr = myEntities[aEntPos].point;
      if (isArc) aPointsPtr++; // avoid to fix center of arc
      while (*aPointsPtr != 0) {
        // Avoid to create additional "Rigid" constraints for coincident points
        bool isCoincAlreadyFixed = false;
        if (!anAlreadyFixed.empty()) {
          if (anAlreadyFixed.find(*aPointsPtr) != anAlreadyFixed.end())
            isCoincAlreadyFixed = true;

          std::vector<std::set<Slvs_hEntity> >::const_iterator aCoincIter =
              myCoincidentPoints.begin();
          for (; !isCoincAlreadyFixed && aCoincIter != myCoincidentPoints.end(); aCoincIter++) {
            if (aCoincIter->find(*aPointsPtr) == aCoincIter->end())
              continue;
            std::set<Slvs_hEntity>::const_iterator anIter = anAlreadyFixed.begin();
            for (; !isCoincAlreadyFixed && anIter != anAlreadyFixed.end(); anIter++)
              if (aCoincIter->find(*anIter) != aCoincIter->end())
                isCoincAlreadyFixed = true;
          }
        }

        if (!isCoincAlreadyFixed) {
          Slvs_Constraint aConstraint = Slvs_MakeConstraint(
              ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
              *aPointsPtr, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
          myConstraints.push_back(aConstraint);
          myConstraintMap[theConstraint].push_back(aConstraint.h);
        }
        aPointsPtr++;
      }

      if (isArc || isCircle) { // add radius constraint
        Slvs_Constraint aConstraint = Slvs_MakeConstraint(
            ++myConstrMaxID, myID, SLVS_C_DIAMETER, myWorkplane.h, 2.0 * aRadius,
            SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aConstrEnt, SLVS_E_UNKNOWN);
        myConstraints.push_back(aConstraint);
        myConstraintMap[theConstraint].push_back(aConstraint.h);
      }

      // The object is already rigid, so there is no constraints added
      if (myConstraintMap[theConstraint].empty()) {
        myConstraintMap.erase(theConstraint);
        myNeedToSolve = false;
      }
      else
        myNeedToSolve = true;
    }
  }
  return true;
}

// ============================================================================
//  Function: changeMirrorConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the "Mirror" constraint in the group
// ============================================================================
bool SketchSolver_ConstraintGroup::changeMirrorConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  DataPtr aConstrData = theConstraint->data();

  // Search this constraint in the current group to update it
  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
  std::vector<Slvs_Constraint>::iterator aConstrIter;
  if (aConstrMapIter != myConstraintMap.end()) {
    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
    aConstrIter = myConstraints.begin() + aConstrPos;
  }

  // Get constraint type and verify the constraint parameters are correct
  SketchSolver_Constraint aConstraint(theConstraint);
  int aConstrType = aConstraint.getType();
  if (aConstrType == SLVS_C_UNKNOWN
      || (aConstrMapIter != myConstraintMap.end() && aConstrIter->type != aConstrType))
    return false;
  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();

  Slvs_hEntity aMirrorLineEnt = SLVS_E_UNKNOWN;
  AttributeRefAttrPtr aConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstrData->attribute(aConstraintAttributes[0]));
  if (!aConstrAttr)
    return false;

  // Convert the object of the attribute to the feature
  FeaturePtr aMirrorLineFeat;
  if (aConstrAttr->isObject() && aConstrAttr->object()) {
    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
        aConstrAttr->object());
    if (!aRC)
      return false;
    std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
    aMirrorLineFeat = aDoc->feature(aRC);
  }
  aMirrorLineEnt = aConstrAttr->isObject() ?
      changeEntityFeature(aMirrorLineFeat) : changeEntity(aConstrAttr->attr());

  if (aConstrMapIter == myConstraintMap.end()) { // Add new constraint
    // Append symmetric constraint for each point of mirroring features
    AttributeRefListPtr aBaseRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        aConstrData->attribute(aConstraintAttributes[1]));
    AttributeRefListPtr aMirroredRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        aConstrData->attribute(aConstraintAttributes[2]));
    if (!aBaseRefList || !aMirroredRefList)
      return false;

    std::list<ObjectPtr> aBaseList = aBaseRefList->list();
    std::list<ObjectPtr> aMirroredList = aMirroredRefList->list();
    if (aBaseList.size() != aMirroredList.size())
      return false;

    myConstraintMap[theConstraint] = std::vector<Slvs_hConstraint>();

    FeaturePtr aBaseFeature, aMirrorFeature;
    ResultConstructionPtr aRC;
    std::list<ObjectPtr>::iterator aBaseIter = aBaseList.begin();
    std::list<ObjectPtr>::iterator aMirIter = aMirroredList.begin();
    for ( ; aBaseIter != aBaseList.end(); aBaseIter++, aMirIter++) {
      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aBaseIter);
      aBaseFeature = aRC ? aRC->document()->feature(aRC) :
          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aBaseIter);
      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aMirIter);
      aMirrorFeature = aRC ? aRC->document()->feature(aRC) :
          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aMirIter);

      if (!aBaseFeature || !aMirrorFeature || 
          aBaseFeature->getKind() != aMirrorFeature->getKind())
        return false;
      Slvs_hEntity aBaseEnt = changeEntityFeature(aBaseFeature);
      Slvs_hEntity aMirrorEnt = changeEntityFeature(aMirrorFeature);
      // Make aMirrorEnt parameters to be symmetric with aBaseEnt
      makeMirrorEntity(aBaseEnt, aMirrorEnt, aMirrorLineEnt);

      if (aBaseFeature->getKind() == SketchPlugin_Point::ID()) {
        Slvs_Constraint aConstraint = Slvs_MakeConstraint(++myConstrMaxID, myID, aConstrType,
            myWorkplane.h, 0.0, aBaseEnt, aMirrorEnt, aMirrorLineEnt, SLVS_E_UNKNOWN);
        myConstraints.push_back(aConstraint);
        myConstraintMap[theConstraint].push_back(aConstraint.h);
      } else {
        int aBasePos = Search(aBaseEnt, myEntities);
        int aMirrorPos = Search(aMirrorEnt, myEntities);
        if (aBaseFeature->getKind() == SketchPlugin_Line::ID()) {
          for (int ind = 0; ind < 2; ind++) {
            Slvs_Constraint aConstraint = Slvs_MakeConstraint(
                ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
                myEntities[aBasePos].point[ind], myEntities[aMirrorPos].point[ind],
                aMirrorLineEnt, SLVS_E_UNKNOWN);
            myConstraints.push_back(aConstraint);
            myConstraintMap[theConstraint].push_back(aConstraint.h);
          }
        } else if (aBaseFeature->getKind() == SketchPlugin_Circle::ID()) {
          Slvs_Constraint aConstraint = Slvs_MakeConstraint(
              ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
              myEntities[aBasePos].point[0], myEntities[aMirrorPos].point[0],
              aMirrorLineEnt, SLVS_E_UNKNOWN);
          myConstraints.push_back(aConstraint);
          myConstraintMap[theConstraint].push_back(aConstraint.h);
          // Additional constraint for equal radii
          Slvs_Constraint anEqRadConstr = Slvs_MakeConstraint(
              ++myConstrMaxID, myID, SLVS_C_EQUAL_RADIUS, myWorkplane.h, 0.0,
              SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aBaseEnt, aMirrorEnt);
          myConstraints.push_back(anEqRadConstr);
          myConstraintMap[theConstraint].push_back(anEqRadConstr.h);
        } else if (aBaseFeature->getKind() == SketchPlugin_Arc::ID()) {
          int aBaseArcInd[3] = {0, 1, 2}; // indices of points of arc, center corresponds center, first point corresponds last point
          int aMirrorArcInd[3] = {0, 2, 1};
          for (int ind = 0; ind < 3; ind++) {
            Slvs_Constraint aConstraint = Slvs_MakeConstraint(
                ++myConstrMaxID, myID, aConstrType, myWorkplane.h, 0.0,
                myEntities[aBasePos].point[aBaseArcInd[ind]], myEntities[aMirrorPos].point[aMirrorArcInd[ind]],
                aMirrorLineEnt, SLVS_E_UNKNOWN);
            myConstraints.push_back(aConstraint);
            myConstraintMap[theConstraint].push_back(aConstraint.h);
          }
        }
      }
    }

    // Set the mirror line unchanged during constraint recalculation
    int aMirrorLinePos = Search(aMirrorLineEnt, myEntities);
    Slvs_Constraint aRigidStart = Slvs_MakeConstraint(
        ++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED, myWorkplane.h, 0,
        myEntities[aMirrorLinePos].point[0], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    myConstraints.push_back(aRigidStart);
    myConstraintMap[theConstraint].push_back(aRigidStart.h);
    Slvs_Constraint aRigidEnd = Slvs_MakeConstraint(
        ++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED, myWorkplane.h, 0,
        myEntities[aMirrorLinePos].point[1], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    myConstraints.push_back(aRigidEnd);
    myConstraintMap[theConstraint].push_back(aRigidEnd.h);

    // Add temporary constraints for initial objects to be unchanged
    for (aBaseIter = aBaseList.begin(); aBaseIter != aBaseList.end(); aBaseIter++) {
      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aBaseIter);
      aBaseFeature = aRC ? aRC->document()->feature(aRC) :
          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aBaseIter);
      if (!aBaseFeature) continue;
      std::list<AttributePtr> aPoints = aBaseFeature->data()->attributes(GeomDataAPI_Point2D::type());
      std::list<AttributePtr>::iterator anIt = aPoints.begin();
      for ( ; anIt != aPoints.end(); anIt++) {
        // Arcs are fixed by center and start points only (to avoid solving errors in SolveSpace)
        if (aBaseFeature->getKind() == SketchPlugin_Arc::ID() &&
            (*anIt)->id() == SketchPlugin_Arc::END_ID())
          continue;
        addTemporaryConstraintWhereDragged(*anIt);
      }
    }
  }
  return true;
}

// ============================================================================
//  Function: changeFilletConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the "Fillet" constraint in the group
// ============================================================================
bool SketchSolver_ConstraintGroup::changeFilletConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  DataPtr aConstrData = theConstraint->data();

  // Search this constraint in the current group to update it
  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.find(theConstraint);
  std::vector<Slvs_Constraint>::iterator aConstrIter;
  if (aConstrMapIter != myConstraintMap.end()) {
    int aConstrPos = Search(aConstrMapIter->second.front(), myConstraints);
    aConstrIter = myConstraints.begin() + aConstrPos;
  }

  // Get constraint type and verify the constraint parameters are correct
  SketchSolver_Constraint aConstraint(theConstraint);
  int aConstrType = aConstraint.getType();
  if (aConstrType == SLVS_C_UNKNOWN)
    return false;
  const std::vector<std::string>& aConstraintAttributes = aConstraint.getAttributes();

  // Obtain hEntity for basic objects of fillet
  Slvs_hEntity aBaseObject[2];
  FeaturePtr aBaseFeature[2];
  for (unsigned int indAttr = 0; indAttr < 2; indAttr++) {
    AttributeRefAttrPtr aConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstrData->attribute(aConstraintAttributes[indAttr]));
    if (!aConstrAttr)
      return false;
    if (aConstrAttr->isObject() && aConstrAttr->object()) {
      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
          aConstrAttr->object());
      if (!aRC)
        return false;
      std::shared_ptr<ModelAPI_Document> aDoc = aRC->document();
      aBaseFeature[indAttr] = aDoc->feature(aRC);
    }
    aBaseObject[indAttr] = aConstrAttr->isObject() ?
        changeEntityFeature(aBaseFeature[indAttr]) : changeEntity(aConstrAttr->attr());
  }
  // Check the base entities have a coincident point
  int aBaseObjInd[2] = {
      Search(aBaseObject[0], myEntities),
      Search(aBaseObject[1], myEntities)
    };
  int aShift[2] = { // shift for calculating correct start and end points for different types of objects
      myEntities[aBaseObjInd[0]].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0,
      myEntities[aBaseObjInd[1]].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0,
    };
  Slvs_hEntity aFirstObjPoints[2] = { // indices of start and end point of first object
      myEntities[aBaseObjInd[0]].point[aShift[0]],
      myEntities[aBaseObjInd[0]].point[1+aShift[0]]
    };
  Slvs_hEntity aSecondObjPoints[2] = { // indices of start and end point of second object
      myEntities[aBaseObjInd[1]].point[aShift[1]],
      myEntities[aBaseObjInd[1]].point[1+aShift[1]]
    };
  bool isCoincidentFound = false;
  int aBaseCoincInd[2] = {0, 0}; // indices in aFirstObjPoint and aSecondObjPoint identifying coincident points
  std::vector<std::set<Slvs_hEntity> >::iterator aCPIter = myCoincidentPoints.begin();
  for ( ; aCPIter != myCoincidentPoints.end() && !isCoincidentFound; aCPIter++)
    for (int ind1 = 0; ind1 < 2 && !isCoincidentFound; ind1++)
      for (int ind2 = 0; ind2 < 2 && !isCoincidentFound; ind2++)
        if (aCPIter->find(aFirstObjPoints[ind1]) != aCPIter->end() &&
            aCPIter->find(aSecondObjPoints[ind2]) != aCPIter->end()) {
          aBaseCoincInd[0] = ind1;
          aBaseCoincInd[1] = ind2;
          isCoincidentFound = true;
        }
  if (!isCoincidentFound) {
    // There is no coincident points between objects. Generate error message
    Events_Error::send(SketchSolver_Error::NO_COINCIDENT_POINTS(), this);
    return false;
  }

  // Create fillet entities
  // - first object is placed on the first base 
  // - second object is on the second base 
  // - third object is a filleting arc
  static const int aNbFilletEnt = 3;
  Slvs_hEntity aFilletEnt[aNbFilletEnt];
  int aFilletObjInd[aNbFilletEnt];
  AttributeRefListPtr aFilletRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aConstrData->attribute(aConstraintAttributes[2]));
  if (!aFilletRefList)
    return false;
  std::list<ObjectPtr> aFilletList = aFilletRefList->list();
  if (aFilletList.size() < aNbFilletEnt)
    return false;
  FeaturePtr aFilletFeature;
  ResultConstructionPtr aRC;
  std::list<ObjectPtr>::iterator aFilIter = aFilletList.begin();
  for (int indEnt = 0; aFilIter != aFilletList.end(); aFilIter++, indEnt++) {
    aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aFilIter);
    aFilletFeature = aRC ? aRC->document()->feature(aRC) :
        std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFilIter);
    if (!aFilletFeature)
      return false;
    aFilletEnt[indEnt] = changeEntityFeature(aFilletFeature);
    aFilletObjInd[indEnt] = Search(aFilletEnt[indEnt], myEntities);
  }
  // At first time, for correct result, move floating points of fillet on the middle points of base objects
  if (myConstraintMap.find(theConstraint) == myConstraintMap.end()) {
    double anArcPoints[6];
    for (int indEnt = 0; indEnt < aNbFilletEnt - 1; indEnt++) {
      int anIndShift = myEntities[aFilletObjInd[indEnt]].type == SLVS_E_ARC_OF_CIRCLE ? 1 : 0;
      int aPointsPos[2] = {
          Search(myEntities[aFilletObjInd[indEnt]].point[anIndShift], myEntities),
          Search(myEntities[aFilletObjInd[indEnt]].point[1+anIndShift], myEntities)
        };
      int aParamPos[2] = {
          Search(myEntities[aPointsPos[0]].param[0], myParams),
          Search(myEntities[aPointsPos[1]].param[0], myParams)
        };
      int anIndex = aParamPos[aBaseCoincInd[indEnt]];
      myParams[anIndex].val =
          0.5 * (myParams[aParamPos[0]].val + myParams[aParamPos[1]].val);
      myParams[1 + anIndex].val =
          0.5 * (myParams[1 + aParamPos[0]].val + myParams[1 + aParamPos[1]].val);
      if (anIndShift == 1) { // place the changed point on the arc
        int aCenterPos = Search(myEntities[aFilletObjInd[indEnt]].point[0], myEntities);
        int aCenterParam = Search(myEntities[aCenterPos].param[0], myParams);
        double x = myParams[anIndex].val - myParams[aCenterParam].val;
        double y = myParams[1 + anIndex].val - myParams[1 + aCenterParam].val;
        double aNorm = sqrt(x*x + y*y);
        if (aNorm >= tolerance) {
          double x1 = myParams[aParamPos[1-aBaseCoincInd[indEnt]]].val - myParams[aCenterParam].val;
          double y1 = myParams[1 + aParamPos[1-aBaseCoincInd[indEnt]]].val - myParams[1 + aCenterParam].val;
          double aRad = sqrt(x1*x1 + y1*y1);
          myParams[anIndex].val = myParams[aCenterPos].val + x * aRad / aNorm;
          myParams[1 + anIndex].val = myParams[1 + aCenterPos].val + y * aRad / aNorm;
        }
      }
      anArcPoints[indEnt*2+2] = myParams[anIndex].val;
      anArcPoints[indEnt*2+3] = myParams[1 + anIndex].val;
    }
    anArcPoints[0] = 0.5 * (anArcPoints[2] + anArcPoints[4]);
    anArcPoints[1] = 0.5 * (anArcPoints[3] + anArcPoints[5]);
    for (int indArcPt = 0; indArcPt < 3; indArcPt++) {
      int aPtPos = Search(myEntities[aFilletObjInd[2]].point[indArcPt], myEntities);
      int aParamPos = Search(myEntities[aPtPos].param[0], myParams);
      myParams[aParamPos].val = anArcPoints[indArcPt * 2];
      myParams[aParamPos + 1].val = anArcPoints[indArcPt * 2 + 1];
    }
  }

  // Check the fillet arc which point to be connected to
  bool isArcInversed = false; // indicates that start and end points of arc should be connected to second and first object respectively
  Slvs_hEntity hEnt = myEntities[aFilletObjInd[2]].point[1];
  int aPos = Search(hEnt, myEntities);
  Slvs_hParam anArcStartPoint = myEntities[aPos].param[0];
  aPos = Search(anArcStartPoint, myParams);
  double anArcPtCoord[2] = {myParams[aPos].val, myParams[aPos+1].val};
  double aSqDistances[2];
  int aPtInd;
  for (int indEnt = 0; indEnt < aNbFilletEnt - 1; indEnt++) {
    aPtInd = aBaseCoincInd[indEnt]+aShift[indEnt];
    hEnt = myEntities[aFilletObjInd[indEnt]].point[aPtInd];
    aPos = Search(hEnt, myEntities);
    Slvs_hParam anObjectPoint = myEntities[aPos].param[0];
    aPos = Search(anObjectPoint, myParams);
    double aPtCoord[2] = {myParams[aPos].val, myParams[aPos+1].val};
    aSqDistances[indEnt] = 
        (anArcPtCoord[0] - aPtCoord[0]) * (anArcPtCoord[0] - aPtCoord[0]) +
        (anArcPtCoord[1] - aPtCoord[1]) * (anArcPtCoord[1] - aPtCoord[1]);
  }
  if (aSqDistances[1] < aSqDistances[0])
    isArcInversed = true;

  // Create list of constraints to generate fillet
  std::vector<Slvs_hConstraint> aConstrList;
  bool isExists = myConstraintMap.find(theConstraint) != myConstraintMap.end(); // constraint already exists
  std::vector<Slvs_hConstraint>::iterator aCMapIter =
    isExists ? myConstraintMap[theConstraint].begin() : aConstrList.begin();
  int aCurConstrPos = isExists ? Search(*aCMapIter, myConstraints) : 0;
  for (int indEnt = 0; indEnt < aNbFilletEnt - 1; indEnt++) {
    // one point of fillet object should be coincident with the point on base, non-coincident with another base object
    aPtInd = 1-aBaseCoincInd[indEnt]+aShift[indEnt]; // (1-aBaseCoincInd[indEnt]) = index of non-coincident point, aShift is used to process all types of shapes
    Slvs_hEntity aPtBase = myEntities[aBaseObjInd[indEnt]].point[aPtInd];
    Slvs_hEntity aPtFillet = myEntities[aFilletObjInd[indEnt]].point[aPtInd];
    if (isExists) {
      myConstraints[aCurConstrPos].ptA = aPtBase;
      myConstraints[aCurConstrPos].ptB = aPtFillet;
      aCMapIter++;
      aCurConstrPos = Search(*aCMapIter, myConstraints);
    } else {
      Slvs_Constraint aCoincConstr = Slvs_MakeConstraint(
          ++myConstrMaxID, myID, SLVS_C_POINTS_COINCIDENT, myWorkplane.h,
          0, aPtBase, aPtFillet, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      myConstraints.push_back(aCoincConstr);
      aConstrList.push_back(aCoincConstr.h);
    }

    // another point of fillet object should be placed on the base object
    Slvs_Constraint aPonCurveConstr;
    int aTangentType;
    if (myEntities[aFilletObjInd[indEnt]].type == SLVS_E_ARC_OF_CIRCLE) {
      // centers of arcs should be coincident
      aPtBase = myEntities[aBaseObjInd[indEnt]].point[0];
      aPtFillet = myEntities[aFilletObjInd[indEnt]].point[0];
      if (isExists) {
        myConstraints[aCurConstrPos].ptA = aPtBase;
        myConstraints[aCurConstrPos].ptB = aPtFillet;
        aCMapIter++;
        aCurConstrPos = Search(*aCMapIter, myConstraints);
      } else {
        aPonCurveConstr = Slvs_MakeConstraint(
            ++myConstrMaxID, myID, SLVS_C_POINTS_COINCIDENT, myWorkplane.h,
            0, aPtBase, aPtFillet, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      }
      aPtFillet = myEntities[aFilletObjInd[indEnt]].point[1+aBaseCoincInd[indEnt]]; // !!! will be used below
      aTangentType = SLVS_C_CURVE_CURVE_TANGENT;
    } else {
      aPtInd = aBaseCoincInd[indEnt];
      aPtFillet = myEntities[aFilletObjInd[indEnt]].point[aPtInd];
      if (isExists) {
        myConstraints[aCurConstrPos].ptA = aPtFillet;
        aCMapIter++;
        aCurConstrPos = Search(*aCMapIter, myConstraints);
      } else {
        aPonCurveConstr = Slvs_MakeConstraint(
            ++myConstrMaxID, myID, SLVS_C_PT_ON_LINE, myWorkplane.h,
            0, aPtFillet, SLVS_E_UNKNOWN, aBaseObject[indEnt], SLVS_E_UNKNOWN);
      }
      aTangentType = SLVS_C_ARC_LINE_TANGENT;
    }
    if (!isExists) {
      myConstraints.push_back(aPonCurveConstr);
      aConstrList.push_back(aPonCurveConstr.h);
    }

    // Bound point of fillet arc should be tangently coincident with a bound point of fillet object
    aPtInd = 1 + (isArcInversed ? 1-indEnt : indEnt);
    Slvs_hEntity aPtArc = myEntities[aFilletObjInd[2]].point[aPtInd];
    if (isExists) {
      myConstraints[aCurConstrPos].ptA = aPtArc;
      myConstraints[aCurConstrPos].ptB = aPtFillet;
      aCMapIter++;
      aCurConstrPos = Search(*aCMapIter, myConstraints);
      myConstraints[aCurConstrPos].entityA = aFilletEnt[2];
      myConstraints[aCurConstrPos].entityB = aFilletEnt[indEnt];
      myConstraints[aCurConstrPos].other = (isArcInversed ? 1-indEnt : indEnt);
      aCMapIter++;
      aCurConstrPos = Search(*aCMapIter, myConstraints);
    } else {
      Slvs_Constraint aCoincConstr = Slvs_MakeConstraint(
          ++myConstrMaxID, myID, SLVS_C_POINTS_COINCIDENT, myWorkplane.h,
          0, aPtArc, aPtFillet, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
      myConstraints.push_back(aCoincConstr);
      aConstrList.push_back(aCoincConstr.h);
      Slvs_Constraint aTangency = Slvs_MakeConstraint(
          ++myConstrMaxID, myID, aTangentType, myWorkplane.h,
          0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aFilletEnt[2], aFilletEnt[indEnt]);
      aTangency.other = (isArcInversed ? 1-indEnt : indEnt);
      aTangency.other2 = aTangentType == SLVS_C_CURVE_CURVE_TANGENT ? aBaseCoincInd[indEnt] : 0;
      myConstraints.push_back(aTangency);
      aConstrList.push_back(aTangency.h);
    }
  }

  // Additional constraint for fillet diameter
  double aRadius = 0.0;  // scalar value of the constraint
  AttributeDoublePtr aDistAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aConstrData->attribute(SketchPlugin_Constraint::VALUE()));
  aRadius = aDistAttr->value();
  if (isExists) {
    myConstraints[aCurConstrPos].entityA = aFilletEnt[2];
    myConstraints[aCurConstrPos].valA = aRadius * 2.0;
    aCMapIter++;
  } else {
    Slvs_Constraint aDiamConstr = Slvs_MakeConstraint(
        ++myConstrMaxID, myID, SLVS_C_DIAMETER, myWorkplane.h, aRadius * 2.0,
        SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aFilletEnt[2], SLVS_E_UNKNOWN);
    myConstraints.push_back(aDiamConstr);
    aConstrList.push_back(aDiamConstr.h);

    myConstraintMap[theConstraint] = aConstrList;
  }

  // Additional temporary constraints for base objects to be fixed
  for (unsigned int indAttr = 0; indAttr < 2; indAttr++) {
    if (!aBaseFeature[indAttr]) {
      AttributeRefAttrPtr aConstrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrData->attribute(aConstraintAttributes[indAttr]));
      addTemporaryConstraintWhereDragged(aConstrAttr->attr());
      continue;
    }
    std::list<AttributePtr> anAttributes =
        aBaseFeature[indAttr]->data()->attributes(GeomDataAPI_Point2D::type());
    std::list<AttributePtr>::iterator anIt = anAttributes.begin();
    for ( ; anIt != anAttributes.end(); anIt++) {
      // Arc should be fixed by center and start points only (to avoid "conflicting constraints" message)
      if (aBaseFeature[indAttr]->getKind() == SketchPlugin_Arc::ID() &&
          (*anIt)->id() == SketchPlugin_Arc::END_ID())
        continue;
      addTemporaryConstraintWhereDragged(*anIt);
    }
  }
  return true;
}

// ============================================================================
//  Function: changeEntity
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the element affected by any constraint
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintGroup::changeEntity(
    std::shared_ptr<ModelAPI_Attribute> theEntity)
{
  // If the entity is already in the group, try to find it
  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator aEntIter =
      myEntityAttrMap.find(theEntity);
  int aEntPos;
  std::vector<Slvs_Param>::const_iterator aParamIter;  // looks at first parameter of already existent entity or at the end of vector otherwise
  if (aEntIter == myEntityAttrMap.end())  // no such entity => should be created
    aParamIter = myParams.end();
  else {  // the entity already exists
    aEntPos = Search(aEntIter->second, myEntities);
    int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
    aParamIter = myParams.begin() + aParamPos;
  }
  const bool isEntExists = (aEntIter != myEntityAttrMap.end());  // defines that the entity already exists
  const bool isNeedToSolve = myNeedToSolve;
  myNeedToSolve = false;

  if (isEntExists) {
    // Verify that the entity is not used by "Rigid" constraint.
    // If it is used, the object should not move.
    std::vector<std::set<Slvs_hEntity> >::iterator aCoincIter = myCoincidentPoints.begin();
    for (; aCoincIter != myCoincidentPoints.end(); aCoincIter++)
      if (aCoincIter->find(aEntIter->second) != aCoincIter->end())
        break;
    std::set<Slvs_hEntity> aCoincident;
    if (aCoincIter != myCoincidentPoints.end()) {
      aCoincident = *aCoincIter;
      aCoincident.erase(aEntIter->second);

      std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
      for (; aConstrIter != myConstraints.end(); aConstrIter++)
        if (aConstrIter->type == SLVS_C_WHERE_DRAGGED &&
            aCoincident.find(aConstrIter->ptA) != aCoincident.end()) {
          myNeedToSolve = true;
          return aEntIter->second;
        }
    }
  }

  // Look over supported types of entities
  Slvs_Entity aNewEntity;
  aNewEntity.h = SLVS_E_UNKNOWN;

  // Point in 3D
  std::shared_ptr<GeomDataAPI_Point> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      theEntity);
  if (aPoint) {
    Slvs_hParam aX = changeParameter(aPoint->x(), aParamIter);
    Slvs_hParam aY = changeParameter(aPoint->y(), aParamIter);
    Slvs_hParam aZ = changeParameter(aPoint->z(), aParamIter);
    if (!isEntExists) // New entity
      aNewEntity = Slvs_MakePoint3d(++myEntityMaxID, myID, aX, aY, aZ);
  } else {
    // All entities except 3D points are created on workplane. So, if there is no workplane yet, then error
    if (myWorkplane.h == SLVS_E_UNKNOWN)
      return SLVS_E_UNKNOWN;

    // Point in 2D
    std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theEntity);
    if (aPoint2D) {
      Slvs_hParam aU = changeParameter(aPoint2D->x(), aParamIter);
      Slvs_hParam aV = changeParameter(aPoint2D->y(), aParamIter);
      if (!isEntExists) // New entity
        aNewEntity = Slvs_MakePoint2d(++myEntityMaxID, myID, myWorkplane.h, aU, aV);
    } else {
      // Scalar value (used for the distance entities)
      AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theEntity);
      if (aScalar) {
        Slvs_hParam aValue = changeParameter(aScalar->value(), aParamIter);
        if (!isEntExists) // New entity
          aNewEntity = Slvs_MakeDistance(++myEntityMaxID, myID, myWorkplane.h, aValue);
      }
    }
  }
  /// \todo Other types of entities

  Slvs_hEntity aResult = SLVS_E_UNKNOWN; // Unsupported or wrong entity type

  if (isEntExists) {
    myNeedToSolve = myNeedToSolve || isNeedToSolve;
    aResult = aEntIter->second;
  } else if (aNewEntity.h != SLVS_E_UNKNOWN) {
    myEntities.push_back(aNewEntity);
    myEntOfConstr.push_back(false);
    myEntityAttrMap[theEntity] = aNewEntity.h;
    aResult = aNewEntity.h;
  }

  // If the attribute was changed by the user, we need to fix it before solving
  if (myNeedToSolve && theEntity->isImmutable())
    addTemporaryConstraintWhereDragged(theEntity, false);

  return aResult;
}

// ============================================================================
//  Function: changeEntity
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the element defined by the feature affected by any constraint
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintGroup::changeEntityFeature(FeaturePtr theEntity)
{
  if (!theEntity->data()->isValid())
    return SLVS_E_UNKNOWN;
  // If the entity is already in the group, try to find it
  std::map<FeaturePtr, Slvs_hEntity>::const_iterator aEntIter = myEntityFeatMap.find(theEntity);
  // defines that the entity already exists
  const bool isEntExists = (myEntityFeatMap.find(theEntity) != myEntityFeatMap.end());
  
  Slvs_Entity aNewEntity;
  aNewEntity.h = SLVS_E_UNKNOWN;

  // SketchPlugin features
  std::shared_ptr<SketchPlugin_Feature> aFeature = std::dynamic_pointer_cast<
      SketchPlugin_Feature>(theEntity);
  if (aFeature) {  // Verify the feature by its kind
    const std::string& aFeatureKind = aFeature->getKind();
    AttributePtr anAttribute;

    // Line
    if (aFeatureKind.compare(SketchPlugin_Line::ID()) == 0) {
      anAttribute = aFeature->data()->attribute(SketchPlugin_Line::START_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aStart = changeEntity(anAttribute);

      anAttribute = aFeature->data()->attribute(SketchPlugin_Line::END_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aEnd = changeEntity(anAttribute);

      if (!isEntExists) // New entity
        aNewEntity = Slvs_MakeLineSegment(++myEntityMaxID, myID, myWorkplane.h, aStart, aEnd);
    }
    // Circle
    else if (aFeatureKind.compare(SketchPlugin_Circle::ID()) == 0) {
      anAttribute = aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aCenter = changeEntity(anAttribute);

      anAttribute = aFeature->data()->attribute(SketchPlugin_Circle::RADIUS_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aRadius = changeEntity(anAttribute);

      if (!isEntExists) // New entity
        aNewEntity = Slvs_MakeCircle(++myEntityMaxID, myID, myWorkplane.h, aCenter,
                                     myWorkplane.normal, aRadius);
    }
    // Arc
    else if (aFeatureKind.compare(SketchPlugin_Arc::ID()) == 0) {
      anAttribute = aFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aCenter = changeEntity(anAttribute);

      anAttribute = aFeature->data()->attribute(SketchPlugin_Arc::START_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aStart = changeEntity(anAttribute);

      anAttribute = aFeature->data()->attribute(SketchPlugin_Arc::END_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aEnd = changeEntity(anAttribute);

      if (!isEntExists)
        aNewEntity = Slvs_MakeArcOfCircle(++myEntityMaxID, myID, myWorkplane.h,
                                          myWorkplane.normal, aCenter, aStart, aEnd);
    }
    // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
    else if (aFeatureKind.compare(SketchPlugin_Point::ID()) == 0) {
      anAttribute = aFeature->data()->attribute(SketchPlugin_Point::COORD_ID());
      if (!anAttribute->isInitialized()) return SLVS_E_UNKNOWN;
      Slvs_hEntity aPoint = changeEntity(anAttribute);

      if (isEntExists)
        return aEntIter->second;

      // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier
      myEntityFeatMap[theEntity] = aPoint;
      myNeedToSolve = true;
      return aPoint;
    }
  }
  /// \todo Other types of features

  if (isEntExists)
    return aEntIter->second;

  if (aNewEntity.h != SLVS_E_UNKNOWN) {
    myEntities.push_back(aNewEntity);
    myEntOfConstr.push_back(false);
    myEntityFeatMap[theEntity] = aNewEntity.h;
    myNeedToSolve = true;
    return aNewEntity.h;
  }

  // Unsupported or wrong entity type
  return SLVS_E_UNKNOWN;
}

// ============================================================================
//  Function: changeNormal
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update the normal of workplane
// ============================================================================
Slvs_hEntity SketchSolver_ConstraintGroup::changeNormal(
    std::shared_ptr<ModelAPI_Attribute> theDirX, std::shared_ptr<ModelAPI_Attribute> theDirY,
    std::shared_ptr<ModelAPI_Attribute> theNorm)
{
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theDirX);
  std::shared_ptr<GeomDataAPI_Dir> aDirY = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theDirY);
  if (!aDirX || !aDirY || (fabs(aDirX->x()) + fabs(aDirX->y()) + fabs(aDirX->z()) < tolerance)
      || (fabs(aDirY->x()) + fabs(aDirY->y()) + fabs(aDirY->z()) < tolerance))
    return SLVS_E_UNKNOWN;

  // quaternion parameters of normal vector
  double qw, qx, qy, qz;
  Slvs_MakeQuaternion(aDirX->x(), aDirX->y(), aDirX->z(), aDirY->x(), aDirY->y(), aDirY->z(), &qw,
                      &qx, &qy, &qz);
  double aNormCoord[4] = { qw, qx, qy, qz };

  // Try to find existent normal
  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator aEntIter =
      myEntityAttrMap.find(theNorm);
  std::vector<Slvs_Param>::const_iterator aParamIter;  // looks to the first parameter of already existent entity or to the end of vector otherwise
  if (aEntIter == myEntityAttrMap.end())  // no such entity => should be created
    aParamIter = myParams.end();
  else {  // the entity already exists, update it
    int aEntPos = Search(aEntIter->second, myEntities);
    int aParamPos = Search(myEntities[aEntPos].param[0], myParams);
    aParamIter = myParams.begin() + aParamPos;
  }

  // Change parameters of the normal
  Slvs_hParam aNormParams[4];
  for (int i = 0; i < 4; i++)
    aNormParams[i] = changeParameter(aNormCoord[i], aParamIter);

  if (aEntIter != myEntityAttrMap.end())  // the entity already exists
    return aEntIter->second;

  // Create a normal
  Slvs_Entity aNormal = Slvs_MakeNormal3d(++myEntityMaxID, myID, aNormParams[0], aNormParams[1],
                                          aNormParams[2], aNormParams[3]);
  myEntities.push_back(aNormal);
  myEntOfConstr.push_back(false);
  myEntityAttrMap[theNorm] = aNormal.h;
  return aNormal.h;
}

// ============================================================================
//  Function: addWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create workplane for the group
// ============================================================================
bool SketchSolver_ConstraintGroup::addWorkplane(std::shared_ptr<ModelAPI_CompositeFeature> theSketch)
{
  if (myWorkplane.h || theSketch->getKind().compare(SketchPlugin_Sketch::ID()) != 0)
    return false;  // the workplane already exists or the function parameter is not Sketch

  mySketch = theSketch;
  updateWorkplane();
  return true;
}

// ============================================================================
//  Function: updateWorkplane
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  update parameters of workplane
// ============================================================================
bool SketchSolver_ConstraintGroup::updateWorkplane()
{
  if (!mySketch->data())
    return false; // case sketch is deleted
  // Get parameters of workplane
  std::shared_ptr<ModelAPI_Attribute> aDirX = mySketch->data()->attribute(
      SketchPlugin_Sketch::DIRX_ID());
  std::shared_ptr<ModelAPI_Attribute> aDirY = mySketch->data()->attribute(
      SketchPlugin_Sketch::DIRY_ID());
  std::shared_ptr<ModelAPI_Attribute> aNorm = mySketch->data()->attribute(
      SketchPlugin_Sketch::NORM_ID());
  std::shared_ptr<ModelAPI_Attribute> anOrigin = mySketch->data()->attribute(
      SketchPlugin_Sketch::ORIGIN_ID());
  // Transform them into SolveSpace format
  Slvs_hEntity aNormalWP = changeNormal(aDirX, aDirY, aNorm);
  if (!aNormalWP)
    return false;
  Slvs_hEntity anOriginWP = changeEntity(anOrigin);
  if (!anOriginWP)
    return false;

  if (!myWorkplane.h) {
    // Create workplane
    myWorkplane = Slvs_MakeWorkplane(++myEntityMaxID, myID, anOriginWP, aNormalWP);
    // Workplane should be added to the list of entities
    myEntities.push_back(myWorkplane);
    myEntOfConstr.push_back(false);
  }
  return true;
}

// ============================================================================
//  Function: changeParameter
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  create/update value of parameter
// ============================================================================
Slvs_hParam SketchSolver_ConstraintGroup::changeParameter(
    const double& theParam, std::vector<Slvs_Param>::const_iterator& thePrmIter)
{
  if (thePrmIter != myParams.end()) {  // Parameter should be updated
    int aParamPos = thePrmIter - myParams.begin();
    if (fabs(thePrmIter->val - theParam) > tolerance) {
      myNeedToSolve = true;  // parameter is changed, need to resolve constraints
      myParams[aParamPos].val = theParam;
    }
    thePrmIter++;
    return myParams[aParamPos].h;
  }

  // Newly created parameter
  Slvs_Param aParam = Slvs_MakeParam(++myParamMaxID, myID, theParam);
  myParams.push_back(aParam);
  myNeedToSolve = true;
  // The list of parameters is changed, move iterator to the end of the list to avoid problems
  thePrmIter = myParams.end();
  return aParam.h;
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
bool SketchSolver_ConstraintGroup::resolveConstraints()
{
  if (!myNeedToSolve)
    return false;

  myConstrSolver.setGroupID(myID);
  myConstrSolver.setParameters(myParams);
  myConstrSolver.setEntities(myEntities);
  myConstrSolver.setConstraints(myConstraints);
  myConstrSolver.setDraggedParameters(myTempPointWhereDragged);

  int aResult = myConstrSolver.solve();
  if (aResult == SLVS_RESULT_OKAY) {  // solution succeeded, store results into correspondent attributes
                                      // Obtain result into the same list of parameters
    if (!myConstrSolver.getResult(myParams))
      return true;

    // We should go through the attributes map, because only attributes have valued parameters
    std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator anEntIter =
        myEntityAttrMap.begin();
    for (; anEntIter != myEntityAttrMap.end(); anEntIter++) {
      if (anEntIter->first->owner().get() && anEntIter->first->owner()->data().get())
        anEntIter->first->owner()->data()->blockSendAttributeUpdated(true);
      if (updateAttribute(anEntIter->first, anEntIter->second))
        updateRelatedConstraints(anEntIter->first);
    }
    updateFilletConstraints();
    // unblock all features then
    for (anEntIter = myEntityAttrMap.begin(); anEntIter != myEntityAttrMap.end(); anEntIter++) {
      if (anEntIter->first->owner().get() && anEntIter->first->owner()->data().get())
        anEntIter->first->owner()->data()->blockSendAttributeUpdated(false);
    }
  } else if (!myConstraints.empty())
    Events_Error::send(SketchSolver_Error::CONSTRAINTS(), this);

  removeTemporaryConstraints();
  myNeedToSolve = false;
  return true;
}

// ============================================================================
//  Function: mergeGroups
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  append specified group to the current group
// ============================================================================
void SketchSolver_ConstraintGroup::mergeGroups(const SketchSolver_ConstraintGroup& theGroup)
{
  // If specified group is empty, no need to merge
  if (theGroup.myConstraintMap.empty())
    return;

  // Map between old and new indexes of SolveSpace constraints
  std::map<Slvs_hConstraint, Slvs_hConstraint> aConstrMap;

  // Add all constraints from theGroup to the current group
  ConstraintMap::const_iterator aConstrIter = theGroup.myConstraintMap.begin();
  for (; aConstrIter != theGroup.myConstraintMap.end(); aConstrIter++)
    if (changeConstraint(aConstrIter->first))
      aConstrMap[aConstrIter->second.back()] = myConstrMaxID;  // the constraint was added => store its ID

  // Add temporary constraints from theGroup
  std::list<Slvs_hConstraint>::const_iterator aTempConstrIter = theGroup.myTempConstraints.begin();
  for (; aTempConstrIter != theGroup.myTempConstraints.end(); aTempConstrIter++) {
    std::map<Slvs_hConstraint, Slvs_hConstraint>::iterator aFind = aConstrMap.find(
        *aTempConstrIter);
    if (aFind != aConstrMap.end())
      myTempConstraints.push_back(aFind->second);
  }

  if (myTempPointWhereDragged.empty())
    myTempPointWhereDragged = theGroup.myTempPointWhereDragged;
  else if (!theGroup.myTempPointWhereDragged.empty()) {  // Need to create additional transient constraint
    std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator aFeatureIter =
        theGroup.myEntityAttrMap.begin();
    for (; aFeatureIter != theGroup.myEntityAttrMap.end(); aFeatureIter++)
      if (aFeatureIter->second == myTempPointWDrgdID) {
        addTemporaryConstraintWhereDragged(aFeatureIter->first);
        break;
      }
  }

  myNeedToSolve = myNeedToSolve || theGroup.myNeedToSolve;
}

// ============================================================================
//  Function: splitGroup
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  divide the group into several subgroups
// ============================================================================
void SketchSolver_ConstraintGroup::splitGroup(std::vector<SketchSolver_ConstraintGroup*>& theCuts)
{
  // Divide constraints and entities into several groups
  std::vector<std::set<Slvs_hEntity> > aGroupsEntities;
  std::vector<std::set<Slvs_hConstraint> > aGroupsConstr;
  int aMaxNbEntities = 0;  // index of the group with maximal nuber of elements (this group will be left in the current)
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for (; aConstrIter != myConstraints.end(); aConstrIter++) {
    Slvs_hEntity aConstrEnt[] = { aConstrIter->ptA, aConstrIter->ptB, aConstrIter->entityA,
        aConstrIter->entityB };
    std::vector<int> anIndexes;
    // Go through the groupped entities and find even one of entities of current constraint
    std::vector<std::set<Slvs_hEntity> >::iterator aGrEntIter;
    for (aGrEntIter = aGroupsEntities.begin(); aGrEntIter != aGroupsEntities.end(); aGrEntIter++) {
      bool isFound = false;
      for (int i = 0; i < 4 && !isFound; i++)
        if (aConstrEnt[i] != 0) {
          isFound = (aGrEntIter->find(aConstrEnt[i]) != aGrEntIter->end());
          // Also we need to check sub-entities
          int aEntPos = Search(aConstrEnt[i], myEntities);
          if (aEntPos != myEntities.size()) { // MPV: to fix the crash on close
            Slvs_hEntity* aSub = myEntities[aEntPos].point;
            for (int j = 0; *aSub != 0 && j < 4 && !isFound; aSub++, j++)
              isFound = (aGrEntIter->find(*aSub) != aGrEntIter->end());
          }
        }
      if (isFound)
        anIndexes.push_back(aGrEntIter - aGroupsEntities.begin());
    }
    // Add new group if no one is found
    if (anIndexes.empty()) {
      std::set<Slvs_hEntity> aNewGrEnt;
      for (int i = 0; i < 4; i++)
        if (aConstrEnt[i] != 0) {
          aNewGrEnt.insert(aConstrEnt[i]);
          int aEntPos = Search(aConstrEnt[i], myEntities);
          if (aEntPos != myEntities.size()) { // MPV: to fix the crash on close
            Slvs_hEntity* aSub = myEntities[aEntPos].point;
            for (int j = 0; *aSub != 0 && j < 4; aSub++, j++)
              aNewGrEnt.insert(*aSub);
          }
        }
      std::set<Slvs_hConstraint> aNewGrConstr;
      aNewGrConstr.insert(aConstrIter->h);

      aGroupsEntities.push_back(aNewGrEnt);
      aGroupsConstr.push_back(aNewGrConstr);
      if (aNewGrEnt.size() > aGroupsEntities[aMaxNbEntities].size())
        aMaxNbEntities = aGroupsEntities.size() - 1;
    } else {  // Add entities indexes into the found group
      aGrEntIter = aGroupsEntities.begin() + anIndexes.front();
      for (int i = 0; i < 4; i++)
        if (aConstrEnt[i] != 0) {
          aGrEntIter->insert(aConstrEnt[i]);
          int aEntPos = Search(aConstrEnt[i], myEntities);
          if (aEntPos != myEntities.size()) { // MPV: to fix the crash on close
            Slvs_hEntity* aSub = myEntities[aEntPos].point;
            for (int j = 0; *aSub != 0 && j < 4; aSub++, j++)
              aGrEntIter->insert(*aSub);
          }
        }
      aGroupsConstr[anIndexes.front()].insert(aConstrIter->h);
      if (aGrEntIter->size() > aGroupsEntities[aMaxNbEntities].size())
        aMaxNbEntities = aGrEntIter - aGroupsEntities.begin();
      if (anIndexes.size() > 1) {  // There are found several connected groups, merge them
        std::vector<std::set<Slvs_hEntity> >::iterator aFirstGroup = aGroupsEntities.begin()
            + anIndexes.front();
        std::vector<std::set<Slvs_hConstraint> >::iterator aFirstConstr = aGroupsConstr.begin()
            + anIndexes.front();
        std::vector<int>::iterator anInd = anIndexes.begin();
        for (++anInd; anInd != anIndexes.end(); anInd++) {
          aFirstGroup->insert(aGroupsEntities[*anInd].begin(), aGroupsEntities[*anInd].end());
          aFirstConstr->insert(aGroupsConstr[*anInd].begin(), aGroupsConstr[*anInd].end());
        }
        if (aFirstGroup->size() > aGroupsEntities[aMaxNbEntities].size())
          aMaxNbEntities = anIndexes.front();
        // Remove merged groups
        for (anInd = anIndexes.end() - 1; anInd != anIndexes.begin(); anInd--) {
          aGroupsEntities.erase(aGroupsEntities.begin() + (*anInd));
          aGroupsConstr.erase(aGroupsConstr.begin() + (*anInd));
        }
      }
    }
  }

  if (aGroupsEntities.size() <= 1)
    return;

  // Remove the group with maximum elements as it will be left in the current group
  aGroupsEntities.erase(aGroupsEntities.begin() + aMaxNbEntities);
  aGroupsConstr.erase(aGroupsConstr.begin() + aMaxNbEntities);

  // Add new groups of constraints and divide current group
  std::vector<SketchSolver_ConstraintGroup*> aNewGroups;
  for (int i = aGroupsEntities.size(); i > 0; i--) {
    SketchSolver_ConstraintGroup* aG = new SketchSolver_ConstraintGroup(mySketch);
    aNewGroups.push_back(aG);
  }
  ConstraintMap::const_iterator aConstrMapIter = myConstraintMap.begin();
  int aConstrMapPos = 0;  // position of iterator in the map (used to restore iterator after removing constraint)
  while (aConstrMapIter != myConstraintMap.end()) {
    std::vector<std::set<Slvs_hConstraint> >::const_iterator aGIter = aGroupsConstr.begin();
    std::vector<SketchSolver_ConstraintGroup*>::iterator aGroup = aNewGroups.begin();
    for (; aGIter != aGroupsConstr.end(); aGIter++, aGroup++)
      if (aGIter->find(aConstrMapIter->second.front()) != aGIter->end()) {
        (*aGroup)->changeConstraint(aConstrMapIter->first);
        removeConstraint(aConstrMapIter->first);
        // restore iterator
        aConstrMapIter = myConstraintMap.begin();
        for (int i = 0; i < aConstrMapPos; i++)
          aConstrMapIter++;
        break;
      }
    if (aGIter == aGroupsConstr.end()) {
      aConstrMapIter++;
      aConstrMapPos++;
    }
  }

  theCuts.insert(theCuts.end(), aNewGroups.begin(), aNewGroups.end());
}

// ============================================================================
//  Function: updateGroup
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  search removed entities and constraints
// ============================================================================
bool SketchSolver_ConstraintGroup::updateGroup()
{
  ConstraintMap::reverse_iterator aConstrIter = myConstraintMap.rbegin();
  bool isAllValid = true;
  bool isCCRemoved = false;  // indicates that at least one of coincidence constraints was removed
  int aConstrIndex = 0;
  while (/*isAllValid && */aConstrIter != myConstraintMap.rend()) {
    if (!aConstrIter->first->data() || !aConstrIter->first->data()->isValid()) {
      if (aConstrIter->first->getKind().compare(SketchPlugin_ConstraintCoincidence::ID()) == 0)
        isCCRemoved = true;
      removeConstraint(aConstrIter->first);
      isAllValid = false;
      // Get back the correct position of iterator after the "remove" operation
      aConstrIter = myConstraintMap.rbegin();
      for (int i = 0; i < aConstrIndex && aConstrIter != myConstraintMap.rend(); i++)
        aConstrIter++;
    } else {
      aConstrIter++;
      aConstrIndex++;
    }
  }

  // Check if some entities are invalid too
  std::set<Slvs_hEntity> anEntToRemove;
  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
      anAttrIter = myEntityAttrMap.begin();
  while (anAttrIter != myEntityAttrMap.end()) {
    if (!anAttrIter->first->owner() || !anAttrIter->first->owner()->data() ||
        !anAttrIter->first->owner()->data()->isValid()) {
      anEntToRemove.insert(anAttrIter->second);
      std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator
          aRemovedIter = anAttrIter;
      anAttrIter++;
      myEntityAttrMap.erase(aRemovedIter);
    } else
      anAttrIter++;
  }
  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIter = myEntityFeatMap.begin();
  while (aFeatIter != myEntityFeatMap.end()) {
    if (!aFeatIter->first || !aFeatIter->first->data() ||
        !aFeatIter->first->data()->isValid()) {
      anEntToRemove.insert(aFeatIter->second);
      std::map<FeaturePtr, Slvs_hEntity>::iterator aRemovedIter = aFeatIter;
      aFeatIter++;
      myEntityFeatMap.erase(aRemovedIter);
    } else
      aFeatIter++;
  }
  removeEntitiesById(anEntToRemove);

  // Probably, need to update coincidence constraints
  if (isCCRemoved && !myExtraCoincidence.empty()) {
    // Make a copy, because the new list of unused constrtaints will be generated
    std::set<std::shared_ptr<SketchPlugin_Constraint> > anExtraCopy = myExtraCoincidence;
    myExtraCoincidence.clear();

    std::set<std::shared_ptr<SketchPlugin_Constraint> >::iterator aCIter = anExtraCopy.begin();
    for (; aCIter != anExtraCopy.end(); aCIter++)
      if ((*aCIter)->data() && (*aCIter)->data()->isValid())
        changeConstraint(*aCIter);
  }

  return !isAllValid;
}

// ============================================================================
//  Function: updateAttribute
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  update features of sketch after resolving constraints
// ============================================================================
bool SketchSolver_ConstraintGroup::updateAttribute(
    std::shared_ptr<ModelAPI_Attribute> theAttribute, const Slvs_hEntity& theEntityID)
{
  // Search the position of the first parameter of the entity
  int anEntPos = Search(theEntityID, myEntities);
  int aFirstParamPos = Search(myEntities[anEntPos].param[0], myParams);

  // Look over supported types of entities

  // Point in 3D
  std::shared_ptr<GeomDataAPI_Point> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      theAttribute);
  if (aPoint) {
    if (fabs(aPoint->x() - myParams[aFirstParamPos].val) > tolerance
        || fabs(aPoint->y() - myParams[aFirstParamPos + 1].val) > tolerance
        || fabs(aPoint->z() - myParams[aFirstParamPos + 2].val) > tolerance) {
      aPoint->setValue(myParams[aFirstParamPos].val, myParams[aFirstParamPos + 1].val,
                       myParams[aFirstParamPos + 2].val);
      return true;
    }
    return false;
  }

  // Point in 2D
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D) {
    if (fabs(aPoint2D->x() - myParams[aFirstParamPos].val) > tolerance
        || fabs(aPoint2D->y() - myParams[aFirstParamPos + 1].val) > tolerance) {
      aPoint2D->setValue(myParams[aFirstParamPos].val, myParams[aFirstParamPos + 1].val);
      return true;
    }
    return false;
  }

  // Scalar value
  AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aScalar) {
    if (fabs(aScalar->value() - myParams[aFirstParamPos].val) > tolerance) {
      aScalar->setValue(myParams[aFirstParamPos].val);
      return true;
    }
    return false;
  }

  /// \todo Support other types of entities
  return false;
}

// ============================================================================
//  Function: updateEntityIfPossible
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  search the entity in this group and update it
// ============================================================================
void SketchSolver_ConstraintGroup::updateEntityIfPossible(
    std::shared_ptr<ModelAPI_Attribute> theEntity)
{
  if (myEntityAttrMap.find(theEntity) != myEntityAttrMap.end()) {
    // If the attribute is a point and it is changed (the group needs to rebuild),
    // probably user has dragged this point into this position,
    // so it is necessary to add constraint which will guarantee the point will not change

    // Store myNeedToSolve flag to verify the entity is really changed
    bool aNeedToSolveCopy = myNeedToSolve;
    myNeedToSolve = false;

    changeEntity(theEntity);

    if (myNeedToSolve)  // the entity is changed
    {
      // Verify the entity is a point and add temporary constraint of permanency
      std::shared_ptr<GeomDataAPI_Point> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point>(
          theEntity);
      std::shared_ptr<GeomDataAPI_Point2D> aPoint2D = std::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(theEntity);
      if (aPoint || aPoint2D)
        addTemporaryConstraintWhereDragged(theEntity);
    }

    // Restore flag of changes
    myNeedToSolve = myNeedToSolve || aNeedToSolveCopy;

    if (myNeedToSolve)
      updateRelatedConstraints(theEntity);
  }
}

// ============================================================================
//  Function: addTemporaryConstraintWhereDragged
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  add transient constraint SLVS_C_WHERE_DRAGGED for the entity, 
//            which was moved by user
// ============================================================================
void SketchSolver_ConstraintGroup::addTemporaryConstraintWhereDragged(
    std::shared_ptr<ModelAPI_Attribute> theEntity,
    bool theAllowToFit)
{
  // Find identifier of the entity
  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::const_iterator anEntIter =
      myEntityAttrMap.find(theEntity);
  if (anEntIter == myEntityAttrMap.end())
    return;

  // Get identifiers of all dragged points
  std::set<Slvs_hEntity> aDraggedPntID;
  aDraggedPntID.insert(myTempPointWDrgdID);
  std::list<Slvs_hConstraint>::const_iterator aTmpCoIter = myTempConstraints.begin();
  for (; aTmpCoIter != myTempConstraints.end(); aTmpCoIter++) {
    unsigned int aConstrPos = Search(*aTmpCoIter, myConstraints);
    if (aConstrPos < myConstraints.size())
      aDraggedPntID.insert(myConstraints[aConstrPos].ptA);
  }
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for (; aConstrIter != myConstraints.end(); aConstrIter++)
    if (aConstrIter->type == SLVS_C_WHERE_DRAGGED)
      aDraggedPntID.insert(aConstrIter->ptA);
  // Find whether there is a point coincident with theEntity, which already has SLVS_C_WHERE_DRAGGED
  std::vector<std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
  for (; aCoPtIter != myCoincidentPoints.end(); aCoPtIter++) {
    if (aCoPtIter->find(anEntIter->second) == aCoPtIter->end())
      continue;  // the entity was not found in current set

    // Find one of already created SLVS_C_WHERE_DRAGGED constraints in current set of coincident points
    std::set<Slvs_hEntity>::const_iterator aDrgIter = aDraggedPntID.begin();
    for (; aDrgIter != aDraggedPntID.end(); aDrgIter++)
      if (aCoPtIter->find(*aDrgIter) != aCoPtIter->end())
        return;  // the SLVS_C_WHERE_DRAGGED constraint already exists
  }
  if (aDraggedPntID.find(anEntIter->second) != aDraggedPntID.end())
    return;

  // If this is a first dragged point, its parameters should be placed 
  // into Slvs_System::dragged field to avoid system inconsistense
  if (myTempPointWhereDragged.empty() && theAllowToFit) {
    int anEntPos = Search(anEntIter->second, myEntities);
    Slvs_hParam* aDraggedParam = myEntities[anEntPos].param;
    for (int i = 0; i < 4; i++, aDraggedParam++)
      if (*aDraggedParam != 0)
        myTempPointWhereDragged.push_back(*aDraggedParam);
    myTempPointWDrgdID = myEntities[anEntPos].h;
    return;
  }

  // Create additional SLVS_C_WHERE_DRAGGED constraint if myTempPointWhereDragged field is not empty
  Slvs_Constraint aWDConstr = Slvs_MakeConstraint(++myConstrMaxID, myID, SLVS_C_WHERE_DRAGGED,
                                                  myWorkplane.h, 0.0, anEntIter->second, 0, 0, 0);
  myConstraints.push_back(aWDConstr);
  myTempConstraints.push_back(aWDConstr.h);
}

// ============================================================================
//  Function: removeTemporaryConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  remove all transient SLVS_C_WHERE_DRAGGED constraints after
//            resolving the set of constraints
// ============================================================================
void SketchSolver_ConstraintGroup::removeTemporaryConstraints(
    const std::set<Slvs_hConstraint>& theRemoved)
{
  std::list<Slvs_hConstraint>::reverse_iterator aTmpConstrIter;
  for (aTmpConstrIter = myTempConstraints.rbegin(); aTmpConstrIter != myTempConstraints.rend();
      aTmpConstrIter++) {
    if (!theRemoved.empty() && theRemoved.find(*aTmpConstrIter) == theRemoved.end())
      continue;
    unsigned int aConstrPos = Search(*aTmpConstrIter, myConstraints);
    if (aConstrPos >= myConstraints.size())
      continue;
    myConstraints.erase(myConstraints.begin() + aConstrPos);

    // If the removing constraint has higher index, decrease the indexer
    if (*aTmpConstrIter == myConstrMaxID)
      myConstrMaxID--;
  }
  myTempConstraints.clear();

  // Clear basic dragged point
  myTempPointWhereDragged.clear();
  myTempPointWDrgdID = SLVS_E_UNKNOWN;
}

// ============================================================================
//  Function: removeConstraint
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  remove constraint and all unused entities
// ============================================================================
void SketchSolver_ConstraintGroup::removeConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  ConstraintMap::iterator anIterToRemove = myConstraintMap.find(theConstraint);
  if (anIterToRemove == myConstraintMap.end())
    return;

  std::vector<Slvs_hConstraint> aCnstrToRemove = anIterToRemove->second;
  // Remove constraint from the map
  myConstraintMap.erase(anIterToRemove);

  std::set<Slvs_hEntity> anEntToRemove;
  
  // Find unused entities
  std::vector<Slvs_hConstraint>::iterator aCnstrToRemoveIter = aCnstrToRemove.begin();
  for (; aCnstrToRemoveIter != aCnstrToRemove.end(); aCnstrToRemoveIter++) {
    int aConstrPos = Search(*aCnstrToRemoveIter, myConstraints);
    Slvs_hEntity aCnstEnt[] = { myConstraints[aConstrPos].ptA, myConstraints[aConstrPos].ptB,
        myConstraints[aConstrPos].entityA, myConstraints[aConstrPos].entityB };
    for (int i = 0; i < 4; i++)
      if (aCnstEnt[i] != 0)
        anEntToRemove.insert(aCnstEnt[i]);
    myConstraints.erase(myConstraints.begin() + aConstrPos);
    if (*aCnstrToRemoveIter == myConstrMaxID)
      myConstrMaxID--;
  }

  // Find all entities which are based on these unused
  std::vector<Slvs_Entity>::const_iterator anEntIter = myEntities.begin();
  for ( ; anEntIter != myEntities.end(); anEntIter++)
    if (anEntIter->type == SLVS_E_LINE_SEGMENT || anEntIter->type == SLVS_E_CIRCLE ||
        anEntIter->type == SLVS_E_ARC_OF_CIRCLE) {
      for (int i = 0; i < 4; i++)
        if (anEntToRemove.find(anEntIter->point[i]) != anEntToRemove.end()) {
          anEntToRemove.insert(anEntIter->h);
          for (int j = 0; j < 4; j++)
            if (anEntIter->param[j] != 0)
              anEntToRemove.insert(anEntIter->point[j]);
          break;
        }
    }

  std::vector<Slvs_Constraint>::const_iterator aConstrIter = myConstraints.begin();
  for (; aConstrIter != myConstraints.end(); aConstrIter++) {
    Slvs_hEntity aEnts[] = { aConstrIter->ptA, aConstrIter->ptB, aConstrIter->entityA, aConstrIter
        ->entityB };
    for (int i = 0; i < 4; i++)
      if (aEnts[i] != 0 && anEntToRemove.find(aEnts[i]) != anEntToRemove.end())
        anEntToRemove.erase(aEnts[i]);
  }

  if (anEntToRemove.empty())
    return;

  // Remove unused entities
  std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator anEntAttrIter =
      myEntityAttrMap.begin();
  while (anEntAttrIter != myEntityAttrMap.end()) {
    if (anEntToRemove.find(anEntAttrIter->second) != anEntToRemove.end()) {
      std::map<std::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity>::iterator aRemovedIter =
          anEntAttrIter;
      anEntAttrIter++;
      myEntityAttrMap.erase(aRemovedIter);
    } else
      anEntAttrIter++;
  }
  std::map<FeaturePtr, Slvs_hEntity>::iterator anEntFeatIter = myEntityFeatMap.begin();
  while (anEntFeatIter != myEntityFeatMap.end()) {
    if (anEntToRemove.find(anEntFeatIter->second) != anEntToRemove.end()) {
      std::map<FeaturePtr, Slvs_hEntity>::iterator aRemovedIter = anEntFeatIter;
      anEntFeatIter++;
      myEntityFeatMap.erase(aRemovedIter);
    } else
      anEntFeatIter++;
  }

  removeEntitiesById(anEntToRemove);

  if (myCoincidentPoints.size() == 1 && myCoincidentPoints.front().empty())
    myCoincidentPoints.clear();
}

// ============================================================================
//  Function: removeEntitiesById
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  Removes specified entities and their parameters
// ============================================================================
void SketchSolver_ConstraintGroup::removeEntitiesById(const std::set<Slvs_hEntity>& theEntities)
{
  std::set<Slvs_hEntity>::const_reverse_iterator aRemIter = theEntities.rbegin();
  for (; aRemIter != theEntities.rend(); aRemIter++) {
    unsigned int anEntPos = Search(*aRemIter, myEntities);
    if (anEntPos >= myEntities.size())
      continue;
    if (myEntities[anEntPos].param[0] != 0) {
      unsigned int aParamPos = Search(myEntities[anEntPos].param[0], myParams);
      if (aParamPos >= myParams.size())
        continue;
      int aNbParams = 0;
      while (myEntities[anEntPos].param[aNbParams] != 0)
        aNbParams++;
      if (myEntities[anEntPos].param[aNbParams - 1] == myParamMaxID)
        myParamMaxID -= aNbParams;
      myParams.erase(myParams.begin() + aParamPos, myParams.begin() + aParamPos + aNbParams);
      if (*aRemIter == myEntityMaxID)
        myEntityMaxID--;
    }
    myEntities.erase(myEntities.begin() + anEntPos);
    myEntOfConstr.erase(myEntOfConstr.begin() + anEntPos);

    // Remove entity's ID from the lists of conincident points
    std::vector<std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
    for (; aCoPtIter != myCoincidentPoints.end(); aCoPtIter++)
      aCoPtIter->erase(*aRemIter);
  }
}

// ============================================================================
//  Function: addCoincidentPoints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  add coincident point the appropriate list of such points
// ============================================================================
bool SketchSolver_ConstraintGroup::addCoincidentPoints(const Slvs_hEntity& thePoint1,
                                                       const Slvs_hEntity& thePoint2)
{
  std::vector<std::set<Slvs_hEntity> >::iterator aCoPtIter = myCoincidentPoints.begin();
  std::vector<std::set<Slvs_hEntity> >::iterator aFirstFound = myCoincidentPoints.end();
  while (aCoPtIter != myCoincidentPoints.end()) {
    bool isFound[2] = {  // indicate which point ID was already in coincidence constraint
        aCoPtIter->find(thePoint1) != aCoPtIter->end(), aCoPtIter->find(thePoint2)
            != aCoPtIter->end(), };
    if (isFound[0] && isFound[1])  // points are already connected by coincidence constraints => no need additional one
      return false;
    if ((isFound[0] && !isFound[1]) || (!isFound[0] && isFound[1])) {
      if (aFirstFound != myCoincidentPoints.end()) {  // there are two groups of coincident points connected by created constraint => merge them
        int aFirstFoundShift = aFirstFound - myCoincidentPoints.begin();
        int aCurrentShift = aCoPtIter - myCoincidentPoints.begin();
        aFirstFound->insert(aCoPtIter->begin(), aCoPtIter->end());
        myCoincidentPoints.erase(aCoPtIter);
        aFirstFound = myCoincidentPoints.begin() + aFirstFoundShift;
        aCoPtIter = myCoincidentPoints.begin() + aCurrentShift;
        continue;
      } else {
        aCoPtIter->insert(isFound[0] ? thePoint2 : thePoint1);
        aFirstFound = aCoPtIter;
      }
    }
    aCoPtIter++;
  }
  // No points were found, need to create new set
  if (aFirstFound == myCoincidentPoints.end()) {
    std::set<Slvs_hEntity> aNewSet;
    aNewSet.insert(thePoint1);
    aNewSet.insert(thePoint2);
    myCoincidentPoints.push_back(aNewSet);
  }

  return true;
}

// ============================================================================
//  Function: updateRelatedConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  emit the signal to update constraints
// ============================================================================
void SketchSolver_ConstraintGroup::updateRelatedConstraints(
    std::shared_ptr<ModelAPI_Attribute> theEntity) const
{
  ConstraintMap::const_iterator aConstrIter = myConstraintMap.begin();
  for (; aConstrIter != myConstraintMap.end(); aConstrIter++) {
    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttributes = aConstrIter->first->data()
        ->attributes(std::string());

    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrIter = anAttributes.begin();
    for (; anAttrIter != anAttributes.end(); anAttrIter++) {
      bool isUpd = (*anAttrIter == theEntity);
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(*anAttrIter);
      if (aRefAttr && !aRefAttr->isObject() && aRefAttr->attr() == theEntity)
        isUpd = true;

      if (isUpd) {
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(aConstrIter->first, anEvent);
        break;
      }
    }
  }
}

void SketchSolver_ConstraintGroup::updateRelatedConstraintsFeature(
    std::shared_ptr<ModelAPI_Feature> theFeature) const
{
  ConstraintMap::const_iterator aConstrIter = myConstraintMap.begin();
  for (; aConstrIter != myConstraintMap.end(); aConstrIter++) {
    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttributes = aConstrIter->first->data()
        ->attributes(std::string());

    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrIter = anAttributes.begin();
    for (; anAttrIter != anAttributes.end(); anAttrIter++) {
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(*anAttrIter);
      if (aRefAttr && aRefAttr->isObject() && aRefAttr->object() == theFeature) {
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(aConstrIter->first, anEvent);
        break;
      }
    }
  }
}

// ============================================================================
//  Function: updateFilletConstraints
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  change fillet arc to be less than 180 degree
// ============================================================================
void SketchSolver_ConstraintGroup::updateFilletConstraints()
{
  ConstraintMap::const_iterator aConstrIter = myConstraintMap.begin();
  for (; aConstrIter != myConstraintMap.end(); aConstrIter++)
    if (aConstrIter->first->getKind() == SketchPlugin_ConstraintFillet::ID()) {
      AttributeRefListPtr aFilletRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
          aConstrIter->first->data()->attribute(SketchPlugin_ConstraintFillet::ENTITY_C()));
      if (!aFilletRefList)
        return;
      ObjectPtr anArcObj = aFilletRefList->object(2);
      std::shared_ptr<GeomDataAPI_Point2D> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          anArcObj->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
      std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          anArcObj->data()->attribute(SketchPlugin_Arc::START_ID()));
      std::shared_ptr<GeomDataAPI_Point2D> aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          anArcObj->data()->attribute(SketchPlugin_Arc::END_ID()));
      double aCosA = aStart->x() - aCenter->x();
      double aSinA = aStart->y() - aCenter->y();
      double aCosB = aEnd->x() - aCenter->x();
      double aSinB = aEnd->y() - aCenter->y();
      if (aCosA * aSinB - aSinA * aCosB <= 0.0) {
        anArcObj->data()->blockSendAttributeUpdated(true);
        double x = aStart->x();
        double y = aStart->y();
        aStart->setValue(aEnd->x(), aEnd->y());
        aEnd->setValue(x, y);
        // Update constraint data
        changeFilletConstraint(aConstrIter->first);
        anArcObj->data()->blockSendAttributeUpdated(false);
      }
    }
}

// ============================================================================
//  Function: makeMirrorEntity
//  Class:    SketchSolver_ConstraintGroup
//  Purpose:  change entities parameters to make them symmetric relating to the mirror line
// ============================================================================
void SketchSolver_ConstraintGroup::makeMirrorEntity(const Slvs_hEntity& theBase,
                                                    const Slvs_hEntity& theMirror,
                                                    const Slvs_hEntity& theMirrorLine)
{
  Slvs_Entity aBase = myEntities[Search(theBase, myEntities)];
  Slvs_Entity aMirror = myEntities[Search(theMirror, myEntities)];
  int i = 0;
  while (aBase.point[i] != 0 && aMirror.point[i] != 0) {
    makeMirrorEntity(aBase.point[i], aMirror.point[i], theMirrorLine);
    i++;
  }
  if (aBase.param[0] != 0 && aMirror.param[0] != 0) { // this is a point, copy it
    Slvs_Entity aMirrorLine = myEntities[Search(theMirrorLine, myEntities)];
    std::shared_ptr<GeomAPI_XY> aLinePoints[2];
    for (i = 0; i < 2; i++) {
      Slvs_Entity aPoint = myEntities[Search(aMirrorLine.point[i], myEntities)];
      int aParamPos = Search(aPoint.param[0], myParams);
      aLinePoints[i] = std::shared_ptr<GeomAPI_XY>(
          new GeomAPI_XY(myParams[aParamPos].val, myParams[1+aParamPos].val));
    }
    int aBaseParamPos = Search(aBase.param[0], myParams);
    int aMirrorParamPos = Search(aMirror.param[0], myParams);
    std::shared_ptr<GeomAPI_XY> aPoint = std::shared_ptr<GeomAPI_XY>(
        new GeomAPI_XY(myParams[aBaseParamPos].val, myParams[1+aBaseParamPos].val));

    // direction of a mirror line
    std::shared_ptr<GeomAPI_Dir2d> aDir = std::shared_ptr<GeomAPI_Dir2d>(
        new GeomAPI_Dir2d(aLinePoints[1]->added(aLinePoints[0]->multiplied(-1.0))));
    // orthogonal direction
    aDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir->y(), -aDir->x()));

    std::shared_ptr<GeomAPI_XY> aVec = std::shared_ptr<GeomAPI_XY>(
        new GeomAPI_XY(aPoint->x() - aLinePoints[0]->x(), aPoint->y() - aLinePoints[0]->y()));
    double aDist = aVec->dot(aDir->xy());
    std::shared_ptr<GeomAPI_XY> aMirrorPoint = aPoint->added(aDir->xy()->multiplied(-2.0 * aDist));

    myParams[aMirrorParamPos].val = aMirrorPoint->x();
    myParams[1+aMirrorParamPos].val = aMirrorPoint->y();
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
