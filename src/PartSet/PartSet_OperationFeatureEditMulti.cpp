// File:        PartSet_OperationFeatureEditMulti.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureEditMulti.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <ModuleBase_OperationDescription.h>
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

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationFeatureEditMulti::PartSet_OperationFeatureEditMulti(const QString& theId,
	                                          QObject* theParent,
                                              FeaturePtr theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature), myIsBlockedSelection(false)
{
}

PartSet_OperationFeatureEditMulti::~PartSet_OperationFeatureEditMulti()
{
}

bool PartSet_OperationFeatureEditMulti::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

void PartSet_OperationFeatureEditMulti::initSelection(const std::list<ModuleBase_ViewerPrs>& theSelected,
                                                      const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
  if (!theHighlighted.empty()) {
    // if there is highlighted object, we check whether it is in the list of selected objects
    // in that case this object is a handle of the moved lines. If there no such object in the selection,
    // the hightlighted object should moved and the selection is skipped. The skipped selection will be
    // deselected in the viewer by blockSelection signal in the startOperation method.
    bool isSelected = false;
    std::list<ModuleBase_ViewerPrs>::const_iterator anIt = theSelected.begin(), aLast = theSelected.end();
    // TODO
    /*for (; anIt != aLast && !isSelected; anIt++) {
      isSelected = (*anIt).feature() == feature();
    }*/
    if (!isSelected)
      myFeatures = theHighlighted;
    else
      myFeatures = theSelected;
  }
  else
    myFeatures = theSelected;
}

void PartSet_OperationFeatureEditMulti::initFeature(FeaturePtr theFeature)
{
  setEditingFeature(theFeature);
}

FeaturePtr PartSet_OperationFeatureEditMulti::sketch() const
{
  return mySketch;
}

void PartSet_OperationFeatureEditMulti::mousePressed(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                             const std::list<ModuleBase_ViewerPrs>& /*theSelected*/,
                                             const std::list<ModuleBase_ViewerPrs>& theHighlighted)
{
}

void PartSet_OperationFeatureEditMulti::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
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

    std::list<ModuleBase_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast = myFeatures.end();
    // TODO
    /*for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = (*anIt).feature();
      if (!aFeature || aFeature == feature())
        continue;
      aSketchFeature = boost::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
      aSketchFeature->move(aDeltaX, aDeltaY);
    }*/
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationFeatureEditMulti::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                              const std::list<ModuleBase_ViewerPrs>& /*theSelected*/,
                                              const std::list<ModuleBase_ViewerPrs>& /*theHighlighted*/)
{
  std::list<ModuleBase_ViewerPrs> aFeatures = myFeatures;
  commit();
  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  // TODO
  /*for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = (*anIt).feature();
    if (aFeature) {
      emit featureConstructed(aFeature, FM_Deactivation);
	}
  }*/
}

void PartSet_OperationFeatureEditMulti::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  emit multiSelectionEnabled(false);

  blockSelection(true);

  myCurPoint.clear();
}

void PartSet_OperationFeatureEditMulti::stopOperation()
{
  emit multiSelectionEnabled(true);

  blockSelection(false, true);

  myFeatures.clear();
}

void PartSet_OperationFeatureEditMulti::blockSelection(bool isBlocked, const bool isRestoreSelection)
{
  if (myIsBlockedSelection == isBlocked)
    return;

  myIsBlockedSelection = isBlocked;
  QList<ObjectPtr> aFeatureList;
  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = myFeatures.begin(),
                                            aLast = myFeatures.end();
  /*for(; anIt != aLast; anIt++)
    aFeatureList.append((*anIt).feature());*/
  if (isBlocked) {
    emit setSelection(QList<ObjectPtr>());
    emit stopSelection(aFeatureList, true);
  }
  else {
    emit stopSelection(aFeatureList, false);
    if (isRestoreSelection) {
      emit setSelection(aFeatureList);
    }
  }
}

void PartSet_OperationFeatureEditMulti::sendFeatures()
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);

  std::list<FeaturePtr > aFeatures;
  std::list<ModuleBase_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast = myFeatures.end();
  // TODO
  /*for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = (*anIt).feature();
    if (!aFeature)
      continue;

    ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
  }*/
  Events_Loop::loop()->flush(anEvent);
  flushUpdated();
}

