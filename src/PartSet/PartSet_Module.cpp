// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSet_Module.h"
#include <PartSet_WidgetSketchLabel.h>
#include <PartSet_Validators.h>
#include <PartSet_Tools.h>
#include <PartSet_WidgetPoint2d.h>
#include <PartSet_WidgetPoint2dDistance.h>
#include <PartSet_WidgetShapeSelector.h>
#include <PartSet_WidgetConstraintShapeSelector.h>
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
#include <ModelAPI_ShapeValidator.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_ContextMenuMgr.h>
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
#include <Config_PropManager.h>

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
#include <QMessageBox>
#include <QMainWindow>

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

  createActions();
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
  aFactory->registerValidator("PartSet_DifferentShapes", new ModelAPI_ShapeValidator);
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

void PartSet_Module::registerProperties()
{
  Config_PropManager::registerProp("Sketch planes", "planes_size", "Size", Config_Prop::Double,
                                   PLANE_SIZE);
  Config_PropManager::registerProp("Sketch planes", "planes_thickness", "Thickness",
                                   Config_Prop::Integer, SKETCH_WIDTH);
}

void PartSet_Module::operationCommitted(ModuleBase_Operation* theOperation) 
{
  if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->commitNestedSketch(theOperation);
  }

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
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->startSketch(theOperation);
  }
  else if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->startNestedSketch(theOperation);
  }

  if (myDocumentShapeFilter.IsNull())
    myDocumentShapeFilter = new PartSet_GlobalFilter(myWorkshop);
  myWorkshop->viewer()->addSelectionFilter(myDocumentShapeFilter);
}

void PartSet_Module::operationStopped(ModuleBase_Operation* theOperation)
{
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->stopSketch(theOperation);
  }
  else if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->stopNestedSketch(theOperation);
  }
  myWorkshop->viewer()->removeSelectionFilter(myDocumentShapeFilter);
}

bool PartSet_Module::canUndo() const
{
  bool aCanUndo = false;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument() && aMgr->canUndo()) {
    aCanUndo = !aMgr->isOperation();
    if (!aCanUndo) // check the enable state additionally by sketch manager
      aCanUndo = aMgr->canUndo();
  }
  return aCanUndo;
}

bool PartSet_Module::canRedo() const
{
  bool aCanRedo = false;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument() && aMgr->canRedo()) {
    aCanRedo = !aMgr->isOperation();
    if (!aCanRedo) // check the enable state additionally by sketch manager
      aCanRedo = aMgr->canRedo();
  }
  return aCanRedo;
}

bool PartSet_Module::canDisplayObject(const ObjectPtr& theObject) const
{
  bool aCanDisplay = false;
  if (!mySketchMgr->canDisplayObject())
    return aCanDisplay;
  CompositeFeaturePtr aSketchFeature = mySketchMgr->activeSketch();
  if (aSketchFeature.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);

    // MPV: the second and third conditions to avoid crash on exit for application
    if (aFeature.get() != NULL && aFeature->data().get() && aFeature->data()->isValid()) {
      if (aFeature == aSketchFeature) {
        aCanDisplay = false;
      }
      else if (aSketchFeature.get() && aSketchFeature->data().get() &&
               aSketchFeature->data()->isValid()) {
        for (int i = 0; i < aSketchFeature->numberOfSubs() && !aCanDisplay; i++) {
          FeaturePtr aSubFeature = aSketchFeature->subFeature(i);
          std::list<ResultPtr> aResults = aSubFeature->results();
          std::list<ResultPtr>::const_iterator aIt;
          for (aIt = aResults.begin(); aIt != aResults.end() && !aCanDisplay; ++aIt) {
            if (theObject == (*aIt))
              aCanDisplay = true;
          }
          if (aSubFeature == theObject)
            aCanDisplay = true;
        }
      }
    }
  }
  else {
    aCanDisplay = ModuleBase_IModule::canDisplayObject(theObject);
  }
  return aCanDisplay;
}

