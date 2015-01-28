// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSet_Module.h"
#include <PartSet_WidgetSketchLabel.h>
#include <PartSet_Validators.h>
#include <PartSet_Tools.h>
#include <PartSet_WidgetPoint2d.h>
#include <PartSet_WidgetPoint2dDistance.h>
#include <PartSet_WidgetShapeSelector.h>
#include <PartSet_SketcherMgr.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_FilterLinearEdge.h>
#include <ModuleBase_FilterFace.h>
#include <ModuleBase_FilterMulti.h>
#include <ModuleBase_FilterCustom.h>
#include <ModuleBase_FilterNoConsructionSubShapes.h>

#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_Tools.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
//#include <SketchPlugin_Arc.h>
//#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
//#include <SketchPlugin_ConstraintRigid.h>

#include <Events_Loop.h>

#include <StdSelect_TypeOfFace.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QApplication>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Dir.h>

#ifdef _DEBUG
#include <QDebug>
#endif

/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT ModuleBase_IModule* createModule(ModuleBase_IWorkshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

PartSet_Module::PartSet_Module(ModuleBase_IWorkshop* theWshop)
  : ModuleBase_IModule(theWshop), 
  myRestartingMode(RM_None)
{
  //myWorkshop = dynamic_cast<XGUI_Workshop*>(theWshop);
  mySketchMgr = new PartSet_SketcherMgr(this);

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();

  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();
  connect(anOpMgr, SIGNAL(keyEnterReleased()), this, SLOT(onEnterReleased()));
  connect(anOpMgr, SIGNAL(operationActivatedByPreselection()),
          this, SLOT(onOperationActivatedByPreselection()));

  ModuleBase_IViewer* aViewer = theWshop->viewer();
  connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)),
          this, SLOT(onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));
}

PartSet_Module::~PartSet_Module()
{
  if (!myDocumentShapeFilter.IsNull())
    myDocumentShapeFilter.Nullify();
}

void PartSet_Module::registerValidators()
{
  //Registering of validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("PartSet_DistanceValidator", new PartSet_DistanceValidator);
  aFactory->registerValidator("PartSet_LengthValidator", new PartSet_LengthValidator);
  aFactory->registerValidator("PartSet_PerpendicularValidator", new PartSet_PerpendicularValidator);
  aFactory->registerValidator("PartSet_ParallelValidator", new PartSet_ParallelValidator);
  aFactory->registerValidator("PartSet_RadiusValidator", new PartSet_RadiusValidator);
  aFactory->registerValidator("PartSet_RigidValidator", new PartSet_RigidValidator);
  aFactory->registerValidator("PartSet_DifferentObjects", new PartSet_DifferentObjectsValidator);
  aFactory->registerValidator("PartSet_SketchValidator", new PartSet_SketchValidator);
}

void PartSet_Module::registerFilters()
{
  //Registering of selection filters
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  ModuleBase_FilterFactory* aFactory = aConnector->selectionFilters();

  aFactory->registerFilter("EdgeFilter", new ModuleBase_FilterLinearEdge);
  aFactory->registerFilter("FaceFilter", new ModuleBase_FilterFace);
  aFactory->registerFilter("MultiFilter", new ModuleBase_FilterMulti);
  Handle(SelectMgr_Filter) aSelectFilter = new ModuleBase_FilterNoConsructionSubShapes(workshop());
  aFactory->registerFilter("NoConstructionSubShapesFilter",
            new ModuleBase_FilterCustom(aSelectFilter));
}

void PartSet_Module::operationCommitted(ModuleBase_Operation* theOperation) 
{
  if (theOperation->isEditOperation())
    return;
  // the selection is cleared after commit the create operation
  // in order to do not use the same selected objects in the restarted operation
  // for common behaviour, the selection is cleared even if the operation is not restarted
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull())
    aContext->ClearSelected();

  /// Restart sketcher operations automatically
  FeaturePtr aFeature = theOperation->feature();
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
  if (aSPFeature && (myRestartingMode == RM_LastFeatureUsed ||
                     myRestartingMode == RM_EmptyFeatureUsed)) {
    myLastOperationId = theOperation->id();
    myLastFeature = myRestartingMode == RM_LastFeatureUsed ? theOperation->feature() : FeaturePtr();
    
    launchOperation(myLastOperationId);
  }
  breakOperationSequence();
}

void PartSet_Module::breakOperationSequence()
{
  myLastOperationId = "";
  myLastFeature = FeaturePtr();
  myRestartingMode = RM_None;
}

void PartSet_Module::operationAborted(ModuleBase_Operation* theOperation)
{
  breakOperationSequence();
}

void PartSet_Module::operationStarted(ModuleBase_Operation* theOperation)
{
  if (theOperation->id().toStdString() == SketchPlugin_Sketch::ID()) {
    mySketchMgr->startSketch(theOperation);
  }
  if (myDocumentShapeFilter.IsNull())
    myDocumentShapeFilter = new PartSet_GlobalFilter(myWorkshop);
  myWorkshop->viewer()->addSelectionFilter(myDocumentShapeFilter);
}

void PartSet_Module::operationStopped(ModuleBase_Operation* theOperation)
{
  if (theOperation->id().toStdString() == SketchPlugin_Sketch::ID()) {
    mySketchMgr->stopSketch(theOperation);
  }
  myWorkshop->viewer()->removeSelectionFilter(myDocumentShapeFilter);
}


