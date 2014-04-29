// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchLine.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>

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

int PartSet_OperationSketchLine::getSelectionMode() const
{
  return 0;//TopAbs_FACE;
}

void PartSet_OperationSketchLine::mouseReleased(const gp_Pnt& thePoint)
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
      commit();
    }
    break;
    case SM_None: {

    }
    break;
    default:
      break;
  }
}

void PartSet_OperationSketchLine::mouseMoved(const gp_Pnt& thePoint)
{
  if (myPointSelectionMode == SM_SecondPoint)
    setLinePoint(thePoint, LINE_ATTR_END);
}

void PartSet_OperationSketchLine::startOperation()
{
  PartSet_OperationSketchBase::startOperation();

  if (mySketch) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(mySketch);

    aFeature->addSub(feature());
  }
  myPointSelectionMode = SM_FirstPoint;
}

void PartSet_OperationSketchLine::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();

  myPointSelectionMode = SM_None;
}

void PartSet_OperationSketchLine::setLinePoint(const gp_Pnt& thePoint,
                                               const std::string& theAttribute)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  double aX = thePoint.X(), anY = thePoint.Y();
  aPoint->setValue(aX, anY);
}

