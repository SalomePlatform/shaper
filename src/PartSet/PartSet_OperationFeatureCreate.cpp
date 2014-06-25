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

#include <GeomAPI_Pnt2d.h>

#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetPoint2D.h>
#include <ModuleBase_WidgetFeature.h>
#include <ModuleBase_WidgetPoint2dDistance.h>

#include <XGUI_ViewerPrs.h>
#include <XGUI_Constants.h>

#include <V3d_View.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationFeatureCreate::PartSet_OperationFeatureCreate(const QString& theId,
                                                               QObject* theParent,
                                                                 FeaturePtr theFeature)
: PartSet_OperationSketchBase(theId, theParent), mySketch(theFeature), myActiveWidget(0)
{
}

PartSet_OperationFeatureCreate::~PartSet_OperationFeatureCreate()
{
}

bool PartSet_OperationFeatureCreate::canProcessKind(const std::string& theId)
{
  return theId == SKETCH_LINE_KIND || theId == SKETCH_POINT_KIND ||
         theId == SKETCH_CIRCLE_KIND ||
         theId == SKETCH_ARC_KIND ||
         theId == SKETCH_CONSTRAINT_DISTANCE_KIND ||
         theId == SKETCH_CONSTRAINT_LENGTH_KIND ||
         theId == SKETCH_CONSTRAINT_RADIUS_KIND ||
         theId == SKETCH_CONSTRAINT_PARALLEL_KIND ||
         theId == SKETCH_CONSTRAINT_PERPENDICULAR_KIND ||
         theId == SKETCH_CONSTRAINT_COINCIDENCE_KIND;
}

bool PartSet_OperationFeatureCreate::canBeCommitted() const
{
  return !myActiveWidget;
}

bool PartSet_OperationFeatureCreate::isGranted(ModuleBase_IOperation* theOperation) const
{
  return theOperation->getDescription()->operationId().toStdString() == PartSet_OperationSketch::Type();
}

std::list<int> PartSet_OperationFeatureCreate::getSelectionModes(FeaturePtr theFeature) const
{
  std::list<int> aModes;
  if (theFeature != feature())
    aModes = PartSet_OperationSketchBase::getSelectionModes(theFeature);
  return aModes;
}

void PartSet_OperationFeatureCreate::init(FeaturePtr theFeature,
                                       const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                       const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
    return;
  myInitFeature = theFeature;
}

FeaturePtr PartSet_OperationFeatureCreate::sketch() const
{
  return mySketch;
}

void PartSet_OperationFeatureCreate::mouseReleased(QMouseEvent* theEvent, Handle(V3d_View) theView,
                                                const std::list<XGUI_ViewerPrs>& theSelected,
                                                const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  if (canBeCommitted())
  {
    // if the point creation is finished, the next mouse release should commit the modification
    // the next release can happens by double click in the viewer
    commit();
    restartOperation(feature()->getKind(), feature());
    return;
  }

  double aX = 0, anY = 0;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
  if (theSelected.empty()) {
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
  }
  else {
    XGUI_ViewerPrs aPrs = theSelected.front();
    const TopoDS_Shape& aShape = aPrs.shape();
    if (!aShape.IsNull()) // the point is selected
    {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
        if (!aVertex.IsNull()) {
          aPoint = BRep_Tool::Pnt(aVertex);
          PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);

          PartSet_Tools::setConstraints(sketch(), feature(), myActiveWidget->attributeID(),
                                        aX, anY);
        }
      }
      else if (aShape.ShapeType() == TopAbs_EDGE) // the line is selected
      {
        PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
        // move to selected line
        if (feature()->getKind() == SKETCH_LINE_KIND) {
          //FeaturePtr aFeature = aPrs.feature();
          //projectPointOnLine(aFeature, myPointSelectionMode, aPoint, theView, aX, anY);
        }
      }
    }
  }
  bool isApplyed = false;
  if (isPointWidget())
    isApplyed = setWidgetPoint(aX, anY);
  else {
    if (!theSelected.empty()) {
      XGUI_ViewerPrs aPrs = theSelected.front();
      FeaturePtr aFeature = aPrs.feature();
      if (aFeature)
        isApplyed = setWidgetFeature(aFeature);
    }
  }
  if (isApplyed) {
    flushUpdated();
    emit activateNextWidget(myActiveWidget);
  }
}

