// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSet_Module.h"
#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Validators.h"
#include "PartSet_Tools.h"
#include "PartSet_WidgetPoint2d.h"
#include "PartSet_WidgetPoint2dDistance.h"
#include "PartSet_WidgetShapeSelector.h"
#include "PartSet_WidgetPoint2dAngle.h"
#include "PartSet_WidgetMultiSelector.h"
#include "PartSet_WidgetEditor.h"
#include "PartSet_WidgetFileSelector.h"
#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_MenuMgr.h"

#include <PartSetPlugin_Remove.h>
#include <PartSetPlugin_Part.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_Tools.h>
#include <GeomValidators_ShapeType.h>

#include <GeomValidators_Face.h>
#include <GeomValidators_ConstructionComposite.h>
#include <GeomValidators_ZeroOffset.h>


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
#include <XGUI_ObjectsBrowser.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>

#include <SketcherPrs_SymbolPrs.h>
#include <SketcherPrs_Tools.h>

#include <Events_Loop.h>
#include <Config_PropManager.h>
#include <Config_Keywords.h>

#include <StdSelect_TypeOfFace.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <AIS_Dimension.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QApplication>
#include <QMessageBox>
#include <QMainWindow>
#include <QLineEdit>

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
  myRestartingMode(RM_None), myVisualLayerId(0)
{
  mySketchMgr = new PartSet_SketcherMgr(this);
  myDataModel = new PartSet_DocumentDataModel(this);

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();

  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();
  connect(anOpMgr, SIGNAL(keyEnterReleased()), this, SLOT(onEnterReleased()));
  connect(anOpMgr, SIGNAL(operationActivatedByPreselection()),
          this, SLOT(onOperationActivatedByPreselection()));

  ModuleBase_IViewer* aViewer = theWshop->viewer();
  connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)),
          this, SLOT(onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));
  connect(aViewer, SIGNAL(viewTransformed(int)),
          SLOT(onViewTransformed(int)));

  myMenuMgr = new PartSet_MenuMgr(this);

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));
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
  aFactory->registerValidator("PartSet_DistanceSelection", new PartSet_DistanceSelection);
  aFactory->registerValidator("PartSet_LengthSelection", new PartSet_LengthSelection);
  aFactory->registerValidator("PartSet_PerpendicularSelection", new PartSet_PerpendicularSelection);
  aFactory->registerValidator("PartSet_ParallelSelection", new PartSet_ParallelSelection);
  aFactory->registerValidator("PartSet_RadiusSelection", new PartSet_RadiusSelection);
  aFactory->registerValidator("PartSet_RigidSelection", new PartSet_RigidSelection);
  aFactory->registerValidator("PartSet_CoincidentSelection", new PartSet_CoincidentSelection);
  aFactory->registerValidator("PartSet_HVDirSelection", new PartSet_HVDirSelection);
  aFactory->registerValidator("PartSet_TangentSelection", new PartSet_TangentSelection);
  aFactory->registerValidator("PartSet_FilletSelection", new PartSet_FilletSelection);

  aFactory->registerValidator("PartSet_DifferentObjects", new PartSet_DifferentObjectsValidator);
  aFactory->registerValidator("PartSet_DifferentShapes", new ModelAPI_ShapeValidator);

  aFactory->registerValidator("GeomValidators_ShapeType", new GeomValidators_ShapeType);
  aFactory->registerValidator("GeomValidators_Face", new GeomValidators_Face);

  aFactory->registerValidator("GeomValidators_ConstructionComposite",
                              new GeomValidators_ConstructionComposite);

  aFactory->registerValidator("GeomValidators_ZeroOffset",
                              new GeomValidators_ZeroOffset);

  aFactory->registerValidator("PartSet_SketchEntityValidator",
                              new PartSet_SketchEntityValidator);

  aFactory->registerValidator("PartSet_SameTypeAttr",
                              new PartSet_SameTypeAttrValidator);
}

void PartSet_Module::registerFilters()
{
  //Registering of selection filters
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  ModuleBase_FilterFactory* aFactory = aConnector->selectionFilters();
}

void PartSet_Module::registerProperties()
{
  Config_PropManager::registerProp("Sketch planes", "planes_size", "Size", Config_Prop::Double,
                                   PLANE_SIZE);
  Config_PropManager::registerProp("Sketch planes", "planes_thickness", "Thickness",
                                   Config_Prop::Integer, SKETCH_WIDTH);
}

void PartSet_Module::onOperationCommitted(ModuleBase_Operation* theOperation) 
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

void PartSet_Module::onOperationAborted(ModuleBase_Operation* theOperation)
{
  breakOperationSequence();
}

