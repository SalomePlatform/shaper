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

#include <SketchSolver_ConstraintMulti.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_IntersectionPoint.h>

static void createCopiedEntity(const FeaturePtr& theFeature, const StoragePtr& theStorage)
{
  EntityWrapperPtr anEntity = theStorage->entity(theFeature);
  if (anEntity) {
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
    if (!anEntity->isExternal() && aSketchFeature->isCopy())
      theStorage->makeExternal(anEntity);
  }
}

void SketchSolver_ConstraintMulti::getEntities(std::list<EntityWrapperPtr>& theEntities)
{
  myAdjusted = false;

  // Lists of objects and number of copies
  AttributeRefListPtr anInitialRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_A());
  myNumberOfObjects = anInitialRefList->size();
  myNumberOfCopies = myBaseConstraint->integer(nameNbObjects())->value() - 1;
  if (myNumberOfCopies <= 0)
    return;

  AttributeRefListPtr aRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (!aRefList || aRefList->size() == 0) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr aFeature;
  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
  // execute for the feature is not called yet
  if ((myNumberOfCopies + 1) * myNumberOfObjects != aRefList->size())
    myNumberOfCopies = aRefList->size() / myNumberOfObjects - 1;

  while (anObjIt != anObjectList.end()) {
    aFeature = ModelAPI_Feature::feature(*anObjIt++);
    if (!aFeature)
      continue;

    // the entity is not created, so it is a copy in "multi" constraint, force its creation
    if (!myStorage->update(aFeature))
      myStorage->update(aFeature, true);
    theEntities.push_back(myStorage->entity(aFeature));
    myOriginalFeatures.insert(aFeature);
    for (int i = 0; i < myNumberOfCopies && anObjIt != anObjectList.end(); ++i, ++anObjIt) {
      // just add copied features into the list of objects
      aFeature = ModelAPI_Feature::feature(*anObjIt);
      if (aFeature) {
        createCopiedEntity(aFeature, myStorage);
        myCopiedFeatures.insert(aFeature);
      }
    }
  }
}

bool SketchSolver_ConstraintMulti::remove()
{
  myStorage->unsubscribeUpdates(this);

  // "Multi" constraint has been removed, thus all copy features become non-copied,
  // add them once again to be a common feature
  std::set<FeaturePtr>::iterator anIt = myCopiedFeatures.begin();
  for (; anIt != myCopiedFeatures.end(); ++anIt) {
    EntityWrapperPtr anEntity = myStorage->entity(*anIt);
    if (anEntity) {
      std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
      if (anEntity->isExternal() && !aSketchFeature->isExternal())
        myStorage->makeNonExternal(anEntity);
    } else if ((*anIt)->data() && (*anIt)->data()->isValid())
      myStorage->update(*anIt, true);
  }

  myOriginalFeatures.clear();
  myCopiedFeatures.clear();
  return SketchSolver_Constraint::remove();
}

void SketchSolver_ConstraintMulti::update()
{
  cleanErrorMsg();
  AttributeRefListPtr anInitialRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_A());
  AttributeIntegerPtr aNbObjects = myBaseConstraint->integer(nameNbObjects());
  if (!anInitialRefList || !aNbObjects)
    return; // the "Multi" constraint is in queue to remove
  bool isUpdated =
    anInitialRefList->size() != myNumberOfObjects || aNbObjects->value()-1 != myNumberOfCopies;
  if (!isUpdated) {
    // additional check that the features and their copies are changed
    AttributeRefListPtr aRefList =
        myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
    if (aRefList && aRefList->size() != 0) {
      FeaturePtr aFeature;
      std::list<ObjectPtr> anObjectList = aRefList->list();
      std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
      for (; anObjIt != anObjectList.end(); ++anObjIt) {
        aFeature = ModelAPI_Feature::feature(*anObjIt);
        if (aFeature && myOriginalFeatures.find(aFeature) == myOriginalFeatures.end() &&
            myCopiedFeatures.find(aFeature) == myCopiedFeatures.end()) {
          isUpdated = true;
          break;
        }
      }
    } else
      isUpdated = true;
  }
  if (isUpdated) {
    remove();
    process();
  }

  // update derived object
  updateLocal();
  adjustConstraint();
}

