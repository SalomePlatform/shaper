// File:        PartSet_OperationFeatureCreate.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationFeatureCreate.h>

#include <PartSet_Tools.h>
#include <PartSet_OperationSketch.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintRigid.h>

#include <GeomAPI_Pnt2d.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetPoint2D.h>
#include <ModuleBase_WidgetValueFeature.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IPropertyPanel.h>

#include <XGUI_Constants.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationFeatureCreate::PartSet_OperationFeatureCreate(const QString& theId,
                                                               QObject* theParent,
                                                               CompositeFeaturePtr theFeature)
    : PartSet_OperationFeatureBase(theId, theParent, theFeature)
{
}

PartSet_OperationFeatureCreate::~PartSet_OperationFeatureCreate()
{
}

bool PartSet_OperationFeatureCreate::canProcessKind(const std::string& theId)
{
  return theId == SketchPlugin_Line::ID() || theId == SketchPlugin_Point::ID()
      || theId == SketchPlugin_Circle::ID() || theId == SketchPlugin_Arc::ID()
      || theId == SketchPlugin_ConstraintDistance::ID()
      || theId == SketchPlugin_ConstraintLength::ID()
      || theId == SketchPlugin_ConstraintRadius::ID()
      || theId == SketchPlugin_ConstraintParallel::ID()
      || theId == SketchPlugin_ConstraintPerpendicular::ID()
      || theId == SketchPlugin_ConstraintCoincidence::ID()
      || theId == SketchPlugin_ConstraintRigid::ID();
}

bool PartSet_OperationFeatureCreate::canBeCommitted() const
{
  if (PartSet_OperationSketchBase::canBeCommitted()) {
    //if(myActiveWidget && !myActiveWidget->isComputedDefault()) {
    return isValid();
  }
  return false;
}

void PartSet_OperationFeatureCreate::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
    double aX, anY;
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
    setWidgetValue(feature(), aX, anY);
    flushUpdated();
}

void PartSet_OperationFeatureCreate::keyReleased(const int theKey)
{
  switch (theKey) {
    case Qt::Key_Return:
    case Qt::Key_Enter: {
        // it start a new line creation at a free point
      if(isValid())
        restartOperation(feature()->getKind());
    }
      break;
    default:
      break;
  }
}

void PartSet_OperationFeatureCreate::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                 const std::list<ModuleBase_ViewerPrs>& theSelected,
                                                 const std::list<ModuleBase_ViewerPrs>& /*theHighlighted*/)
{
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
  double aX = aPoint.X(), anY = aPoint.Y();
  bool isClosedContour = false;

  if (theSelected.empty()) {
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
  } else {
    ModuleBase_ViewerPrs aPrs = theSelected.front();
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) { // a point is selected
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          aPoint = BRep_Tool::Pnt(aVertex);
          PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
          ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
          PartSet_Tools::setConstraints(sketch(), feature(), aActiveWgt->attributeID(), aX, anY);
          isClosedContour = true;
        }
      } else if (aShape.ShapeType() == TopAbs_EDGE) { // a line is selected
        PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
      }
    }
  }
  ObjectPtr aFeature;
  if (!theSelected.empty()) {
    ModuleBase_ViewerPrs aPrs = theSelected.front();
    aFeature = aPrs.object();
  } else {
    aFeature = feature();  // for the widget distance only
  }

  bool isApplyed = setWidgetValue(aFeature, aX, anY);
  if (isApplyed) {
    flushUpdated();
    myPropertyPanel->activateNextWidget();
  }

  if (!myPropertyPanel->activeWidget()) {
    if(commit() && !isClosedContour) {
      // if the point creation is finished, the next mouse release should commit the modification
      // the next release can happens by double click in the viewer
      restartOperation(feature()->getKind(), feature());
    }
  }
}

void PartSet_OperationFeatureCreate::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  emit multiSelectionEnabled(false);
}

void PartSet_OperationFeatureCreate::abortOperation()
{
  emit featureConstructed(feature(), FM_Hide);
  PartSet_OperationSketchBase::abortOperation();
}

void PartSet_OperationFeatureCreate::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit multiSelectionEnabled(true);
}

void PartSet_OperationFeatureCreate::afterCommitOperation()
{
  PartSet_OperationSketchBase::afterCommitOperation();
  emit featureConstructed(feature(), FM_Deactivation);
}

FeaturePtr PartSet_OperationFeatureCreate::createFeature(const bool theFlushMessage,
  CompositeFeaturePtr theCompositeFeature)
{
  FeaturePtr aNewFeature = ModuleBase_Operation::createFeature(false, sketch());

  if (theFlushMessage)
    flushCreated();
  return aNewFeature;
}


void PartSet_OperationFeatureCreate::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  PartSet_OperationFeatureBase::onWidgetActivated(theWidget);
  if (myInitFeature && theWidget) {
    ModuleBase_WidgetPoint2D* aWgt = dynamic_cast<ModuleBase_WidgetPoint2D*>(theWidget);
    if (aWgt && aWgt->initFromPrevious(myInitFeature)) {
      myInitFeature = FeaturePtr();
      if (myPropertyPanel)
        myPropertyPanel->activateNextWidget();
    }
  }
}
