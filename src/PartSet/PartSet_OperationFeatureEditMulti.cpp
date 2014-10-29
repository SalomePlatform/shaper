// File:        PartSet_OperationFeatureEditMulti.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureEditMulti.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_ISelection.h>

#include <ModelAPI_Events.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <ModelAPI_Events.h>

#include <Events_Loop.h>

#include <SketchPlugin_Line.h>

#include <V3d_View.hxx>

#include <QMouseEvent>
#ifdef _DEBUG
#include <QDebug>
#endif

//using namespace std;

PartSet_OperationFeatureEditMulti::PartSet_OperationFeatureEditMulti(const QString& theId,
                                                                     QObject* theParent,
                                                                     CompositeFeaturePtr theFeature)
    : PartSet_OperationSketchBase(theId, theParent),
      mySketch(theFeature),
      myIsBlockedSelection(false)
{
  myIsEditing = true;
}

PartSet_OperationFeatureEditMulti::~PartSet_OperationFeatureEditMulti()
{
}


bool isContains(const QList<ModuleBase_ViewerPrs>& theSelected, const ModuleBase_ViewerPrs& thePrs)
{
  foreach (ModuleBase_ViewerPrs aPrs, theSelected) {
    if (aPrs.object() == thePrs.object())
      return true;
  }
  return false;
}


void PartSet_OperationFeatureEditMulti::initSelection(ModuleBase_ISelection* theSelection)
{
  //if (!theHighlighted.empty()) {
  //  // if there is highlighted object, we check whether it is in the list of selected objects
  //  // in that case this object is a handle of the moved lines. If there no such object in the selection,
  //  // the hightlighted object should moved and the selection is skipped. The skipped selection will be
  //  // deselected in the viewer by blockSelection signal in the startOperation method.
  //  bool isSelected = false;
  //  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = theSelected.begin(), 
  //    aLast = theSelected.end();
  //  for (; anIt != aLast && !isSelected; anIt++) {
  //    isSelected = ModelAPI_Feature::feature((*anIt).object()) == feature();
  //  }
  //  if (!isSelected)
  //    myFeatures = theHighlighted;
  //  else
  //    myFeatures = theSelected;
  //} else
  myFeatures = theSelection->getSelected();
  QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();
  // add highlighted elements if they are not selected
  foreach (ModuleBase_ViewerPrs aPrs, aHighlighted) {
    if (!isContains(myFeatures, aPrs))
      myFeatures.append(aPrs);
  }
  // Remove current feature if it is in the list (it will be moved as main feature)
  foreach (ModuleBase_ViewerPrs aPrs, myFeatures) {
    FeaturePtr aF = ModelAPI_Feature::feature(aPrs.object());
    if (ModelAPI_Feature::feature(aPrs.object()) == feature()) {
      myFeatures.removeOne(aPrs);
      break;
    }
  }
}

CompositeFeaturePtr PartSet_OperationFeatureEditMulti::sketch() const
{
  return mySketch;
}

//void PartSet_OperationFeatureEditMulti::mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection)
//{
//}

void PartSet_OperationFeatureEditMulti::mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer)
{
  if (!(theEvent->buttons() & Qt::LeftButton))
    return;

  if (theViewer->isSelectionEnabled())
    theViewer->enableSelection(false);

  Handle(V3d_View) aView = theViewer->activeView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);

  //blockSelection(true);
  if (myCurPoint.myIsInitialized) {
    double aCurX, aCurY;
    PartSet_Tools::convertTo2D(myCurPoint.myPoint, sketch(), aView, aCurX, aCurY);

    double aX, anY;
    PartSet_Tools::convertTo2D(aPoint, sketch(), aView, aX, anY);

    double aDeltaX = aX - aCurX;
    double aDeltaY = anY - aCurY;

    boost::shared_ptr<SketchPlugin_Feature> aSketchFeature = boost::dynamic_pointer_cast<
        SketchPlugin_Feature>(feature());
    aSketchFeature->move(aDeltaX, aDeltaY);

    foreach (ModuleBase_ViewerPrs aPrs, myFeatures) {
      ObjectPtr aObject = aPrs.object();
      if (!aObject || aObject == feature())
        continue;
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (aFeature) {
        aSketchFeature = boost::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature)
          aSketchFeature->move(aDeltaX, aDeltaY);
      }
    }
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationFeatureEditMulti::mouseReleased(
    QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
    ModuleBase_ISelection* theSelection)
{
  theViewer->enableSelection(true);
  if (commit()) {
    foreach (ModuleBase_ViewerPrs aPrs, myFeatures) {
      ObjectPtr aFeature = aPrs.object();
      if (aFeature) {
        emit featureConstructed(aFeature, FM_Deactivation);
      }
    }
  }
}

void PartSet_OperationFeatureEditMulti::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //emit multiSelectionEnabled(false);

  //blockSelection(true);

  myCurPoint.clear();
}

void PartSet_OperationFeatureEditMulti::stopOperation()
{
  //emit multiSelectionEnabled(true);

  //blockSelection(false, true);

  myFeatures.clear();
}

//void PartSet_OperationFeatureEditMulti::blockSelection(bool isBlocked,
//                                                       const bool isRestoreSelection)
//{
//  if (myIsBlockedSelection == isBlocked)
//    return;
//
//  myIsBlockedSelection = isBlocked;
//  QList<ObjectPtr> aFeatureList;
////  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast =
////      myFeatures.end();
//  /*for(; anIt != aLast; anIt++)
//   aFeatureList.append((*anIt).feature());*/
//  //if (isBlocked) {
//  //  emit setSelection(QList<ObjectPtr>());
//  //  emit stopSelection(aFeatureList, true);
//  //} else {
//  //  emit stopSelection(aFeatureList, false);
//  //  if (isRestoreSelection) {
//  //    emit setSelection(aFeatureList);
//  //  }
//  //}
//}

void PartSet_OperationFeatureEditMulti::sendFeatures()
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);

  foreach (ModuleBase_ViewerPrs aPrs, myFeatures) {
    ObjectPtr aFeature = aPrs.object();
    if (!aFeature)
      continue;

    ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
  }
  Events_Loop::loop()->flush(anEvent);
  flushUpdated();
}

