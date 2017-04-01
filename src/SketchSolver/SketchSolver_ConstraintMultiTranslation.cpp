// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_PointWrapper.h>

#include <SketchPlugin_MultiTranslation.h>

#include <ModelAPI_AttributeString.h>

#include <GeomDataAPI_Point2D.h>

void SketchSolver_ConstraintMultiTranslation::getAttributes(
    EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
    bool& theFullValue, std::list<EntityWrapperPtr>& theEntities)
{
  DataPtr aData = myBaseConstraint->data();
  AttributeRefAttrPtr aStartPointAttr = aData->refattr(SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributeRefAttrPtr aEndPointAttr = aData->refattr(SketchPlugin_MultiTranslation::END_POINT_ID());
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

  // add owner of start and end points of Multi-Translation to the list of monitored features
  FeaturePtr anOwner = ModelAPI_Feature::feature(aStartPointAttr->attr()->owner());
  if (anOwner)
    myFeatures.insert(anOwner);
  anOwner = ModelAPI_Feature::feature(aEndPointAttr->attr()->owner());
  if (anOwner)
    myFeatures.insert(anOwner);
}

void SketchSolver_ConstraintMultiTranslation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aStartPoint, aEndPoint;
  bool aFullValue;
  std::list<EntityWrapperPtr> aBaseEntities;
  getAttributes(aStartPoint, aEndPoint, aFullValue, aBaseEntities);
  if (!myErrorMsg.empty())
    return;

  myAdjusted = false;
  adjustConstraint();

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateFeature::GROUP());
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

  if (aStartPointChanged || anEndPointChanged || isMethodChanged)
    myAdjusted = false;
}

void SketchSolver_ConstraintMultiTranslation::adjustConstraint()
{
  if (myAdjusted)
    return;

  // Obtain delta between start and end points of translation
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aStartWrapper =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(myStorage->entity(
      myBaseConstraint->attribute(SketchPlugin_MultiTranslation::START_POINT_ID())));
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aEndWrapper =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(myStorage->entity(
      myBaseConstraint->attribute(SketchPlugin_MultiTranslation::END_POINT_ID())));

  GCSPointPtr aStart = aStartWrapper->point();
  GCSPointPtr aEnd   = aEndWrapper->point();

  myDelta[0] = *(aEnd->x) - *(aStart->x);
  myDelta[1] = *(aEnd->y) - *(aStart->y);

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