void PartSet_Module::sendOperation(ModuleBase_Operation* theOperation)
{
  // Install the document filter before any other filter
  if (myDocumentShapeFilter.IsNull())
    myDocumentShapeFilter = new PartSet_GlobalFilter(myWorkshop);
  myWorkshop->viewer()->addSelectionFilter(myDocumentShapeFilter);

  ModuleBase_IModule::sendOperation(theOperation);
}

void PartSet_Module::onOperationStarted(ModuleBase_Operation* theOperation)
{
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    Handle(V3d_Viewer) aViewer = myWorkshop->viewer()->AISContext()->CurrentViewer();
    aViewer->AddZLayer(myVisualLayerId);
    mySketchMgr->startSketch(theOperation);
  }
  else if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->startNestedSketch(theOperation);
  }
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->stopSketch(theOperation);
    Handle(V3d_Viewer) aViewer = myWorkshop->viewer()->AISContext()->CurrentViewer();
    aViewer->RemoveZLayer(myVisualLayerId);
    myVisualLayerId = 0;
  }
  else if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->stopNestedSketch(theOperation);
  }
  myWorkshop->viewer()->removeSelectionFilter(myDocumentShapeFilter);
}

ModuleBase_Operation* PartSet_Module::currentOperation() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  return anOpMgr->currentOperation();
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
  // the sketch manager put the restriction to the objects display
  return mySketchMgr->canDisplayObject(theObject);
}

bool PartSet_Module::canActivateSelection(const ObjectPtr& theObject) const
{
  bool aCanActivate = ModuleBase_IModule::canActivateSelection(theObject);

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (isSketchOp || isNestedOp) {
    // in active sketch operation it is possible to activate operation object in selection
    // in the edit operation, e.g. points of the line can be moved when the line is edited
    aCanActivate = aCanActivate || anOperation->isEditOperation();
  }
  return aCanActivate;
}

bool PartSet_Module::addViewerMenu(QMenu* theMenu, const QMap<QString, QAction*>& theStdActions) const
{
  return myMenuMgr->addViewerMenu(theMenu, theStdActions);
}

void PartSet_Module::activeSelectionModes(QIntList& theModes)
{
  theModes.clear();
  if (mySketchMgr->activeSketch().get())
    PartSet_SketcherMgr::sketchSelectionModes(theModes);
}

bool PartSet_Module::isMouseOverWindow()
{
  return mySketchMgr->isMouseOverWindow();
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
  if (anOperation) {
    if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation)) {
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
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
    bool aFoundWidget = false;
    bool aFoundObligatory = false;
    for (; anIt != aLast && !aFoundObligatory; anIt++) {
      if (!aFoundWidget)
        aFoundWidget = *anIt == anActiveWidget;
      else
        aFoundObligatory = (*anIt)->isObligatory();
    }
    if (!aFoundObligatory)
      myRestartingMode = RM_Forbided;
  }
}

ModuleBase_ModelWidget* PartSet_Module::createWidgetByType(const std::string& theType, QWidget* theParent,
                                            Config_WidgetAPI* theWidgetApi, std::string theParentId)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  ModuleBase_ModelWidget* aWgt = NULL;
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aLabelWgt = new PartSet_WidgetSketchLabel(theParent, 
      theWidgetApi, theParentId, mySketchMgr->isConstraintsShown());
    aLabelWgt->setWorkshop(aWorkshop);
    connect(aLabelWgt, SIGNAL(planeSelected(const std::shared_ptr<GeomAPI_Pln>&)),
      mySketchMgr, SLOT(onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>&)));
    connect(aLabelWgt, SIGNAL(showConstraintToggled(bool)),
      mySketchMgr, SLOT(onShowConstraintsToggle(bool)));
    aWgt = aLabelWgt;
  } else if (theType == "sketch-2dpoint_selector") {
    PartSet_WidgetPoint2D* aPointWgt = new PartSet_WidgetPoint2D(theParent, theWidgetApi, theParentId);
    aPointWgt->setWorkshop(aWorkshop);
    aPointWgt->setSketch(mySketchMgr->activeSketch());
    connect(aPointWgt, SIGNAL(vertexSelected()), this, SLOT(onVertexSelected()));
    aWgt = aPointWgt;
  } else if (theType == "point2ddistance") {
    PartSet_WidgetPoint2dDistance* aDistanceWgt = new PartSet_WidgetPoint2dDistance(theParent, theWidgetApi, theParentId);
    aDistanceWgt->setWorkshop(aWorkshop);
    aDistanceWgt->setSketch(mySketchMgr->activeSketch());
    aWgt = aDistanceWgt;
  } else if(theType == "point2dangle") {
    PartSet_WidgetPoint2dAngle* anAngleWgt = new PartSet_WidgetPoint2dAngle(theParent, theWidgetApi, theParentId);
    anAngleWgt->setWorkshop(aWorkshop);
    anAngleWgt->setSketch(mySketchMgr->activeSketch());
    aWgt = anAngleWgt;
  } else if (theType == "sketch_shape_selector") {
    PartSet_WidgetShapeSelector* aShapeSelectorWgt =
      new PartSet_WidgetShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  } else if (theType == "sketch_multi_selector") {
    PartSet_WidgetMultiSelector* aShapeSelectorWgt =
      new PartSet_WidgetMultiSelector(theParent, workshop(), theWidgetApi, theParentId);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  } else if (theType == WDG_DOUBLEVALUE_EDITOR) {
    aWgt = new PartSet_WidgetEditor(theParent, workshop(), theWidgetApi, theParentId);
  } else if (theType == "export_file_selector") {
    aWgt = new PartSet_WidgetFileSelector(theParent, workshop(), theWidgetApi, theParentId);
  } else if (theType == "sketch_launcher") {
    aWgt = new PartSet_WidgetSketchCreator(theParent, this, theWidgetApi, theParentId);
  }
  return aWgt;
}


