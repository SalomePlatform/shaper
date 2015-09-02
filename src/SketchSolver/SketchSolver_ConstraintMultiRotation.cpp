#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_MultiRotation.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <math.h>

void SketchSolver_ConstraintMultiRotation::getAttributes(
    Slvs_hEntity& theCenter, double& theAngle,
    std::vector< std::vector<Slvs_hEntity> >& thePoints,
    std::vector< std::vector<Slvs_hEntity> >& theEntities)
{
  DataPtr aData = myBaseConstraint->data();
  theAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_MultiRotation::ANGLE_ID()))->value();

  AttributePtr aCenterAttr = aData->attribute(SketchPlugin_MultiRotation::CENTER_ID());
  if (!aCenterAttr || !aCenterAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }
  int aType = SLVS_E_UNKNOWN; // type of created entity
  Slvs_hEntity anEntityID = myGroup->getAttributeId(aCenterAttr);
  if (anEntityID == SLVS_E_UNKNOWN)
    anEntityID = changeEntity(aCenterAttr, aType);
  theCenter = anEntityID;

  // Lists of objects and number of copies
  AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  myNumberOfObjects = anInitialRefList->size();
  myNumberOfCopies = (size_t) aData->integer(SketchPlugin_MultiRotation::NUMBER_OF_COPIES_ID())->value();
  AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aRefList) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  // Obtain all points of initial features and store them into separate lists
  // containing their translated copies.
  // Also all circles and arc collected too, because they will be constrained by equal radii.
  FeaturePtr aFeature;
  ResultConstructionPtr aRC;
  static const size_t MAX_POINTS = 3;
  std::vector<Slvs_hEntity> aPoints[MAX_POINTS]; // lists of points of features
  std::vector<Slvs_hEntity> anEntities;
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjectIter = anObjectList.begin();
  while (anObjectIter != anObjectList.end()) {
    for (size_t i = 0; i < MAX_POINTS; ++i)
      aPoints[i].clear();
    anEntities.clear();

    for (size_t i = 0; i <= myNumberOfCopies && anObjectIter != anObjectList.end(); i++, anObjectIter++) {
      aFeature = ModelAPI_Feature::feature(*anObjectIter);
      if (!aFeature)
        continue;
      anEntityID = changeEntity(aFeature, aType);
      anEntities.push_back(anEntityID);
      Slvs_Entity anEntity = myStorage->getEntity(anEntityID);
      switch (aType) {
      case SLVS_E_POINT_IN_2D:
      case SLVS_E_POINT_IN_3D:
        aPoints[0].push_back(anEntityID);
        break;
      case SLVS_E_LINE_SEGMENT:
        aPoints[0].push_back(anEntity.point[0]); // start point of line
        aPoints[1].push_back(anEntity.point[1]); // end point of line
        break;
      case SLVS_E_CIRCLE:
        aPoints[0].push_back(anEntity.point[0]); // center of circle
        break;
      case SLVS_E_ARC_OF_CIRCLE:
        aPoints[0].push_back(anEntity.point[0]); // center of arc
        aPoints[1].push_back(anEntity.point[1]); // start point of arc
        aPoints[2].push_back(anEntity.point[2]); // end point of arc
        break;
      default:
        myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
        return;
      }
    }

    for (size_t i = 0; i < MAX_POINTS; ++i)
      if (!aPoints[i].empty())
        thePoints.push_back(aPoints[i]);
    if (!anEntities.empty())
      theEntities.push_back(anEntities);
  }
}

void SketchSolver_ConstraintMultiRotation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  std::vector<std::vector<Slvs_hEntity> > anEntitiesAndCopies;
  getAttributes(myRotationCenter, myAngle, myPointsAndCopies, anEntitiesAndCopies);
  if (!myErrorMsg.empty())
    return;

  // Set the rotation center unchanged during constraint recalculation
  Slvs_Constraint aConstraint;
  if (!myStorage->isPointFixed(myRotationCenter, aConstraint.h, true)) {
    aConstraint = Slvs_MakeConstraint(
        SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_WHERE_DRAGGED, myGroup->getWorkplaneId(), 0.0,
        myRotationCenter, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
    aConstraint.h = myStorage->addConstraint(aConstraint);
    mySlvsConstraints.push_back(aConstraint.h);
  }

  processEntities(anEntitiesAndCopies);
  adjustConstraint();
}

void SketchSolver_ConstraintMultiRotation::updateLocal()
{
  // update angle value
  myAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      myBaseConstraint->attribute(SketchPlugin_MultiRotation::ANGLE_ID()))->value();
}

void SketchSolver_ConstraintMultiRotation::adjustConstraint()
{
  if (fabs(myAngle) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  std::list<Slvs_Constraint> aCoincident = myStorage->getConstraintsByType(SLVS_C_POINTS_COINCIDENT);
  std::list<Slvs_Constraint>::const_iterator aCoIt;

  // Check overconstrained on rotation center (if it is coincident with other fixed point)
  Slvs_hConstraint aFixedCenter;
  if (myStorage->isPointFixed(myRotationCenter, aFixedCenter, false)) {
    Slvs_hConstraint aFixed;
    for (aCoIt = aCoincident.begin(); aCoIt != aCoincident.end(); ++aCoIt)
      if ((aCoIt->ptA == myRotationCenter && myStorage->isPointFixed(aCoIt->ptB, aFixed, true)) ||
          (aCoIt->ptB == myRotationCenter && myStorage->isPointFixed(aCoIt->ptA, aFixed, true))) {
        // Un-fix the center
        myStorage->removeConstraint(aFixedCenter);
        std::vector<Slvs_hConstraint>::iterator aSCIt = mySlvsConstraints.begin();
        for (; aSCIt != mySlvsConstraints.end(); ++aSCIt)
          if (*aSCIt == aFixedCenter) {
            mySlvsConstraints.erase(aSCIt);
            break;
          }
      }
  }

  // Obtain coordinates of rotation center
  Slvs_Entity aRotCenter = myStorage->getEntity(myRotationCenter);
  myCenterCoord[0] = myStorage->getParameter(aRotCenter.param[0]).val;
  myCenterCoord[1] = myStorage->getParameter(aRotCenter.param[1]).val;

  myRotationVal[0] = sin(myAngle * PI / 180.0);
  myRotationVal[1] = cos(myAngle * PI / 180.0);

  SketchSolver_ConstraintMulti::adjustConstraint();
}

void SketchSolver_ConstraintMultiRotation::getRelative(
    double theAbsX, double theAbsY, double& theRelX, double& theRelY)
{
  theRelX = theAbsX - myCenterCoord[0];
  theRelY = theAbsY - myCenterCoord[1];
}

void SketchSolver_ConstraintMultiRotation::getAbsolute(
    double theRelX, double theRelY, double& theAbsX, double& theAbsY)
{
  theAbsX = theRelX + myCenterCoord[0];
  theAbsY = theRelY + myCenterCoord[1];
}

void SketchSolver_ConstraintMultiRotation::transformRelative(double& theX, double& theY)
{
  // rotate direction
  // myRotationVal[0] = sinA, myRotationVal[1] = cosA
  double aTemp = theX * myRotationVal[1] - theY * myRotationVal[0];
  theY = theX * myRotationVal[0] + theY * myRotationVal[1];
  theX = aTemp;
}

const std::string& SketchSolver_ConstraintMultiRotation::nameNbCopies()
{
  return SketchPlugin_MultiRotation::NUMBER_OF_COPIES_ID();
}