void PartSet_Module::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  ModuleBase_IPropertyPanel* aPanel = theOperation->propertyPanel();
  if ((theOperation->id().toStdString() == SketchPlugin_Sketch::ID()) && 
    (theOperation->isEditOperation())) {
    // we have to manually activate the sketch label in edit mode
      aPanel->activateWidget(aPanel->modelWidgets().first());
      return;
  }

  // Restart last operation type 
  if ((theOperation->id() == myLastOperationId) && myLastFeature) {
    ModuleBase_ModelWidget* aWgt = aPanel->activeWidget();
    if (theOperation->id().toStdString() == SketchPlugin_Line::ID()) {
      // Initialise new line with first point equal to end of previous
      PartSet_WidgetPoint2D* aPnt2dWgt = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
      if (aPnt2dWgt) {
        std::shared_ptr<ModelAPI_Data> aData = myLastFeature->data();
        std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));
        if (aPoint) {
          aPnt2dWgt->setPoint(aPoint->x(), aPoint->y());
          PartSet_Tools::setConstraints(mySketchMgr->activeSketch(), theOperation->feature(), 
            aWgt->attributeID(), aPoint->x(), aPoint->y());
          aPanel->activateNextWidget(aPnt2dWgt);
        }
      }
    }
  } else {
    // Start editing constraint
    if (theOperation->isEditOperation()) {
      // TODO: #391 - to be removed
      std::string aId = theOperation->id().toStdString();
      if (PartSet_SketcherMgr::sketchOperationIdList().contains(QString(aId.c_str()))) {
        if ((aId == SketchPlugin_ConstraintRadius::ID()) ||
            (aId == SketchPlugin_ConstraintLength::ID()) || 
            (aId == SketchPlugin_ConstraintDistance::ID())) {
          // Find and activate widget for management of point for dimension line position
          QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
          foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
            PartSet_WidgetPoint2D* aPntWgt = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
            if (aPntWgt) {
              aPanel->activateWidget(aPntWgt);
              return;
            }
          }
        } 
      }
    }
  }
}


void PartSet_Module::onSelectionChanged()
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (!aOperation)
    return;

  bool isSketcherOp = false;
  // An edit operation is enable only if the current opeation is the sketch operation
  if (mySketchMgr->activeSketch()) {
    if (PartSet_Tools::sketchPlane(mySketchMgr->activeSketch()))
      isSketcherOp = (aOperation->id().toStdString() == SketchPlugin_Sketch::ID());
  }
  if (isSketcherOp) {
    // Editing of constraints can be done on selection
    ModuleBase_ISelection* aSelect = myWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
    if (aSelected.size() == 1) {
      ModuleBase_ViewerPrs aPrs = aSelected.first();
      ObjectPtr aObject = aPrs.object();
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (aFeature) {
        std::string aId = aFeature->getKind();
        if ((aId == SketchPlugin_ConstraintRadius::ID()) ||
            (aId == SketchPlugin_ConstraintLength::ID()) || 
            (aId == SketchPlugin_ConstraintDistance::ID())) {
          editFeature(aFeature);
        }
      }
    }
  } 
}

void PartSet_Module::onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  anOpMgr->onKeyReleased(theEvent);
}

void PartSet_Module::onEnterReleased()
{
  myRestartingMode = RM_EmptyFeatureUsed;
}

void PartSet_Module::onOperationActivatedByPreselection()
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (!aOperation)
    return;

  // Set final definitions if they are necessary
  //propertyPanelDefined(aOperation);

  /// Commit sketcher operations automatically
  FeaturePtr aFeature = aOperation->feature();
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
  if (aSPFeature) {
    aOperation->commit();
  }
}

void PartSet_Module::onNoMoreWidgets()
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation) {
    /// Restart sketcher operations automatically
    FeaturePtr aFeature = aOperation->feature();
    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
              std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (aSPFeature) {
      if (myRestartingMode != RM_Forbided)
        myRestartingMode = RM_LastFeatureUsed;
      aOperation->commit();
    }
  }
}

void PartSet_Module::onVertexSelected()
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation->id().toStdString() == SketchPlugin_Line::ID()) {
    /// If last line finished on vertex the lines creation sequence has to be break
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
    if (aWidgets.last() == aPanel->activeWidget()) {
      myRestartingMode = RM_Forbided;
    }
  }
}

QWidget* PartSet_Module::createWidgetByType(const std::string& theType, QWidget* theParent,
                                            Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                            QList<ModuleBase_ModelWidget*>& theModelWidgets)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aWgt = new PartSet_WidgetSketchLabel(theParent, theWidgetApi, theParentId);
    aWgt->setWorkshop(aWorkshop);
    connect(aWgt, SIGNAL(planeSelected(const std::shared_ptr<GeomAPI_Pln>&)), 
      mySketchMgr, SLOT(onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>&)));
    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } else if (theType == "sketch-2dpoint_selector") {
    PartSet_WidgetPoint2D* aWgt = new PartSet_WidgetPoint2D(theParent, theWidgetApi, theParentId);
    aWgt->setWorkshop(aWorkshop);
    aWgt->setSketch(mySketchMgr->activeSketch());

    connect(aWgt, SIGNAL(vertexSelected()), this, SLOT(onVertexSelected()));

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } if (theType == "point2ddistance") {
    PartSet_WidgetPoint2dDistance* aWgt = new PartSet_WidgetPoint2dDistance(theParent, theWidgetApi, theParentId);
    aWgt->setWorkshop(aWorkshop);
    aWgt->setSketch(mySketchMgr->activeSketch());

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } if (theType == "sketch_shape_selector") {
    PartSet_WidgetShapeSelector* aWgt = 
      new PartSet_WidgetShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aWgt->setSketcher(mySketchMgr->activeSketch());

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } if (theType == "sketch_constraint_shape_selector") {
    PartSet_WidgetConstraintShapeSelector* aWgt = 
      new PartSet_WidgetConstraintShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aWgt->setSketcher(mySketchMgr->activeSketch());

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } else
    return 0;
}