void PartSet_OperationFeatureCreate::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  if (canBeCommitted()) {
    commit();
    restartOperation(feature()->getKind(), feature());
  }
  else {
    double aX, anY;
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theView);
    PartSet_Tools::convertTo2D(aPoint, sketch(), theView, aX, anY);
    setWidgetPoint(aX, anY);
    flushUpdated();
  }
}

void PartSet_OperationFeatureCreate::keyReleased(std::string theName, QKeyEvent* theEvent)
{
  int aKeyType = theEvent->key();
  // the second point should be activated by any modification in the property panel
  if (!theName.empty())
  {
    //setPointSelectionMode(myFeaturePrs->getNextMode(theName), false);
  }
  keyReleased(theEvent->key());
}

void PartSet_OperationFeatureCreate::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  myActiveWidget = theWidget;

  if (myInitFeature && myActiveWidget) {
    ModuleBase_WidgetPoint2D* aWgt = dynamic_cast<ModuleBase_WidgetPoint2D*>(myActiveWidget);
    if (aWgt)
      aWgt->initFromPrevious(myInitFeature);
    myInitFeature = FeaturePtr();
    emit activateNextWidget(myActiveWidget);
  }
}

void PartSet_OperationFeatureCreate::keyReleased(const int theKey)
{
  switch (theKey) {
    case Qt::Key_Return: {
      if (canBeCommitted())
      {
        commit();
        // it start a new line creation at a free point
        restartOperation(feature()->getKind(), FeaturePtr());
      }
    }
    break;
    case Qt::Key_Escape: {
      if (canBeCommitted())
      {
        commit();
      }
      else
      {
        abort();
      }
    }
    default:
    break;
  }
}

void PartSet_OperationFeatureCreate::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  //setPointSelectionMode(!myInitFeature ? SM_FirstPoint : SM_SecondPoint);

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

FeaturePtr PartSet_OperationFeatureCreate::createFeature(const bool theFlushMessage)
{
  FeaturePtr aNewFeature = ModuleBase_Operation::createFeature(false);
  if (sketch()) {
    boost::shared_ptr<SketchPlugin_Feature> aFeature = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(sketch());

    aFeature->addSub(aNewFeature);
  }
  //myFeaturePrs->init(aNewFeature);
  //myFeaturePrs->setFeature(myInitFeature, SM_FirstPoint);

  emit featureConstructed(aNewFeature, FM_Activation);
  if (theFlushMessage)
    flushCreated();
  return aNewFeature;
}

bool PartSet_OperationFeatureCreate::isPointWidget() const
{
  return dynamic_cast<ModuleBase_WidgetPoint2D*>(myActiveWidget) ||
         dynamic_cast<ModuleBase_WidgetPoint2dDistance*>(myActiveWidget);
}

bool PartSet_OperationFeatureCreate::setWidgetPoint(double theX, double theY)
{
  boost::shared_ptr<GeomAPI_Pnt2d> aPoint(new GeomAPI_Pnt2d(theX, theY));
  ModuleBase_WidgetPoint2D* aWidget = dynamic_cast<ModuleBase_WidgetPoint2D*>(myActiveWidget);
  if (aWidget) {
    aWidget->setPoint(aPoint);
    return true;
  } else {
    ModuleBase_WidgetPoint2dDistance* aWgt = dynamic_cast<ModuleBase_WidgetPoint2dDistance*>(myActiveWidget);
    if (aWgt) {
      aWgt->setPoint(feature(), aPoint);
      return true;
    }
  }
  return false;
}

bool PartSet_OperationFeatureCreate::setWidgetFeature(const FeaturePtr& theFeature)
{
  ModuleBase_WidgetFeature* aWidget = dynamic_cast<ModuleBase_WidgetFeature*>(myActiveWidget);
  if (!aWidget)
    return false;

  return aWidget->setFeature(theFeature);
}