bool PartSet_Module::deleteObjects()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  // 1. check whether the delete should be processed in the module
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (isSketchOp || isNestedOp) {
    // 2. find selected presentations
    // selected objects should be collected before the current operation abort because
    // the abort leads to selection lost on constraint objects. It can be corrected after #386 issue
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    ModuleBase_ISelection* aSel = workshop()->selection();
    QObjectPtrList aSelectedObj = aSel->selectedPresentations();
    // if there are no selected objects in the viewer, that means that the selection in another
    // place cased this method. It is necessary to return the false value to understande in above
    // method that delete is not processed
    if (aSelectedObj.count() == 0)
      return false;

    // avoid delete of the objects, which are not belong to the current sketch
    // in order to do not delete results of other sketches
    QObjectPtrList aSketchObjects;
    QObjectPtrList::const_iterator anIt = aSelectedObj.begin(), aLast = aSelectedObj.end();
    for ( ; anIt != aLast; anIt++) {
      ObjectPtr anObject = *anIt;
      if (mySketchMgr->isObjectOfSketch(anObject)) {
        // sketch feature should be used in this list because workshop deletes features only
        // results are skipped
        FeaturePtr aSketchFeature = ModelAPI_Feature::feature(anObject);
        aSketchObjects.append(aSketchFeature);
      }
    }
    // if the selection contains only local selected presentations from other sketches,
    // the Delete operation should not be done at all
    if (aSketchObjects.size() == 0)
      return true;

    // the active nested sketch operation should be aborted unconditionally
    if (isNestedOp)
      anOperation->abort();

    // 3. start operation
    QString aDescription = aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text();
    aMgr->startOperation(aDescription.toStdString());

    // 4. delete features
    // sketch feature should be skipped, only sub-features can be removed
    // when sketch operation is active
    std::set<FeaturePtr> anIgnoredFeatures;
    anIgnoredFeatures.insert(mySketchMgr->activeSketch());
    aWorkshop->deleteFeatures(aSketchObjects, anIgnoredFeatures);
  
    // 5. stop operation
    aWorkshop->displayer()->updateViewer();
    aMgr->finishOperation();
  } else {
    bool isPartRemoved = false;
    // Delete part with help of PartSet plugin
    // TODO: the deleted objects has to be processed by multiselection
    QObjectPtrList aObjects = myWorkshop->selection()->selectedObjects();
    if (aObjects.size() == 1) {
      ObjectPtr aObj = aObjects.first();
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
      if (aFeature.get() && (aFeature->getKind() == PartSetPlugin_Part::ID())) {
        // Remove feature should be created in the document of the part results
        ResultPtr aPartResult = aFeature->firstResult();
        if (aPartResult.get()) {
          std::shared_ptr<ModelAPI_ResultPart> aPart =
                       std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartResult);
          DocumentPtr aPartDoc = aPart->partDoc();
          if (aPartDoc.get()) {
            aMgr->startOperation(PartSetPlugin_Remove::ID());
            FeaturePtr aFeature = aPartDoc->addFeature(PartSetPlugin_Remove::ID());
            aFeature->execute();
            aMgr->finishOperation();
            isPartRemoved = true;
          }
        }
      }
    }
    return isPartRemoved;
  }
  return true;
}

