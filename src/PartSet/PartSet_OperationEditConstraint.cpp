// File:        PartSet_OperationEditConstraint.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationEditConstraint.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_EditLine.h>
#include <PartSet_FeaturePrs.h>
#include <SketchPlugin_ConstraintLength.h>

#include <ModuleBase_OperationDescription.h>
#include <Model_Events.h>

#include <XGUI_ViewerPrs.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <Model_Events.h>

#include <Events_Loop.h>

#include <SketchPlugin_Line.h>

#include <V3d_View.hxx>
#include <AIS_LengthDimension.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationEditConstraint::PartSet_OperationEditConstraint(const QString& theId,
	                                          QObject* theParent,
                                              FeaturePtr theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature), myIsBlockedSelection(false)
{
  std::string aKind = theId.toStdString();
  myFeaturePrs = PartSet_Tools::createFeaturePrs(aKind, theFeature);

  // changed
  myEditor = new PartSet_EditLine(0);
  connect(myEditor, SIGNAL(stopped(double)), this, SLOT(onEditStopped(double)));
}

PartSet_OperationEditConstraint::~PartSet_OperationEditConstraint()
{
}

bool PartSet_OperationEditConstraint::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

std::list<int> PartSet_OperationEditConstraint::getSelectionModes(FeaturePtr theFeature) const
{
  return PartSet_OperationSketchBase::getSelectionModes(theFeature);
}

void PartSet_OperationEditConstraint::init(FeaturePtr theFeature,
                                     const std::list<XGUI_ViewerPrs>& theSelected,
                                     const std::list<XGUI_ViewerPrs>& theHighlighted)
{
  setFeature(theFeature);
  /*
  if (!theHighlighted.empty()) {
    // if there is highlighted object, we check whether it is in the list of selected objects
    // in that case this object is a handle of the moved lines. If there no such object in the selection,
    // the hightlighted object should moved and the selection is skipped. The skipped selection will be
    // deselected in the viewer by blockSelection signal in the startOperation method.
    bool isSelected = false;
    std::list<XGUI_ViewerPrs>::const_iterator anIt = theSelected.begin(), aLast = theSelected.end();
    for (; anIt != aLast && !isSelected; anIt++) {
      isSelected = (*anIt).feature() == feature();
    }
    if (!isSelected)
      myFeatures = theHighlighted;
    else
      myFeatures = theSelected;
  }
  else
    myFeatures = theSelected;*/
}

FeaturePtr PartSet_OperationEditConstraint::sketch() const
{
  return mySketch;
}

void PartSet_OperationEditConstraint::mousePressed(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                             const std::list<XGUI_ViewerPrs>& theSelected,
                                             const std::list<XGUI_ViewerPrs>& theHighlighted)
{
  //if (myFeatures.size() == 1)
  {
    FeaturePtr aFeature;
    if (!theHighlighted.empty())
      aFeature = theHighlighted.front().feature();
    if (!aFeature && !theSelected.empty()) // changed
      aFeature = theSelected.front().feature();

    if (aFeature && aFeature == feature()) { // continue the feature edit
    }
    else {
      //XGUI_ViewerPrs aFeaturePrs = myFeatures.front();
      commit();
      emit featureConstructed(feature(), FM_Deactivation);

      /*bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
      if(aHasShift && !theHighlighted.empty()) {
        std::list<XGUI_ViewerPrs> aSelected;
        aSelected.push_back(aFeaturePrs);
        aSelected.push_back(theHighlighted.front());
        emit setSelection(aSelected);
      }
      else if (aFeature) {
        restartOperation(PartSet_OperationEditConstraint::Type(), aFeature);
      }*/
    }
  }
}

void PartSet_OperationEditConstraint::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  if (!(theEvent->buttons() &  Qt::LeftButton))
    return;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);

  blockSelection(true);
  if (myCurPoint.myIsInitialized) {
    double aCurX, aCurY;
    PartSet_Tools::convertTo2D(myCurPoint.myPoint, sketch(), theView, aCurX, aCurY);

    double aX, anY;
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);

    /*double aDeltaX = aX - aCurX;
    double aDeltaY = anY - aCurY;

    PartSet_Tools::moveFeature(feature(), aDeltaX, aDeltaY);*/
    myFeaturePrs->setPoint(aX, anY, SM_SecondPoint);


    /*std::list<XGUI_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast = myFeatures.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = (*anIt).feature();
      if (!aFeature || aFeature == feature())
        continue;
      PartSet_Tools::moveFeature(aFeature, aDeltaX, aDeltaY);
    }*/
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationEditConstraint::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                              const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                              const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  //std::list<XGUI_ViewerPrs> aFeatures = myFeatures;
  //if (myFeatures.size() == 1) {
    blockSelection(false);
  /*}
  else {
    commit();
    std::list<XGUI_ViewerPrs>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = (*anIt).feature();
      if (aFeature)
        emit featureConstructed(aFeature, FM_Deactivation);
    }
  }*/
}

void PartSet_OperationEditConstraint::mouseDoubleClick(QMouseEvent* theEvent, Handle_V3d_View theView,
                                               const std::list<XGUI_ViewerPrs>& theSelected,
                                               const std::list<XGUI_ViewerPrs>& theHighlighted)
{
  // changed
  if (!theSelected.empty()) {

    double aValue;
    if(PartSet_Tools::featureValue(feature(), CONSTRAINT_ATTR_VALUE, aValue)) {
      QPoint aPos = theEvent->globalPos();
      myEditor->start(aPos, aValue);
    }
  }
}

void PartSet_OperationEditConstraint::startOperation()
{
  // do nothing in order to do not create a new feature
  emit multiSelectionEnabled(false);

  //if (myFeatures.size() > 1)
  //  blockSelection(true);

  myCurPoint.clear();
}

void PartSet_OperationEditConstraint::stopOperation()
{
  emit multiSelectionEnabled(true);

  //blockSelection(false, myFeatures.size() > 1);

  //myFeatures.clear();
}

void PartSet_OperationEditConstraint::blockSelection(bool isBlocked, const bool isRestoreSelection)
{
  if (myIsBlockedSelection == isBlocked)
    return;

  myIsBlockedSelection = isBlocked;
  if (isBlocked) {
    emit setSelection(std::list<XGUI_ViewerPrs>());
    //emit stopSelection(myFeatures, true);
  }
  else {
    //emit stopSelection(myFeatures, false);
    //if (isRestoreSelection)
    //  emit setSelection(myFeatures);
  }
}

FeaturePtr PartSet_OperationEditConstraint::createFeature(const bool /*theFlushMessage*/)
{
  // do nothing in order to do not create a new feature
  return FeaturePtr();
}

void PartSet_OperationEditConstraint::sendFeatures()
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_MOVED);

  std::list<FeaturePtr> aFeatures;
  aFeatures.push_back(feature());
  std::list<FeaturePtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  //std::list<XGUI_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast = myFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = (*anIt);//.feature();
    if (!aFeature)
      continue;

    Model_FeatureUpdatedMessage aMessage(aFeature, anEvent);
    Events_Loop::loop()->send(aMessage);
  }
  Events_Loop::loop()->flush(anEvent);
  flushUpdated();
}

void PartSet_OperationEditConstraint::onEditStopped(double theValue)
{
  PartSet_Tools::setFeatureValue(feature(), theValue, CONSTRAINT_ATTR_VALUE);

  flushUpdated();
  commit();
  emit featureConstructed(feature(), FM_Deactivation);
  //restartOperation(feature()->getKind(), FeaturePtr());
}