void SketchSolver_ConstraintMulti::adjustConstraint()
{
  AttributeRefListPtr aRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
  if (!aRefList || aRefList->size() == 0) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr anOriginal, aFeature;
  std::list<double>::iterator aXIt, aYIt;

  std::list<ObjectPtr> anObjectList = aRefList->list();
  std::list<ObjectPtr>::iterator anObjIt = anObjectList.begin();
  while (anObjIt != anObjectList.end()) {
    anOriginal = ModelAPI_Feature::feature(*anObjIt++);
    if (!anOriginal)
      continue;

    // Fill lists of coordinates of points composing a feature
    std::list<double> aX, aY;
    double aXCoord, aYCoord;
    std::list<AttributePtr> aPoints =
        anOriginal->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<AttributePtr>::iterator aPtIt = aPoints.begin();
    for (; aPtIt != aPoints.end(); ++aPtIt) {
      AttributePoint2DPtr aPoint2D = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*aPtIt);
      if (aPoint2D->isInitialized()) {
        aXCoord = aPoint2D->x();
        aYCoord = aPoint2D->y();
        getRelative(aXCoord, aYCoord, aXCoord, aYCoord);
      } else
        aXCoord = aYCoord = 0;

      aX.push_back(aXCoord);
      aY.push_back(aYCoord);
    }

    // Calculate positions of copied features
    for (int i = 0; i < myNumberOfCopies && anObjIt != anObjectList.end(); ++i, ++anObjIt) {
      aFeature = ModelAPI_Feature::feature(*anObjIt);
      if (!aFeature)
        continue;

      if (myIsEventsBlocked)
        aFeature->data()->blockSendAttributeUpdated(true);

      if (aFeature->getKind() == SketchPlugin_Circle::ID()) // update circle's radius
        aFeature->real(SketchPlugin_Circle::RADIUS_ID())->setValue(
            anOriginal->real(SketchPlugin_Circle::RADIUS_ID())->value());

      aPoints = aFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
      for (aPtIt = aPoints.begin(), aXIt = aX.begin(), aYIt = aY.begin();
           aPtIt != aPoints.end(); ++aXIt, ++aYIt, ++aPtIt) {
        if (!(*aPtIt)->isInitialized())
          continue;
        transformRelative(*aXIt, *aYIt);
        getAbsolute(*aXIt, *aYIt, aXCoord, aYCoord);

        AttributePoint2DPtr aPoint2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*aPtIt);
        aPoint2D->setValue(aXCoord, aYCoord);
      }

      // update transformed entity if it exists in the storage
      if (myStorage->entity(aFeature))
        myStorage->update(aFeature);
    }
  }

  myAdjusted = true;
}

void SketchSolver_ConstraintMulti::notify(const FeaturePtr& theFeature,
                                          PlaneGCSSolver_Update*)
{
  if (myOriginalFeatures.find(theFeature) == myOriginalFeatures.end() &&
      myCopiedFeatures.find(theFeature) == myCopiedFeatures.end())
    return; // the feature is not used by constraint => nothing to update

  if (myIsProcessingNotify)
    return; // "notify" is already processing

  // do not adjust "multi"-constraint if the number of objects is changed,
  // wait until the constraint is updated (issue #2425: changing number of copies by parameter)
  if (myNumberOfCopies + 1 == myBaseConstraint->integer(nameNbObjects())->value()) {
    myIsProcessingNotify = true;

    // update derivative object
    updateLocal();
    myAdjusted = false;
    adjustConstraint();

    myIsProcessingNotify = false;
  }
}

void SketchSolver_ConstraintMulti::blockEvents(bool isBlocked)
{
  myIsEventsBlocked = isBlocked;

  std::set<FeaturePtr>::iterator anIt = myOriginalFeatures.begin();
  for (; anIt != myOriginalFeatures.end(); ++anIt)
    (*anIt)->data()->blockSendAttributeUpdated(isBlocked);
  for (anIt = myCopiedFeatures.begin(); anIt != myCopiedFeatures.end(); ++anIt)
    (*anIt)->data()->blockSendAttributeUpdated(isBlocked);

  SketchSolver_Constraint::blockEvents(isBlocked);
}
