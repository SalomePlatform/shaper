// File:        PartSet_OperationEditLine.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationEditLine.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <SketchPlugin_Line.h>

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

bool PartSet_OperationEditLine::isGranted() const
{
  return true;
}

std::list<int> PartSet_OperationEditLine::getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const
{
  std::list<int> aModes;
  aModes.push_back(TopAbs_VERTEX);
  aModes.push_back(TopAbs_EDGE);
  return aModes;
}

void PartSet_OperationEditLine::init(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  setFeature(theFeature);
}

void PartSet_OperationEditLine::mousePressed(const gp_Pnt& thePoint, QMouseEvent* theEvent)
{
  if (!(theEvent->buttons() &  Qt::LeftButton))
    return;
  myCurPressed = thePoint;
}

void PartSet_OperationEditLine::mouseMoved(const gp_Pnt& thePoint, QMouseEvent* theEvent)
{
  if (!(theEvent->buttons() &  Qt::LeftButton))
    return;

  double aCurX, aCurY;
  PartSet_Tools::ConvertTo2D(myCurPressed, mySketch, aCurX, aCurY);

  double aX, anY;
  PartSet_Tools::ConvertTo2D(thePoint, mySketch, aX, anY);

  double aDeltaX = aX - aCurX;
  double aDeltaY = anY - aCurY;

  moveLinePoint(aDeltaX, aDeltaY, LINE_ATTR_START);
  moveLinePoint(aDeltaX, aDeltaY, LINE_ATTR_END);
  myCurPressed = thePoint;
}

void PartSet_OperationEditLine::setSelected(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                            const TopoDS_Shape& theShape)
{
  if (theFeature == feature())
    return;

  commit();

  if (theFeature)
    emit launchOperation(PartSet_OperationEditLine::Type(), theFeature);
}

void PartSet_OperationEditLine::startOperation()
{
  // do nothing in order to do not create a new feature
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationEditLine::createFeature()
{
  // do nothing in order to do not create a new feature
  return boost::shared_ptr<ModelAPI_Feature>();
}

void  PartSet_OperationEditLine::moveLinePoint(double theDeltaX, double theDeltaY,
                                               const std::string& theAttribute)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));

  aPoint->setValue(aPoint->x() + theDeltaX, aPoint->y() + theDeltaY);
}