void PartSet_Module::onFeatureTriggered()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  // 1. check whether the delete should be processed in the module
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isNestedOp = PartSet_SketcherMgr::isNestedCreateOperation(anOperation);
  if (isNestedOp) {
    // in case if in the viewer nothing is displayed, the create operation should not be
    // comitted even if all values of the feature are initialized
    if (!mySketchMgr->canDisplayCurrentCreatedFeature()) {
      QAction* aCmd = dynamic_cast<QAction*>(sender());
      //Do nothing on uncheck
      if (aCmd->isCheckable() && !aCmd->isChecked())
        return;

      // the action information should be saved before the operation is aborted
      // because this abort leads to update command status, which unchecks this action
      anOperation->abort();

      launchOperation(aCmd->data().toString());
    }
  }
  ModuleBase_IModule::onFeatureTriggered();
}

void PartSet_Module::onObjectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS) 
{
  Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_InteractiveContext) aCtx = anAIS->GetContext();
    Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast(anAIS);
    if (!aDim.IsNull()) {
      aCtx->SetZLayer(aDim, myVisualLayerId);
    } else {
      Handle(SketcherPrs_SymbolPrs) aCons = Handle(SketcherPrs_SymbolPrs)::DownCast(anAIS);
      if (!aCons.IsNull())
        aCtx->SetZLayer(aCons, myVisualLayerId);
    }
  }
}

void PartSet_Module::onViewTransformed(int theTrsfType)
{
  // Set length of arrows constant in pixel size
  // if the operation is panning or rotate or panglobal then do nothing
  if ((theTrsfType == 1) || (theTrsfType == 3) || (theTrsfType == 4))
    return;

  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  //Handle(V3d_View) aView = aViewer->activeView();

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();

  Handle(V3d_Viewer) aV3dViewer = aContext->CurrentViewer();
  Handle(V3d_View) aView;
  double aScale = 0;
  for (aV3dViewer->InitDefinedViews(); 
       aV3dViewer->MoreDefinedViews(); 
       aV3dViewer->NextDefinedViews()) {
    Handle(V3d_View) aV = aV3dViewer->DefinedView();
    double aS = aV->Scale();
    if (aS > aScale) {
      aScale = aS;
      aView = aV;
    }
  }
  if (aView.IsNull())
    return;
  double aLen = aView->Convert(20);

  double aPrevLen = SketcherPrs_Tools::getArrowSize();
  SketcherPrs_Tools::setArrowSize(aLen);
  const double aPrevScale = aViewer->Scale(aViewer->activeView());
  const double aCurScale = aViewer->activeView()->Camera()->Scale();
  aViewer->SetScale(aViewer->activeView(), aCurScale);
  SketcherPrs_Tools::setTextHeight (aCurScale / aPrevScale * SketcherPrs_Tools::getTextHeight());
  bool isModified = false;
  QList<AISObjectPtr> aPrsList = aDisplayer->displayedPresentations();
  foreach (AISObjectPtr aAIS, aPrsList) {
    Handle(AIS_InteractiveObject) aAisObj = aAIS->impl<Handle(AIS_InteractiveObject)>();

    Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast(aAisObj);
    if (!aDim.IsNull()) {
      aDim->DimensionAspect()->ArrowAspect()->SetLength(aLen);
      aContext->Redisplay(aDim, false);
      isModified = true;
    }
  }
  if (isModified)
    aDisplayer->updateViewer();
}


void PartSet_Module::customizeObjectBrowser(QWidget* theObjectBrowser)
{
  XGUI_ObjectsBrowser* aOB = dynamic_cast<XGUI_ObjectsBrowser*>(theObjectBrowser);
  if (aOB) {
    QLineEdit* aLabel = aOB->activeDocLabel();
    QPalette aPalet = aLabel->palette();
    aPalet.setColor(QPalette::Text, QColor(0, 72, 140));
    aLabel->setPalette(aPalet);
    aOB->treeView()->setExpandsOnDoubleClick(false);
    connect(aOB->treeView(), SIGNAL(doubleClicked(const QModelIndex&)), 
      SLOT(onTreeViewDoubleClick(const QModelIndex&)));
    connect(aOB, SIGNAL(headerMouseDblClicked(const QModelIndex&)), 
      SLOT(onTreeViewDoubleClick(const QModelIndex&)));
    connect(aOB->treeView(), SIGNAL(doubleClicked(const QModelIndex&)), 
      myDataModel, SLOT(onMouseDoubleClick(const QModelIndex&)));
  }
}


