#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

////#include <SketchPlugin_Arc.h>
#include <SketchPlugin_MultiTranslation.h>
////
////#include <ModelAPI_AttributeDouble.h>
////#include <ModelAPI_AttributeInteger.h>
////#include <ModelAPI_AttributeRefAttr.h>
////#include <ModelAPI_AttributeRefList.h>
////#include <ModelAPI_ResultConstruction.h>
////#include <ModelAPI_Data.h>
////
////#include <GeomAPI_Dir2d.h>
////#include <GeomAPI_XY.h>
////
////#include <math.h>


void SketchSolver_ConstraintMultiTranslation::getAttributes(
    EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
    std::list< std::list<EntityWrapperPtr> >& theEntities)
{
  DataPtr aData = myBaseConstraint->data();
  AttributePtr aStartPointAttr = aData->attribute(SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributePtr aEndPointAttr = aData->attribute(SketchPlugin_MultiTranslation::END_POINT_ID());
  if (!aStartPointAttr || !aStartPointAttr->isInitialized() ||
      !aEndPointAttr || !aEndPointAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = CONSTRAINT_MULTI_TRANSLATION;

  myStorage->update(aStartPointAttr);
  theStartPoint = myStorage->entity(aStartPointAttr);
  myStorage->update(aEndPointAttr);
  theEndPoint = myStorage->entity(aEndPointAttr);

  getEntitiesAndCopies(theEntities);

////  int aType = SLVS_E_UNKNOWN; // type of created entity
////  Slvs_hEntity anEntityID = myGroup->getAttributeId(aStartPointAttr);
////  if (anEntityID == SLVS_E_UNKNOWN)
////    anEntityID = changeEntity(aStartPointAttr, aType);
////  theStartPoint = anEntityID;
////  anEntityID = myGroup->getAttributeId(aEndPointAttr);
////  if (anEntityID == SLVS_E_UNKNOWN)
////    anEntityID = changeEntity(aEndPointAttr, aType);
////  theEndPoint = anEntityID;
////
////  // Lists of objects and number of copies
////  AttributeRefListPtr anInitialRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
////      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
////  myNumberOfObjects = anInitialRefList->size();
////  myNumberOfCopies = (size_t) aData->integer(SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID())->value() - 1;
////  if (myNumberOfCopies <= 0)
////    return;
////
////  AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
////      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
////  if (!aRefList) {
////    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
////    return;
////  }
////
////  // Obtain all points of initial features and store them into separate lists
////  // containing their translated copies.
////  // Also all circles and arc collected too, because they will be constrained by equal radii.
////  FeaturePtr aFeature;
////  ResultConstructionPtr aRC;
////  static const size_t MAX_POINTS = 3;
////  std::vector<Slvs_hEntity> aPoints[MAX_POINTS]; // lists of points of features
////  std::vector<Slvs_hEntity> anEntities;          // list of translated entities
////  std::list<ObjectPtr> anObjectList = aRefList->list();
////  std::list<ObjectPtr>::iterator anObjectIter = anObjectList.begin();
////  while (anObjectIter != anObjectList.end()) {
////    for (size_t i = 0; i < MAX_POINTS; i++)
////      aPoints[i].clear();
////    anEntities.clear();
////
////    for (size_t i = 0; i <= myNumberOfCopies && anObjectIter != anObjectList.end(); i++, anObjectIter++) {
////      aFeature = ModelAPI_Feature::feature(*anObjectIter);
////      if (!aFeature)
////        continue;
////      anEntityID = changeEntity(aFeature, aType);
////      anEntities.push_back(anEntityID);
////      Slvs_Entity anEntity = myStorage->getEntity(anEntityID);
////      switch (aType) {
////      case SLVS_E_POINT_IN_2D:
////      case SLVS_E_POINT_IN_3D:
////        aPoints[0].push_back(anEntityID);
////        break;
////      case SLVS_E_LINE_SEGMENT:
////        aPoints[0].push_back(anEntity.point[0]); // start point of line
////        aPoints[1].push_back(anEntity.point[1]); // end point of line
////        break;
////      case SLVS_E_CIRCLE:
////        aPoints[0].push_back(anEntity.point[0]); // center of circle
////        break;
////      case SLVS_E_ARC_OF_CIRCLE:
////        aPoints[0].push_back(anEntity.point[0]); // center of arc
////        aPoints[1].push_back(anEntity.point[1]); // start point of arc
////        aPoints[2].push_back(anEntity.point[2]); // end point of arc
////        break;
////      default:
////        myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
////        return;
////      }
////    }
////
////    for (size_t i = 0; i < MAX_POINTS; ++i)
////      if (!aPoints[i].empty())
////        thePoints.push_back(aPoints[i]);
////    if (!anEntities.empty())
////      theEntities.push_back(anEntities);
////  }
}

void SketchSolver_ConstraintMultiTranslation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    /// TODO: Put error message here
    return;
  }
////  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
////    update(myBaseConstraint);

  EntityWrapperPtr aStartPoint, aEndPoint;
  std::list<std::list<EntityWrapperPtr> > anEntitiesAndCopies;
  getAttributes(aStartPoint, aEndPoint, anEntitiesAndCopies);
  if (!myErrorMsg.empty())
    return;

////  // Create translation line
////  if (myTranslationLine == SLVS_E_UNKNOWN) {
////    Slvs_Entity aTranslationLine = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
////        myGroup->getWorkplaneId(), aStartPoint, aEndPoint);
////    aTranslationLine.h = myStorage->addEntity(aTranslationLine);
////    myTranslationLine = aTranslationLine.h;
////  } else {
////    Slvs_Entity aTranslationLine = myStorage->getEntity(myTranslationLine);
////    if (aTranslationLine.point[0] != aStartPoint || aTranslationLine.point[1] != aEndPoint) {
////      aTranslationLine.point[0] = aStartPoint;
////      aTranslationLine.point[1] = aEndPoint;
////      myStorage->updateEntity(aTranslationLine);
////    }
////  }

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aTransConstraints;

  std::list< std::list<EntityWrapperPtr> >::iterator anEntIt = anEntitiesAndCopies.begin();
  for (; anEntIt != anEntitiesAndCopies.end(); ++anEntIt) {
    std::list<ConstraintWrapperPtr> aNewConstraints =
        aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID, myType,
        0.0, aStartPoint, aEndPoint, *anEntIt);
    aTransConstraints.insert(aTransConstraints.end(), aNewConstraints.begin(), aNewConstraints.end());
  }
  myStorage->addConstraint(myBaseConstraint, aTransConstraints);

  myAdjusted = false;
