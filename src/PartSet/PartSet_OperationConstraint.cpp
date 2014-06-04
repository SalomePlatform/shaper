// File:        PartSet_OperationConstraint.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationConstraint.h>

#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>

#include <GeomDataAPI_Point2D.h>

#include <ModuleBase_OperationDescription.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeDouble.h>

#include <SketchPlugin_Constraint.h>

#include <Geom_Line.hxx>
#include <gp_Lin.hxx>

#include <XGUI_ViewerPrs.h>
#include <XGUI_Constants.h>

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

PartSet_OperationConstraint::PartSet_OperationConstraint(const QString& theId,
	                                          QObject* theParent,
                                              boost::shared_ptr<ModelAPI_Feature> theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature)
{
}

PartSet_OperationConstraint::~PartSet_OperationConstraint()
{
}

bool PartSet_OperationConstraint::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

void PartSet_OperationConstraint::init(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                       const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                       const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  //if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
  //  return;
  // use the last point of the previous feature as the first of the new one
  //boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  //myInitPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationConstraint::sketch() const
{
  return mySketch;
}

void PartSet_OperationConstraint::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                const std::list<XGUI_ViewerPrs>& theSelected,
                                                const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  /*if (myPointSelectionMode == SM_DonePoint)
  {
    // if the point creation is finished, the next mouse release should commit the modification
    // the next release can happens by double click in the viewer
    commit();
    restartOperation(PartSet_OperationConstraint::Type(), feature());
    return;
  }

  double aX, anY;

  bool isFoundPoint = false;
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
*/
  if (theSelected.empty()) {
    //PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
    //isFoundPoint = true;
  }
  else {
    XGUI_ViewerPrs aPrs = theSelected.front();
    boost::shared_ptr<ModelAPI_Feature> aFeature = aPrs.feature();

    setFeature(aFeature);
    setValue(120);
    flushUpdated();

    /*const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) // the point is selected
    {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          aPoint = BRep_Tool::Pnt(aVertex);
          PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
          isFoundPoint = true;

          setConstraints(aX, anY);
        }
      }
      else if (aShape.ShapeType() == TopAbs_EDGE) // the line is selected
      {
        boost::shared_ptr<ModelAPI_Feature> aFeature = aPrs.feature();
        if (aFeature) {
          double X0, X1, X2, X3;
          double Y0, Y1, Y2, Y3;
          getLinePoint(aFeature, LINE_ATTR_START, X2, Y2);
          getLinePoint(aFeature, LINE_ATTR_END, X3, Y3);
          PartSet_Tools::convertTo2D(aPoint, sketch(), theView, X1, Y1);

          switch (myPointSelectionMode) {
            case SM_FirstPoint:
              PartSet_Tools::projectPointOnLine(X2, Y2, X3, Y3, X1, Y1, aX, anY);
            break;
            case SM_SecondPoint: {
              getLinePoint(feature(), LINE_ATTR_START, X0, Y0);
              PartSet_Tools::intersectLines(X0, Y0, X1, Y1, X2, Y2, X3, Y3, aX, anY);
            }
            break;
            default:
            break;
          }
          isFoundPoint = true;
        }
      }
    }*/
  }

  /*switch (myPointSelectionMode)
  {
    case SM_FirstPoint: {
      setLinePoint(feature(), aX, anY, LINE_ATTR_START);
      setLinePoint(feature(), aX, anY, LINE_ATTR_END);
      flushUpdated();

      //setPointSelectionMode(SM_SecondPoint);
    }
    break;
    case SM_SecondPoint: {
      setLinePoint(feature(), aX, anY, LINE_ATTR_END);
      flushUpdated();

      //setPointSelectionMode(SM_DonePoint);
   }
    break;
    default:
      break;
  }*/
}

void PartSet_OperationConstraint::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
/*  switch (myPointSelectionMode)
  {
    case SM_FirstPoint: {
      double aX, anY;
      gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
      PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
      setLinePoint(feature(), aX, anY, LINE_ATTR_START);
      setLinePoint(feature(), aX, anY, LINE_ATTR_END);
      flushUpdated();
      emit focusActivated(LINE_ATTR_START);
    }
    break;
    case SM_SecondPoint:
    {
      gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
      setLinePoint(aPoint, theView, LINE_ATTR_END);
      flushUpdated();
      emit focusActivated(LINE_ATTR_END);
    }
    break;
    case SM_DonePoint:
    {
      commit();
      restartOperation(PartSet_OperationConstraint::Type(), feature());
    }
    default:
      break;
  }*/
}

void PartSet_OperationConstraint::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //setPointSelectionMode(!myInitPoint ? SM_FirstPoint : SM_SecondPoint);

  emit multiSelectionEnabled(false);
}

void PartSet_OperationConstraint::abortOperation()
{
  emit featureConstructed(feature(), FM_Hide);
  PartSet_OperationSketchBase::abortOperation();
}

void PartSet_OperationConstraint::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit multiSelectionEnabled(true);
}

void PartSet_OperationConstraint::afterCommitOperation()
{
  PartSet_OperationSketchBase::afterCommitOperation();  
  emit featureConstructed(feature(), FM_Deactivation);
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationConstraint::createFeature(const bool theFlushMessage)
{
  boost::shared_ptr<ModelAPI_Feature> aNewFeature = ModuleBase_Operation::createFeature(false);
  if (sketch()) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(sketch());

    aFeature->addSub(aNewFeature);
  }
  /*if (myInitPoint) {
    setLinePoint(aNewFeature, myInitPoint->x(), myInitPoint->y(), LINE_ATTR_START);
    setLinePoint(aNewFeature, myInitPoint->x(), myInitPoint->y(), LINE_ATTR_END);

    boost::shared_ptr<ModelAPI_Data> aData = aNewFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                (aData->attribute(LINE_ATTR_START));
    createConstraint(myInitPoint, aPoint);
  }*/

  emit featureConstructed(aNewFeature, FM_Activation);
  if (theFlushMessage)
    flushCreated();
  return aNewFeature;
}

void PartSet_OperationConstraint::setFeature(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
    return;

  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  anAttr->setFeature(theFeature);
}

void PartSet_OperationConstraint::setValue(const double theValue)
{
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();

  boost::shared_ptr<ModelAPI_AttributeDouble> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  anAttr->setValue(theValue);

}