void PartSet_Module::addViewerItems(QMenu* theMenu) const
{
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if (!PartSet_SketcherMgr::isSketchOperation(anOperation) &&
      !PartSet_SketcherMgr::isNestedSketchOperation(anOperation))
    return;

  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  QObjectPtrList aObjects = aSelection->selectedPresentations();
  if (aObjects.size() > 0) {
    bool hasFeature = false;
    foreach(ObjectPtr aObject, aObjects)
    {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (aFeature.get() != NULL) {
        hasFeature = true;
      }
    }
    if (hasFeature) {
      XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
      XGUI_Workshop* aWorkshop = aConnector->workshop();
      QAction* anAction = aWorkshop->contextMenuMgr()->action("DELETE_CMD");
      theMenu->addAction(anAction);
    }
  }
  bool isConstruction;
  if (mySketchMgr->canChangeConstruction(isConstruction)) {
    QAction* anAction = action("CONSTRUCTION_CMD");
    theMenu->addAction(anAction);
    anAction->setChecked(isConstruction);
  }
}

void PartSet_Module::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  ModuleBase_IPropertyPanel* aPanel = theOperation->propertyPanel();
  if (PartSet_SketcherMgr::isSketchOperation(theOperation) &&  (theOperation->isEditOperation())) {
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
      if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation) &&
          PartSet_SketcherMgr::isDistanceOperation(theOperation)) {
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


void PartSet_Module::onSelectionChanged()
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (!aOperation)
    return;

  bool isSketcherOp = false;
  // An edit operation is enable only if the current opeation is the sketch operation
  if (mySketchMgr->activeSketch()) {
    if (PartSet_Tools::sketchPlane(mySketchMgr->activeSketch()))
      isSketcherOp = PartSet_SketcherMgr::isSketchOperation(aOperation);
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
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if(anOperation && PartSet_SketcherMgr::isNestedSketchOperation(anOperation)) {
    // Set final definitions if they are necessary
    //propertyPanelDefined(aOperation);

    /// Commit sketcher operations automatically
    anOperation->commit();
  }
}

void PartSet_Module::onNoMoreWidgets()
{
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation)) {
    if (anOperation) {
      if (myRestartingMode != RM_Forbided)
        myRestartingMode = RM_LastFeatureUsed;
      anOperation->commit();
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

ModuleBase_ModelWidget* PartSet_Module::createWidgetByType(const std::string& theType, QWidget* theParent,
                                            Config_WidgetAPI* theWidgetApi, std::string theParentId)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  ModuleBase_ModelWidget* aWgt = NULL;
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aLabelWgt = new PartSet_WidgetSketchLabel(theParent, theWidgetApi, theParentId);
    aLabelWgt->setWorkshop(aWorkshop);
    connect(aLabelWgt, SIGNAL(planeSelected(const std::shared_ptr<GeomAPI_Pln>&)),
      mySketchMgr, SLOT(onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>&)));
    aWgt = aLabelWgt;
  } else if (theType == "sketch-2dpoint_selector") {
    PartSet_WidgetPoint2D* aPointWgt = new PartSet_WidgetPoint2D(theParent, theWidgetApi, theParentId);
    aPointWgt->setWorkshop(aWorkshop);
    aPointWgt->setSketch(mySketchMgr->activeSketch());
    connect(aPointWgt, SIGNAL(vertexSelected()), this, SLOT(onVertexSelected()));
    aWgt = aPointWgt;
  } if (theType == "point2ddistance") {
    PartSet_WidgetPoint2dDistance* aDistanceWgt = new PartSet_WidgetPoint2dDistance(theParent, theWidgetApi, theParentId);
    aDistanceWgt->setWorkshop(aWorkshop);
    aDistanceWgt->setSketch(mySketchMgr->activeSketch());
    aWgt = aDistanceWgt;
  } if (theType == "sketch_shape_selector") {
    PartSet_WidgetShapeSelector* aShapeSelectorWgt =
      new PartSet_WidgetShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  } if (theType == "sketch_constraint_shape_selector") {
    PartSet_WidgetConstraintShapeSelector* aConstraintShapeSelectorWgt =
      new PartSet_WidgetConstraintShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aConstraintShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aConstraintShapeSelectorWgt;
  }
  return aWgt;
}