////  processEntities(anEntitiesAndCopies);
  adjustConstraint();
}

////void SketchSolver_ConstraintMultiTranslation::adjustConstraint()
////{
////  if (myAdjusted)
////    return;
////
////  Slvs_Entity aTranslationLine = myStorage->getEntity(myTranslationLine);
////  Slvs_hConstraint aFixed; // temporary variable
////  // Set the translation line unchanged during constraint recalculation
////  for (int i = 0; i < 2; i++) {
////    if (myStorage->isPointFixed(aTranslationLine.point[i], aFixed, true))
////      continue;
////    Slvs_Constraint aConstraint = Slvs_MakeConstraint(
////        SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_WHERE_DRAGGED, myGroup->getWorkplaneId(), 0.0,
////        aTranslationLine.point[i], SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN);
////    aConstraint.h = myStorage->addConstraint(aConstraint);
////    myStorage->addTemporaryConstraint(aConstraint.h);
////  }
////
////  // Check if the distance between point is 0, no need to resolve constraints (just wait another values)
////  double aXY[4];
////  for (int i = 0; i < 2; i++) {
////    Slvs_Entity aPnt = myStorage->getEntity(aTranslationLine.point[i]);
////    aXY[2*i] = myStorage->getParameter(aPnt.param[0]).val;
////    aXY[2*i+1] = myStorage->getParameter(aPnt.param[1]).val;
////  }
////  myDelta[0] = aXY[2] - aXY[0];
////  myDelta[1] = aXY[3] - aXY[1];
////  if (fabs(myDelta[0]) + fabs(myDelta[1]) < tolerance) {
////    myStorage->setNeedToResolve(false);
////    return;
////  }
////
////  SketchSolver_ConstraintMulti::adjustConstraint();
////}
////
////void SketchSolver_ConstraintMultiTranslation::getRelative(
////    double theAbsX, double theAbsY, double& theRelX, double& theRelY)
////{
////  theRelX = theAbsX;
////  theRelY = theAbsY;
////}
////
////void SketchSolver_ConstraintMultiTranslation::getAbsolute(
////    double theRelX, double theRelY, double& theAbsX, double& theAbsY)
////{
////  theAbsX = theRelX;
////  theAbsY = theRelY;
////}
////
////void SketchSolver_ConstraintMultiTranslation::transformRelative(double& theX, double& theY)
////{
////  // translate coordinates
////  theX += myDelta[0];
////  theY += myDelta[1];
////}

const std::string& SketchSolver_ConstraintMultiTranslation::nameNbObjects()
{
  return SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID();
}
