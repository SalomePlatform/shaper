// File:        PartSet_OperationEditLine.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationEditLine.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>

#ifdef _DEBUG
#include <QDebug>
#endif

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
  //if (theFeature != feature())
  //  aModes.push_back(TopAbs_VERTEX);
  return aModes;
}

void PartSet_OperationEditLine::init(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  setFeature(theFeature);
}

void PartSet_OperationEditLine::mouseReleased(const gp_Pnt& thePoint)
{
  /*switch (myPointSelectionMode)
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
*/
}

void PartSet_OperationEditLine::mouseMoved(const gp_Pnt& thePoint)
{
/*  switch (myPointSelectionMode)
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
*/
}

void PartSet_OperationEditLine::keyReleased(const int theKey)
{
/*  switch (theKey) {
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
  */
}

void PartSet_OperationEditLine::setSelected(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                            const TopoDS_Shape& theShape)
{
  if (theFeature == feature())
    return;

  commit();

  if (theFeature)
    emit launchOperation("EditLine", theFeature);
}

void PartSet_OperationEditLine::startOperation()
{
  //PartSet_OperationSketchBase::startOperation();
  //myPointSelectionMode = SM_FirstPoint;
}

void PartSet_OperationEditLine::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  //myPointSelectionMode = SM_None;
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationEditLine::createFeature()
{
  return boost::shared_ptr<ModelAPI_Feature>();
}

void PartSet_OperationEditLine::setLinePoint(const gp_Pnt& thePoint,
                                               const std::string& theAttribute)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));

  double aX = 0;
  double anY = 0;
  convertTo2D(thePoint, aX, anY);
  aPoint->setValue(aX, anY);
}

void PartSet_OperationEditLine::setLinePoint(boost::shared_ptr<ModelAPI_Feature> theSourceFeature,
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

void PartSet_OperationEditLine::convertTo2D(const gp_Pnt& thePoint, double& theX, double& theY)
{
  if (!mySketch)
    return;

  boost::shared_ptr<ModelAPI_AttributeDouble> anAttr;
  boost::shared_ptr<ModelAPI_Data> aData = mySketch->data();

  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));

  boost::shared_ptr<GeomDataAPI_Dir> aX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRX));
  boost::shared_ptr<GeomDataAPI_Dir> anY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRY));

  gp_Pnt aVec(thePoint.X() - anOrigin->x(), thePoint.Y() - anOrigin->y(), thePoint.Z() - anOrigin->z());
  theX = aVec.X() * aX->x() + aVec.Y() * aX->y() + aVec.Z() * aX->z();
  theY = aVec.X() * anY->x() + aVec.Y() * anY->y() + aVec.Z() * anY->z();
}
