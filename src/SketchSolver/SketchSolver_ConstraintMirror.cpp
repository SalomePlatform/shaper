// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateFeature.h>

#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeRefList.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>


static void mirrorPoints(const std::shared_ptr<GeomAPI_Lin2d>& theMirrorLine,
                         const AttributePoint2DPtr& theOriginal,
                         const AttributePoint2DPtr& theMirrored);

static void mirrorEntities(const std::shared_ptr<GeomAPI_Lin2d>& theMirrorLine,
                           const FeaturePtr& theOriginal,
                           const FeaturePtr& theMirrored);



void SketchSolver_ConstraintMirror::getAttributes(
    EntityWrapperPtr&,
    std::vector<EntityWrapperPtr>&)
{
  AttributeRefAttrPtr aMirLineRefAttr =
      myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  if (!aMirLineRefAttr || !aMirLineRefAttr->isInitialized() || !aMirLineRefAttr->isObject()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  FeaturePtr aMirrorLine = ModelAPI_Feature::feature(aMirLineRefAttr->object());
  myFeatures.insert(aMirrorLine);

  myType = TYPE(myBaseConstraint);
  myStorage->update(aMirrorLine);


  AttributeRefListPtr aBaseRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
  AttributeRefListPtr aMirroredRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_C());
  myNumberOfObjects = aMirroredRefList->size();
  if (!aBaseRefList || !aMirroredRefList) {
    myErrorMsg = SketchSolver_Error::INCORRECT_MIRROR_ATTRIBUTE();
    return;
  }

  // store only original entities because mirrored ones
  // will be updated separately in adjustConstraint
  std::list<ObjectPtr> aList = aBaseRefList->list();
  std::list<ObjectPtr>::iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    if (aFeature) {
      myStorage->update(aFeature);
      myFeatures.insert(aFeature);
    }
  }
}

void SketchSolver_ConstraintMirror::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aMirrorLine;
  std::vector<EntityWrapperPtr> aBaseList;
  getAttributes(aMirrorLine, aBaseList);
  if (!myErrorMsg.empty())
    return;

  adjustConstraint();
  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateFeature::GROUP());
}


void SketchSolver_ConstraintMirror::update()
{
  cleanErrorMsg();
  remove();
  process();
}

void SketchSolver_ConstraintMirror::adjustConstraint()
{
  AttributeRefAttrPtr aMirrLineRefAttr =
      myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomAPI_Lin2d> aMirrorLine =
      PlaneGCSSolver_Tools::line(ModelAPI_Feature::feature(aMirrLineRefAttr->object()));

  AttributeRefListPtr aBaseRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
  AttributeRefListPtr aMirroredRefList =
      myBaseConstraint->reflist(SketchPlugin_Constraint::ENTITY_C());

  std::list<ObjectPtr> aBaseList = aBaseRefList->list();
  std::list<ObjectPtr> aMirroredList = aMirroredRefList->list();
  std::list<ObjectPtr>::iterator aBIt, aMIt;
  for (aBIt = aBaseList.begin(), aMIt = aMirroredList.begin();
       aBIt != aBaseList.end() && aMIt != aMirroredList.end();
       ++aBIt, ++aMIt) {
    FeaturePtr aBase = ModelAPI_Feature::feature(*aBIt);
    FeaturePtr aMirrored = ModelAPI_Feature::feature(*aMIt);
    mirrorEntities(aMirrorLine, aBase, aMirrored);

    // update mirrored entity if it exists in the storage
    if (myStorage->entity(aMirrored))
      myStorage->update(aMirrored);
  }
}

void SketchSolver_ConstraintMirror::notify(const FeaturePtr& theFeature,
                                           PlaneGCSSolver_Update*)
{
  if (myFeatures.find(theFeature) == myFeatures.end())
    return; // the feature is not used by constraint => nothing to update
  adjustConstraint();
}

void SketchSolver_ConstraintMirror::blockEvents(bool isBlocked)
{
  std::set<FeaturePtr>::iterator anIt = myFeatures.begin();
  for (; anIt != myFeatures.end(); ++anIt)
    (*anIt)->data()->blockSendAttributeUpdated(isBlocked);

  SketchSolver_Constraint::blockEvents(isBlocked);
}




// =================   Auxiliary functions   ==================================
void mirrorPoints(const std::shared_ptr<GeomAPI_Lin2d>& theMirrorLine,
                  const AttributePoint2DPtr& theOriginal,
                  const AttributePoint2DPtr& theMirrored)
{
  std::shared_ptr<GeomAPI_Pnt2d> anOriginal = theOriginal->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aPtOnLine = theMirrorLine->project(anOriginal);
  std::shared_ptr<GeomAPI_XY> aPerp = aPtOnLine->xy()->decreased(anOriginal->xy());
  theMirrored->setValue(anOriginal->x() + aPerp->x() * 2.0, anOriginal->y() + aPerp->y() * 2.0);
}

void mirrorEntities(const std::shared_ptr<GeomAPI_Lin2d>& theMirrorLine,
                    const FeaturePtr& theOriginal,
                    const FeaturePtr& theMirrored)
{
  std::list<AttributePtr> aPoints0 =
      theOriginal->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr> aPoints1 =
      theMirrored->data()->attributes(GeomDataAPI_Point2D::typeId());

  // process specific features
  if (theOriginal->getKind() == SketchPlugin_Arc::ID()) {
    // orientation of arc
    theMirrored->boolean(SketchPlugin_Arc::REVERSED_ID())->setValue(
        !theOriginal->boolean(SketchPlugin_Arc::REVERSED_ID())->value());
  } else if (theOriginal->getKind() == SketchPlugin_Circle::ID()) {
    // radius of the circle
    theMirrored->real(SketchPlugin_Circle::RADIUS_ID())->setValue(
        theOriginal->real(SketchPlugin_Circle::RADIUS_ID())->value());
  }

  // mirror all initialized points of features
  std::list<AttributePtr>::iterator anIt0, anIt1;
  for (anIt0 = aPoints0.begin(), anIt1 = aPoints1.begin();
       anIt0 != aPoints0.end() && anIt1 != aPoints1.end(); ++anIt0, ++anIt1) {
    AttributePoint2DPtr aPt0 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt0);
    AttributePoint2DPtr aPt1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt1);
    if (aPt0->isInitialized() && aPt1->isInitialized())
      mirrorPoints(theMirrorLine, aPt0, aPt1);
  }
}
