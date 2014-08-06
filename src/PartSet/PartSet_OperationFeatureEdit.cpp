// File:        PartSet_OperationFeatureEdit.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureEdit.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Constraint.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_ViewerPrs.h>

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
                                              FeaturePtr theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature), myIsBlockedSelection(false)
{
}

PartSet_OperationFeatureEdit::~PartSet_OperationFeatureEdit()
{
}

bool PartSet_OperationFeatureEdit::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

std::list<int> PartSet_OperationFeatureEdit::getSelectionModes(ObjectPtr theFeature) const
{
  return PartSet_OperationSketchBase::getSelectionModes(theFeature);
}

void PartSet_OperationFeatureEdit::initFeature(FeaturePtr theFeature)
{
  setEditingFeature(theFeature);
}


FeaturePtr PartSet_OperationFeatureEdit::sketch() const
{
  return mySketch;
}

void PartSet_OperationFeatureEdit::mousePressed(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                             const std::list<ModuleBase_ViewerPrs>& theSelected,
                                             const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
  ObjectPtr aObject;
  if (!theHighlighted.empty())
    aObject = theHighlighted.front().object();
  if (!aObject && !theSelected.empty()) // changed for a constrain
    aObject = theSelected.front().object();

  FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
  if (!aFeature || aFeature != feature()) {
    if (commit()) {
      emit featureConstructed(feature(), FM_Deactivation);

      bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
      if(aHasShift && !theHighlighted.empty()) {
        QList<ObjectPtr> aSelected;
        std::list<ModuleBase_ViewerPrs>::const_iterator aIt;
        for (aIt = theSelected.cbegin(); aIt != theSelected.cend(); ++aIt)
          aSelected.append((*aIt).object());
        /*for (aIt = theHighlighted.cbegin(); aIt != theHighlighted.cend(); ++aIt) {
          if (!aSelected.contains((*aIt).object()))
            aSelected.append((*aIt).object());
        }*/
        //aSelected.push_back(feature());
        //aSelected.push_back(theHighlighted.front().object());
        emit setSelection(aSelected);
      }
      else if (aFeature) {
        restartOperation(PartSet_OperationFeatureEdit::Type(), aFeature);
      }
    }
  }
}

void PartSet_OperationFeatureEdit::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
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

    double aDeltaX = aX - aCurX;
    double aDeltaY = anY - aCurY;

    boost::shared_ptr<SketchPlugin_Feature> aSketchFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(feature());
    aSketchFeature->move(aDeltaX, aDeltaY);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendUpdated(feature(), anEvent);
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationFeatureEdit::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                              const std::list<ModuleBase_ViewerPrs>& /*theSelected*/,
                                              const std::list<ModuleBase_ViewerPrs>& /*theHighlighted*/)
{
  blockSelection(false);
}

void PartSet_OperationFeatureEdit::mouseDoubleClick(QMouseEvent* theEvent, Handle_V3d_View theView,
                                                    const std::list<ModuleBase_ViewerPrs>& theSelected,
                                                    const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
  // TODO the functionality is important only for constraint feature. Should be moved in another place
  if (!theSelected.empty()) {
    ModuleBase_ViewerPrs aFeaturePrs = theSelected.front();
    if (!aFeaturePrs.owner().IsNull()) {
      Handle(AIS_DimensionOwner) anOwner = Handle(AIS_DimensionOwner)::DownCast(aFeaturePrs.owner());
      if (!anOwner.IsNull() && anOwner->SelectionMode() == AIS_DSM_Text) {
        bool isValid;
        double aValue = PartSet_Tools::featureValue(feature(), SketchPlugin_Constraint::VALUE(), isValid);
        if (isValid) {
          ModuleBase_WidgetEditor::editFeatureValue(feature(), SketchPlugin_Constraint::VALUE());
          flushUpdated();
        }
      }
    }
  }
}


void PartSet_OperationFeatureEdit::keyReleased(const int theKey)
{
  if (theKey == Qt::Key_Return) {
    commit();
  } else 
    PartSet_OperationSketchBase::keyReleased(theKey);
}

void PartSet_OperationFeatureEdit::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  emit multiSelectionEnabled(false);

  myCurPoint.clear();
}

void PartSet_OperationFeatureEdit::stopOperation()
{
  emit multiSelectionEnabled(true);

  blockSelection(false, false);
}

void PartSet_OperationFeatureEdit::blockSelection(bool isBlocked, const bool isRestoreSelection)
{
  if (myIsBlockedSelection == isBlocked)
    return;

  myIsBlockedSelection = isBlocked;
  QList<ObjectPtr> aFeatureList;
  aFeatureList.append(feature());

  if (isBlocked) {
    emit setSelection(QList<ObjectPtr>());
    emit stopSelection(aFeatureList, true);
  }
  else {
    emit stopSelection(aFeatureList, false);
    if (isRestoreSelection)
      emit setSelection(aFeatureList);
  }
}

FeaturePtr PartSet_OperationFeatureEdit::createFeature(const bool /*theFlushMessage*/)
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

