// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchLine.h>

#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <SketchPlugin_Line.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_OperationSketchLine::PartSet_OperationSketchLine(const QString& theId,
	                                          QObject* theParent,
                                              boost::shared_ptr<ModelAPI_Feature> theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature),
  myPointSelectionMode(SM_FirstPoint)
{
}

PartSet_OperationSketchLine::~PartSet_OperationSketchLine()
{
}

bool PartSet_OperationSketchLine::isGranted() const
{
  return true;
}

std::list<int> PartSet_OperationSketchLine::getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const
{
  std::list<int> aModes;
  if (theFeature != feature()) {
    aModes.push_back(TopAbs_VERTEX);
    aModes.push_back(TopAbs_EDGE);
  }
  return aModes;
}

void PartSet_OperationSketchLine::mouseReleased(const gp_Pnt& thePoint, QMouseEvent* /*theEvent*/)
{
  switch (myPointSelectionMode)
  {
    case SM_FirstPoint: {
      setLinePoint(thePoint, LINE_ATTR_START);
      myPointSelectionMode = SM_SecondPoint;
    }
    break;
    case SM_SecondPoint: {
      setLinePoint(thePoint, LINE_ATTR_END);
      myPointSelectionMode = SM_None;
    }
    break;
    case SM_None: {
    }
    break;
    default:
      break;
  }
}

void PartSet_OperationSketchLine::mouseMoved(const gp_Pnt& thePoint, QMouseEvent* /*theEvent*/)
{
  switch (myPointSelectionMode)
  {
    case SM_SecondPoint:
      setLinePoint(thePoint, LINE_ATTR_END);
      break;
    case SM_None: {
      boost::shared_ptr<ModelAPI_Feature> aPrevFeature = feature();
      // stop the last operation
      commitOperation();
      document()->finishOperation();
      //emit changeSelectionMode(aPrevFeature, TopAbs_VERTEX);
      // start a new operation
      document()->startOperation();
      startOperation();
      // use the last point of the previous feature as the first of the new one
      setLinePoint(aPrevFeature, LINE_ATTR_END, LINE_ATTR_START);
      myPointSelectionMode = SM_SecondPoint;

      emit featureConstructed(aPrevFeature, FM_Deactivation);
    }
    break;
    default:
      break;
  }
}

void PartSet_OperationSketchLine::keyReleased(const int theKey)
{
  switch (theKey) {
    case Qt::Key_Escape: {
      if (myPointSelectionMode != SM_None)
        emit featureConstructed(feature(), FM_Abort);
      abort();
    }
    break;
    case Qt::Key_Return: {
      if (myPointSelectionMode != SM_None) {
        emit featureConstructed(feature(), FM_Abort);
        myPointSelectionMode = SM_FirstPoint;
        document()->abortOperation();
      }
      else
        myPointSelectionMode = SM_FirstPoint;
    }
    break;
    default:
    break;
  }
}

void PartSet_OperationSketchLine::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  myPointSelectionMode = SM_FirstPoint;
}

void PartSet_OperationSketchLine::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  myPointSelectionMode = SM_None;
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationSketchLine::createFeature()
{
  boost::shared_ptr<ModelAPI_Feature> aNewFeature = ModuleBase_Operation::createFeature();
  if (mySketch) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(mySketch);

    aFeature->addSub(aNewFeature);
  }
  emit featureConstructed(aNewFeature, FM_Activation);
  return aNewFeature;
}

void PartSet_OperationSketchLine::setLinePoint(const gp_Pnt& thePoint,
                                               const std::string& theAttribute)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));

  double aX, anY;
  PartSet_Tools::ConvertTo2D(thePoint, mySketch, aX, anY);
  aPoint->setValue(aX, anY);
}

void PartSet_OperationSketchLine::setLinePoint(boost::shared_ptr<ModelAPI_Feature> theSourceFeature,
                                               const std::string& theSourceAttribute,
                                               const std::string& theAttribute)
{
  boost::shared_ptr<ModelAPI_Data> aData = theSourceFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theSourceAttribute));
  double aX = aPoint->x();
  double anY = aPoint->y();

  aData = feature()->data();
  aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  aPoint->setValue(aX, anY);
}
