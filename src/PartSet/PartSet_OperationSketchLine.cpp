// File:        PartSet_OperationSketchLine.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketchLine.h>

#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>

#include <Geom_Line.hxx>
#include <gp_Lin.hxx>

#include <XGUI_ViewerPrs.h>

#include <SketchPlugin_Line.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

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
  return std::list<int>();
}

void PartSet_OperationSketchLine::init(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!theFeature)
    return;
  // use the last point of the previous feature as the first of the new one
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  myInitPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));
}

void PartSet_OperationSketchLine::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                const std::list<XGUI_ViewerPrs>& theSelected)
{
  gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(theEvent->pos(), theView);

  if (!theSelected.empty()) {
    XGUI_ViewerPrs aPrs = theSelected.front();
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull())
          aPoint = BRep_Tool::Pnt(aVertex);
      }
      else if (aShape.ShapeType() == TopAbs_EDGE) {
        boost::shared_ptr<ModelAPI_Feature> aFeature = aPrs.feature();
        if (!aFeature)
          return;
        double X0, X1, X2, X3;
        double Y0, Y1, Y2, Y3;
        getLinePoint(aFeature, LINE_ATTR_START, X2, Y2);
        getLinePoint(aFeature, LINE_ATTR_END, X3, Y3);

        if (myPointSelectionMode == SM_SecondPoint) {
          getLinePoint(feature(), LINE_ATTR_START, X0, Y0);
          PartSet_Tools::ConvertTo2D(aPoint, mySketch, theView, X1, Y1);

          double aX, anY;
          PartSet_Tools::IntersectLines(X0, Y0, X1, Y1, X2, Y2, X3, Y3, aX, anY);

          setLinePoint(aX, anY, LINE_ATTR_END);
          commit();
          emit featureConstructed(feature(), FM_Deactivation);
          emit launchOperation(PartSet_OperationSketchLine::Type(), feature());
          return;
        }

      }
    }
  }

  switch (myPointSelectionMode)
  {
    case SM_FirstPoint: {
      setLinePoint(aPoint, theView, LINE_ATTR_START);
      myPointSelectionMode = SM_SecondPoint;
    }
    break;
    case SM_SecondPoint: {
      setLinePoint(aPoint, theView, LINE_ATTR_END);
      commit();
      emit featureConstructed(feature(), FM_Deactivation);
      emit launchOperation(PartSet_OperationSketchLine::Type(), feature());
    }
    break;
    default:
      break;
  }
}

void PartSet_OperationSketchLine::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  switch (myPointSelectionMode)
  {
    case SM_SecondPoint:
    {
      gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(theEvent->pos(), theView);
      setLinePoint(aPoint, theView, LINE_ATTR_END);
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
      abort();
    }
    break;
    case Qt::Key_Return: {
      abort();
      emit launchOperation(PartSet_OperationSketchLine::Type(), boost::shared_ptr<ModelAPI_Feature>());
    }
    break;
    default:
    break;
  }
}

void PartSet_OperationSketchLine::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  myPointSelectionMode = !myInitPoint ? SM_FirstPoint : SM_SecondPoint;
  emit multiSelectionEnabled(false);
}

void PartSet_OperationSketchLine::abortOperation()
{
  emit featureConstructed(feature(), FM_Abort);
  PartSet_OperationSketchBase::abortOperation();
}

void PartSet_OperationSketchLine::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit multiSelectionEnabled(true);
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationSketchLine::createFeature()
{
  boost::shared_ptr<ModelAPI_Feature> aNewFeature = ModuleBase_Operation::createFeature();
  if (mySketch) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(mySketch);

    aFeature->addSub(aNewFeature);
  }
  if (myInitPoint) {
    boost::shared_ptr<ModelAPI_Data> aData = aNewFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                (aData->attribute(LINE_ATTR_START));
    aPoint->setValue(myInitPoint->x(), myInitPoint->y());
  }

  emit featureConstructed(aNewFeature, FM_Activation);
  return aNewFeature;
}

void PartSet_OperationSketchLine::getLinePoint(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                               const std::string& theAttribute,
                                               double& theX, double& theY)
{
  if (!theFeature)
    return;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  theX = aPoint->x();
  theY = aPoint->y();
}

void PartSet_OperationSketchLine::setLinePoint(double theX, double theY,
                                               const std::string& theAttribute)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  aPoint->setValue(theX, theY);
}

void PartSet_OperationSketchLine::setLinePoint(const gp_Pnt& thePoint,
                                               Handle(V3d_View) theView,
                                               const std::string& theAttribute)
{
  double aX, anY;
  PartSet_Tools::ConvertTo2D(thePoint, mySketch, theView, aX, anY);
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  aPoint->setValue(aX, anY);
}
