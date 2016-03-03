#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_MultiTranslation.h>

#include <ModelAPI_AttributeString.h>

#include <GeomDataAPI_Point2D.h>

void SketchSolver_ConstraintMultiTranslation::getAttributes(
    EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
    bool& theFullValue, std::list<EntityWrapperPtr>& theEntities)
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

  AttributeStringPtr aMethodTypeAttr = aData->string(SketchPlugin_MultiTranslation::VALUE_TYPE());
  theFullValue = aMethodTypeAttr->value() != "SingleValue";

  getEntities(theEntities);
}

void SketchSolver_ConstraintMultiTranslation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    /// TODO: Put error message here
    return;
  }

  EntityWrapperPtr aStartPoint, aEndPoint;
  bool aFullValue;
  std::list<EntityWrapperPtr> aBaseEntities;
  getAttributes(aStartPoint, aEndPoint, aFullValue, aBaseEntities);
  if (!myErrorMsg.empty())
    return;

  AttributeStringPtr aMethodTypeAttr =
      myBaseConstraint->data()->string(SketchPlugin_MultiTranslation::VALUE_TYPE());

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aTransConstraints;

  std::list<EntityWrapperPtr>::iterator anEntIt = aBaseEntities.begin();
  for (; anEntIt != aBaseEntities.end(); ++anEntIt) {
    std::list<ConstraintWrapperPtr> aNewConstraints =
        aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID, myType,
        0.0, aFullValue, aStartPoint, aEndPoint, std::list<EntityWrapperPtr>(1, *anEntIt));
    aTransConstraints.insert(aTransConstraints.end(), aNewConstraints.begin(), aNewConstraints.end());
  }

  myStorage->addConstraint(myBaseConstraint, aTransConstraints);

  myAdjusted = false;
  adjustConstraint();
}

const std::string& SketchSolver_ConstraintMultiTranslation::nameNbObjects()
{
  return SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID();
}

void SketchSolver_ConstraintMultiTranslation::updateLocal()
{
  DataPtr aData = myBaseConstraint->data();
  AttributePoint2DPtr aStartPointAttribute = GeomDataAPI_Point2D::getPoint2D(aData,
                                      SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributePoint2DPtr anEndPointAttribute = GeomDataAPI_Point2D::getPoint2D(aData,
                                      SketchPlugin_MultiTranslation::END_POINT_ID());
  AttributeStringPtr aMethodTypeAttr = aData->string(SketchPlugin_MultiTranslation::VALUE_TYPE());
  bool aFullValue = aMethodTypeAttr->value() != "SingleValue";

  bool aStartPointChanged = aStartPointAttribute != myStartPointAttribute;
  bool anEndPointChanged = anEndPointAttribute != myEndPointAttribute;
  bool isMethodChanged = aFullValue != myIsFullValue;

  if (aStartPointChanged)
    myStartPointAttribute = aStartPointAttribute;
  if (aStartPointChanged)
    myEndPointAttribute = anEndPointAttribute;
  if (isMethodChanged)
    myIsFullValue = aFullValue;

  if (aStartPointChanged || anEndPointChanged || isMethodChanged) {
    DataPtr aData = myBaseConstraint->data();
    std::list<ConstraintWrapperPtr> aConstraints = myStorage->constraint(myBaseConstraint);
    std::list<ConstraintWrapperPtr>::const_iterator anIt = aConstraints.begin(),
                                                    aLast = aConstraints.end();
    std::list<EntityWrapperPtr> anEntities;
    for (; anIt != aLast; anIt++) {
      ConstraintWrapperPtr aConstraint = *anIt;
      aConstraint->setIsFullValue(myIsFullValue);
      anEntities.clear();

      const std::list<EntityWrapperPtr>& aConstraintEntities = aConstraint->entities();
      std::list<EntityWrapperPtr>::const_iterator aSIt = aConstraintEntities.begin(),
                                                  aSLast = aConstraintEntities.end();
      EntityWrapperPtr aStartEntity = *aSIt++;
      if (aStartPointChanged) {
        AttributePtr aStartPointAttr = aData->attribute(SketchPlugin_MultiTranslation::START_POINT_ID());
        myStorage->update(aStartPointAttr);
        aStartEntity = myStorage->entity(aStartPointAttr);
      }
      anEntities.push_back(aStartEntity);

      EntityWrapperPtr anEndEntity = *aSIt++;
      if (anEndPointChanged) {
        AttributePtr anEndPointAttr = aData->attribute(SketchPlugin_MultiTranslation::END_POINT_ID());
        myStorage->update(anEndPointAttr);
        anEndEntity = myStorage->entity(anEndPointAttr);
      }
      anEntities.push_back(anEndEntity);

      for (; aSIt != aSLast; ++aSIt)
        anEntities.push_back(*aSIt);

      aConstraint->setEntities(anEntities);
    }
    myStorage->addConstraint(myBaseConstraint, aConstraints);

    myAdjusted = false;
  }
}

void SketchSolver_ConstraintMultiTranslation::adjustConstraint()
{
  if (myAdjusted)
    return;

  // Obtain delta between start and end points of translation
  EntityWrapperPtr aStart = myStorage->entity(
      myBaseConstraint->attribute(SketchPlugin_MultiTranslation::START_POINT_ID()));
  std::list<ParameterWrapperPtr> aStartParams = aStart->parameters();
  EntityWrapperPtr aEnd = myStorage->entity(
      myBaseConstraint->attribute(SketchPlugin_MultiTranslation::END_POINT_ID()));
  std::list<ParameterWrapperPtr> aEndParams = aEnd->parameters();

  myDelta[0] = aEndParams.front()->value() - aStartParams.front()->value();
  myDelta[1] = aEndParams.back()->value() - aStartParams.back()->value();

  if (myIsFullValue && myNumberOfCopies > 0) {
    myDelta[0] /= myNumberOfCopies;
    myDelta[1] /= myNumberOfCopies;
  }

  SketchSolver_ConstraintMulti::adjustConstraint();
}

void SketchSolver_ConstraintMultiTranslation::getRelative(
    double theAbsX, double theAbsY, double& theRelX, double& theRelY)
{
  theRelX = theAbsX;
  theRelY = theAbsY;
}

void SketchSolver_ConstraintMultiTranslation::getAbsolute(
    double theRelX, double theRelY, double& theAbsX, double& theAbsY)
{
  theAbsX = theRelX;
  theAbsY = theRelY;
}

void SketchSolver_ConstraintMultiTranslation::transformRelative(double& theX, double& theY)
{
  // translate coordinates
  theX += myDelta[0];
  theY += myDelta[1];
}

