// File:        PartSet_OperationFeatureEdit.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureEdit.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_OperationFeatureEditMulti.h>

#include <SketchPlugin_Constraint.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IViewer.h>

#include <ModelAPI_Events.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

#include <SketchPlugin_Line.h>

#include <V3d_View.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_DimensionSelectionMode.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationFeatureEdit::PartSet_OperationFeatureEdit(const QString& theId,
                                                           QObject* theParent,
                                                           CompositeFeaturePtr theFeature)
    : PartSet_OperationFeatureBase(theId, theParent, theFeature),
      myIsBlockedSelection(false)
{
  myIsEditing = true;
}

PartSet_OperationFeatureEdit::~PartSet_OperationFeatureEdit()
{
}


void PartSet_OperationFeatureEdit::mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection)
{
  ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
  if(aActiveWgt && aActiveWgt->isViewerSelector()) {
    // Almost do nothing, all stuff in on PartSet_OperationFeatureBase::mouseReleased
    PartSet_OperationFeatureBase::mousePressed(theEvent, theViewer, theSelection);
    return;
  }
  QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
  QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();
  bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
  if (aHasShift && !aHighlighted.empty()) {
    foreach (ModuleBase_ViewerPrs aPrs, aHighlighted) {
      aSelected.append(aPrs);
    }
  }
  ObjectPtr aObject;
  if (!aSelected.empty()) {
    aObject = aSelected.first().object();
  } else {   
    if (!aHighlighted.empty())
      aObject = aHighlighted.first().object();
  }
  //if (!theHighlighted.empty())
  //  aObject = theHighlighted.front().object();
  //if (!aObject && !theSelected.empty())  // changed for a constrain
  //  aObject = theSelected.front().object();

  FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
  if (!aFeature || aFeature != feature() || (aSelected.size() > 1)) {
    if (commit()) {
      theViewer->enableSelection(true);
      emit featureConstructed(feature(), FM_Deactivation);

      // If we have selection and prehilighting with shift pressed 
      // Then we have to select all these objects and restart as multi edit operfation
      //bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
      //if (aHasShift && !theHighlighted.empty()) {
      //  QList<ObjectPtr> aSelected;
      //  std::list<ModuleBase_ViewerPrs>::const_iterator aIt;
      //  for (aIt = theSelected.cbegin(); aIt != theSelected.cend(); ++aIt)
      //    aSelected.append((*aIt).object());

      //  for (aIt = theHighlighted.cbegin(); aIt != theHighlighted.cend(); ++aIt) {
      //    if (!aSelected.contains((*aIt).object()))
      //      aSelected.append((*aIt).object());
      //  }
      //  emit setSelection(aSelected);
      //} else 
      if (aFeature) {
        std::string anOperationType =
            (aSelected.size() > 1) ?
                PartSet_OperationFeatureEditMulti::Type() : PartSet_OperationFeatureEdit::Type();
        restartOperation(anOperationType, aFeature);
      }
      //}
    }
  }
}

void PartSet_OperationFeatureEdit::mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer)
{
  if (!(theEvent->buttons() & Qt::LeftButton))
    return;
  Handle(V3d_View) aView = theViewer->activeView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);

  theViewer->enableSelection(false);

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
    // MPV: added condition because it could be external edge of some object, not sketch
    if (aSketchFeature && aSketchFeature->sketch() == sketch().get()) {
      aSketchFeature->move(aDeltaX, aDeltaY);
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
      ModelAPI_EventCreator::get()->sendUpdated(feature(), anEvent);
    }
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationFeatureEdit::mouseReleased(
    QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
    ModuleBase_ISelection* theSelection)
{
  theViewer->enableSelection(true);
  ModuleBase_ModelWidget* aActiveWgt = 0;
  if (myPropertyPanel)
    aActiveWgt = myPropertyPanel->activeWidget();
  if(aActiveWgt && aActiveWgt->isViewerSelector()) {
    // Almost do nothing, all stuff in on PartSet_OperationFeatureBase::mouseReleased
    PartSet_OperationFeatureBase::mouseReleased(theEvent, theViewer, theSelection);
  }// else {
    //blockSelection(false);
  //}
}

void PartSet_OperationFeatureEdit::mouseDoubleClick(
    QMouseEvent* theEvent, Handle_V3d_View theView,
    ModuleBase_ISelection* theSelection)
{
  // TODO the functionality is important only for constraint feature. Should be moved in another place
  QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
  if (!aSelected.empty()) {
    ModuleBase_ViewerPrs aFeaturePrs = aSelected.first();
    if (!aFeaturePrs.owner().IsNull()) {
      Handle(AIS_DimensionOwner) anOwner = Handle(AIS_DimensionOwner)::DownCast(
          aFeaturePrs.owner());
      if (!anOwner.IsNull() && anOwner->SelectionMode() == AIS_DSM_Text) {
        bool isValid;
        double aValue = PartSet_Tools::featureValue(feature(), SketchPlugin_Constraint::VALUE(),
                                                    isValid);
        if (isValid) {
          ModuleBase_WidgetEditor::editFeatureValue(feature(), SketchPlugin_Constraint::VALUE());
          flushUpdated();
        }
      }
    }
  }
}

void PartSet_OperationFeatureEdit::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //emit multiSelectionEnabled(false);

  myCurPoint.clear();
}

void PartSet_OperationFeatureEdit::stopOperation()
{
  //emit multiSelectionEnabled(true);

  //blockSelection(false, false);
}

//void PartSet_OperationFeatureEdit::blockSelection(bool isBlocked, const bool isRestoreSelection)
//{
//  if (myIsBlockedSelection == isBlocked)
//    return;
//
//  myIsBlockedSelection = isBlocked;
//  QList<ObjectPtr> aFeatureList;
//  aFeatureList.append(feature());
//
//  //if (isBlocked) {
//  //  emit setSelection(QList<ObjectPtr>());
//  //  emit stopSelection(aFeatureList, true);
//  //} else {
//  //  emit stopSelection(aFeatureList, false);
//  //  if (isRestoreSelection)
//  //    emit setSelection(aFeatureList);
//  //}
//}

FeaturePtr PartSet_OperationFeatureEdit::createFeature(const bool theFlushMessage,
  CompositeFeaturePtr theCompositeFeature)
{
  // do nothing in order to do not create a new feature
  return FeaturePtr();
}

void PartSet_OperationFeatureEdit::sendFeatures()
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);

  FeaturePtr aFeature = feature();
  ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);

  Events_Loop::loop()->flush(anEvent);
  flushUpdated();
}

