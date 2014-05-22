// File:        PartSet_OperationEditLine.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationEditLine.h>
#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

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

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationEditLine::PartSet_OperationEditLine(const QString& theId,
	                                          QObject* theParent,
                                              boost::shared_ptr<ModelAPI_Feature> theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature)
{
}

PartSet_OperationEditLine::~PartSet_OperationEditLine()
{
}

bool PartSet_OperationEditLine::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

std::list<int> PartSet_OperationEditLine::getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const
{
  return PartSet_OperationSketchBase::getSelectionModes(theFeature);
}

void PartSet_OperationEditLine::init(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                     const std::list<XGUI_ViewerPrs>& thePresentations)
{
  setFeature(theFeature);
  myFeatures = thePresentations;
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationEditLine::sketch() const
{
  return mySketch;
}

void PartSet_OperationEditLine::mousePressed(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  if (!(theEvent->buttons() &  Qt::LeftButton))
    return;
  gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(theEvent->pos(), theView);
  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationEditLine::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  if (!(theEvent->buttons() &  Qt::LeftButton))
    return;

  gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(theEvent->pos(), theView);

  if (myCurPoint.myIsInitialized) {
    double aCurX, aCurY;
    PartSet_Tools::ConvertTo2D(myCurPoint.myPoint, sketch(), theView, aCurX, aCurY);

    double aX, anY;
    PartSet_Tools::ConvertTo2D(aPoint, sketch(), theView, aX, anY);

    double aDeltaX = aX - aCurX;
    double aDeltaY = anY - aCurY;

    moveLinePoint(feature(), aDeltaX, aDeltaY, LINE_ATTR_START);
    moveLinePoint(feature(), aDeltaX, aDeltaY, LINE_ATTR_END);

    std::list<XGUI_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast = myFeatures.end();
    for (; anIt != aLast; anIt++) {
      boost::shared_ptr<ModelAPI_Feature> aFeature = (*anIt).feature();
      if (!aFeature || aFeature == feature())
        continue;
      moveLinePoint(aFeature, aDeltaX, aDeltaY, LINE_ATTR_START);
      moveLinePoint(aFeature, aDeltaX, aDeltaY, LINE_ATTR_END);
    }
  }
  sendFeatures();

  myCurPoint.setPoint(aPoint);
}

void PartSet_OperationEditLine::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                              const std::list<XGUI_ViewerPrs>& theSelected)
{
  std::list<XGUI_ViewerPrs> aFeatures = myFeatures;
  if (myFeatures.size() == 1) {
    if (theSelected.empty())
      return;

    boost::shared_ptr<ModelAPI_Feature> aFeature = theSelected.front().feature();
    commit();
    emit launchOperation(PartSet_OperationEditLine::Type(), aFeature);
  }
  else {
    commit();
    std::list<XGUI_ViewerPrs>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
    for (; anIt != aLast; anIt++) {
      boost::shared_ptr<ModelAPI_Feature> aFeature = (*anIt).feature();
      if (aFeature)
        emit featureConstructed(aFeature, FM_Deactivation);
    }
  }
}

void PartSet_OperationEditLine::startOperation()
{
  // do nothing in order to do not create a new feature
  emit multiSelectionEnabled(false);
  emit setSelection(std::list<XGUI_ViewerPrs>());
  emit stopSelection(myFeatures, true);
  myCurPoint.clear();
}

void PartSet_OperationEditLine::stopOperation()
{
  emit multiSelectionEnabled(true);
  bool isSelectFeatures = myFeatures.size() > 1;
  emit stopSelection(myFeatures, false);
  if (isSelectFeatures)
    emit setSelection(myFeatures);

  myFeatures.clear();
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationEditLine::createFeature()
{
  // do nothing in order to do not create a new feature
  return boost::shared_ptr<ModelAPI_Feature>();
}

void PartSet_OperationEditLine::moveLinePoint(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                               double theDeltaX, double theDeltaY,
                                               const std::string& theAttribute)
{
  if (!theFeature || theFeature->getKind() != "SketchLine")
    return;

  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));

  aPoint->setValue(aPoint->x() + theDeltaX, aPoint->y() + theDeltaY);
}

void PartSet_OperationEditLine::sendFeatures()
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_MOVED);

  std::list<boost::shared_ptr<ModelAPI_Feature> > aFeatures;
  std::list<XGUI_ViewerPrs>::const_iterator anIt = myFeatures.begin(), aLast = myFeatures.end();
  for (; anIt != aLast; anIt++) {
    boost::shared_ptr<ModelAPI_Feature> aFeature = (*anIt).feature();
    if (!aFeature || aFeature == feature())
      continue;

    Model_FeatureUpdatedMessage aMessage(aFeature, anEvent);
    Events_Loop::loop()->send(aMessage);
  }
  Events_Loop::loop()->flush(anEvent);
}