void PartSet_Module::createActions()
{
  QAction* anAction;

  anAction = new QAction(tr("Construction"), this);
  anAction->setCheckable(true);
  addAction("CONSTRUCTION_CMD", anAction);
}

QAction* PartSet_Module::action(const QString& theId) const
{
  if (myActions.contains(theId))
    return myActions[theId];
  return 0;
}

void PartSet_Module::addAction(const QString& theId, QAction* theAction)
{
  if (myActions.contains(theId))
    qCritical("A command with Id = '%s' already defined!", qPrintable(theId));
  theAction->setData(theId);
  connect(theAction, SIGNAL(triggered(bool)), this, SLOT(onAction(bool)));
  myActions[theId] = theAction;
}

void PartSet_Module::onAction(bool isChecked)
{
  QAction* aAction = static_cast<QAction*>(sender());
  QString anId = aAction->data().toString();

  if (anId == "CONSTRUCTION_CMD") {
    mySketchMgr->setConstruction(isChecked);
  }
}

bool PartSet_Module::deleteObjects()
{
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (!isSketchOp && !isNestedOp)
    return false;

  // sketch feature should be skipped, only sub-features can be removed
  // when sketch operation is active
  CompositeFeaturePtr aSketch = mySketchMgr->activeSketch();

  // selected objects should be collected before the current operation abort because
  // the abort leads to selection lost on constraint objects. It can be corrected after #386 issue
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  ModuleBase_ISelection* aSel = aConnector->selection();
  QObjectPtrList aSelectedObj = aSel->selectedPresentations();

  // if there are no selected objects in the viewer, that means that the selection in another
  // place cased this method. It is necessary to return the false value to understande in above
  // method that delete is not processed
  if (aSelectedObj.count() == 0)
    return false;

  if (isNestedOp)
    anOperation->abort();

  // the active nested sketch operation should be aborted unconditionally
  if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation))
    anOperation->abort();

  std::set<FeaturePtr> aRefFeatures;
  foreach (ObjectPtr aObj, aSelectedObj)
  {
    //ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    //if (aPart) {
      // TODO: check for what there is this condition. It is placed here historicaly because
      // ther is this condition during remove features.
    //} else {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    if (aFeature.get() != NULL) {
      aObj->document()->refsToFeature(aFeature, aRefFeatures, false);
    }
    //}
  }

  QString aDescription = aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text();
  /**
  // according to #355 feature, it is not necessary to inform about dependencies during
  // sketch delete operation
  // 
  if (!aRefFeatures.empty()) {
    QStringList aRefNames;
    std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                         aLast = aRefFeatures.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = (*anIt);
      if (aFeature == aSketch)
        continue;
      aRefNames.append((*anIt)->name().c_str());
    }
    if (!aRefNames.empty()) {
      QString aNames = aRefNames.join(", ");
      aDescription += aNames.prepend(" ");

      QMainWindow* aDesktop = aWorkshop->desktop();
      QMessageBox::StandardButton aRes = QMessageBox::warning(
          aDesktop, tr("Delete features"),
          QString(tr("Selected features are used in the following features: %1.\
  These features will be deleted also. Would you like to continue?")).arg(aNames),
          QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
      if (aRes != QMessageBox::Yes)
        return;
    }
  }*/

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->startOperation(aDescription.toStdString());
  std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                       aLast = aRefFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aRefFeature = (*anIt);
    if (aRefFeature == aSketch)
      continue;
    aRefFeature->document()->removeFeature(aRefFeature);
  }

  foreach (ObjectPtr aObj, aSelectedObj)
  {
    DocumentPtr aDoc = aObj->document();
    //ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    //if (aPart) {
    //  if (aDoc == aMgr->activeDocument()) {
    //    aDoc->close();
    //  }
    //} else {
      //FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    if (aFeature.get() != NULL) {
      aDoc->removeFeature(aFeature);
    }
    //}
  }
  aWorkshop->displayer()->updateViewer();
  //myDisplayer->updateViewer();
  aMgr->finishOperation();

  return true;
}