void PartSet_Module::addObjectBrowserMenu(QMenu* theMenu) const
{
  QObjectPtrList aObjects = myWorkshop->selection()->selectedObjects();
  int aSelected = aObjects.size();
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aSelected == 1) {
    bool hasResult = false;
    bool hasFeature = false;
    bool hasParameter = false;
    ModuleBase_Tools::checkObjects(aObjects, hasResult, hasFeature, hasParameter);

    ObjectPtr aObject = aObjects.first();
    if (aObject) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
      FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      bool isPart = aPart.get() || 
        (aPartFeature.get() && (aPartFeature->getKind() == PartSetPlugin_Part::ID()));
      if (isPart) {
        DocumentPtr aPartDoc;
        if (!aPart.get()) {
          aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult());
        }
        if (aPart.get()) // this may be null is Part feature is disabled
          aPartDoc = aPart->partDoc();
        if (aMgr->activeDocument() == aPartDoc)
          theMenu->addAction(myMenuMgr->action("DEACTIVATE_PART_CMD"));
        else
          theMenu->addAction(myMenuMgr->action("ACTIVATE_PART_CMD"));
      } else if (aObject->document() == aMgr->activeDocument()) {
        if (hasParameter || hasFeature)
          theMenu->addAction(myMenuMgr->action("EDIT_CMD"));
      }
    } else {  // If feature is 0 the it means that selected root object (document)
      if (aMgr->activeDocument() != aMgr->moduleDocument())
        theMenu->addAction(myMenuMgr->action("ACTIVATE_PARTSET_CMD"));
    }
  } else if (aSelected == 0) {
    // if there is no selection then it means that upper label is selected
    QModelIndexList aIndexes = myWorkshop->selection()->selectedIndexes();
    if (aIndexes.size() == 0) // it means that selection happens in top label outside of tree view
      if (aMgr->activeDocument() != aMgr->moduleDocument())
        theMenu->addAction(myMenuMgr->action("ACTIVATE_PARTSET_CMD"));
  }
}

void PartSet_Module::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENT_CHANGED)) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    XGUI_DataTree* aTreeView = aWorkshop->objectBrowser()->treeView();
    QLineEdit* aLabel = aWorkshop->objectBrowser()->activeDocLabel();
    QPalette aPalet = aLabel->palette();

    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aActiveDoc = aMgr->activeDocument();
    DocumentPtr aDoc = aMgr->moduleDocument();
    QModelIndex aOldIndex = myDataModel->activePartTree();
    if (aActiveDoc == aDoc) {
      if (aOldIndex.isValid())
        aTreeView->setExpanded(aOldIndex, false);
      myDataModel->deactivatePart();
      aPalet.setColor(QPalette::Text, QColor(0, 72, 140));
    } else {
      std::string aGrpName = ModelAPI_ResultPart::group();
      for (int i = 0; i < aDoc->size(aGrpName); i++) {
        ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aDoc->object(aGrpName, i));
        if (aPart->partDoc() == aActiveDoc) {
          QModelIndex aIndex = myDataModel->partIndex(aPart);
          if (myDataModel->activatePart(aIndex)) {
            if (aOldIndex.isValid())
              aTreeView->setExpanded(aOldIndex, false);
            aTreeView->setExpanded(myDataModel->activePartTree(), true);
            aPalet.setColor(QPalette::Text, Qt::black);
          }
          break;
        }
      }
    }
    aLabel->setPalette(aPalet);
    aWorkshop->updateCommandStatus();

    // Update displayed objects in order to update active color
    XGUI_Displayer* aDisplayer = aWorkshop->displayer();
    QObjectPtrList aObjects = aDisplayer->displayedObjects();
    foreach(ObjectPtr aObj, aObjects)
      aDisplayer->redisplay(aObj, false);
    aDisplayer->updateViewer();
  }
}

void PartSet_Module::onTreeViewDoubleClick(const QModelIndex& theIndex)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  if (!theIndex.isValid()) {
    aMgr->setActiveDocument(aMgr->moduleDocument());
    return;
  }
  if (theIndex.column() != 0) // Use only first column
    return;
  ObjectPtr aObj = myDataModel->object(theIndex);
  ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
  if (!aPart.get()) { // Probably this is Feature
    FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aPartFeature.get() && (aPartFeature->getKind() == PartSetPlugin_Part::ID())) {
      aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult());
    }
  }
  if (aPart.get()) { // if this is a part
    if (aPart->partDoc() == aMgr->activeDocument()) {
      aMgr->setActiveDocument(aMgr->moduleDocument());
    } else {
      aPart->activate();
    }
  }
}
