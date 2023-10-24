// Copyright (C) 2014-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PartSet_Module.h"
#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Validators.h"
#include "PartSet_Tools.h"
#include "PartSet_PreviewPlanes.h"
#include "PartSet_WidgetBSplinePoints.h"
#include "PartSet_WidgetPoint2d.h"
#include "PartSet_WidgetPoint2DFlyout.h"
#include "PartSet_WidgetShapeSelector.h"
#include "PartSet_WidgetMultiSelector.h"
#include "PartSet_WidgetFeaturePointSelector.h"
#include "PartSet_WidgetEditor.h"
#include "PartSet_WidgetFileSelector.h"
#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_SketcherReentrantMgr.h"
#include "PartSet_ResultSketchPrs.h"
#include "PartSet_MenuMgr.h"
#include "PartSet_CustomPrs.h"
#include "PartSet_IconFactory.h"
#include "PartSet_OverconstraintListener.h"
#include "PartSet_TreeNodes.h"
#include "PartSet_FieldStepPrs.h"
#include "PartSet_BSplineWidget.h"

#include "PartSet_Filters.h"
#include "PartSet_FilterInfinite.h"

#ifdef _DEBUG
#include <QDebug>
#endif

#include <PartSetPlugin_Remove.h>
#include <PartSetPlugin_Part.h>
#include <PartSetPlugin_Duplicate.h>

#include <SketchPlugin_ConstraintCoincidence.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelectionActivate.h>
#include <ModuleBase_WidgetChoice.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_Preferences.h>

#include <ModelAPI_ResultField.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeImage.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultParameter.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <XGUI_ActiveControlMgr.h>
#include <XGUI_ActiveControlSelector.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_DataModel.h>
#include <XGUI_Displayer.h>
#include <XGUI_ErrorMgr.h>
#include <XGUI_FacesPanelSelector.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_ObjectsBrowser.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Projection.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_SymbolPrs.h>
#include <SketcherPrs_PositionMgr.h>
#include <SketcherPrs_Coincident.h>
#include <SketcherPrs_Tools.h>

#include <Events_Loop.h>
#include <Events_MessageBool.h>
#include <Config_PropManager.h>
#include <Config_Keywords.h>

#include <AIS_InteractiveObject.hxx>
#include <PrsDim_Dimension.hxx>
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
#include <QLineEdit>
#include <QString>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Dir.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>

#include <SUIT_ResourceMgr.h>

#include <Basics_OCCTVersion.hxx>

#define FEATURE_ITEM_COLOR "0,0,225"


/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT ModuleBase_IModule* createModule(ModuleBase_IWorkshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

//******************************************************
PartSet_Module::PartSet_Module(ModuleBase_IWorkshop* theWshop)
: ModuleBase_IModule(theWshop),
  myIsOperationIsLaunched(false),
  myVisualLayerId(0),
  myRoot(0)
{
  new PartSet_IconFactory(this);

  mySketchMgr = new PartSet_SketcherMgr(this);
  mySketchReentrantMgr = new PartSet_SketcherReentrantMgr(theWshop);

  ModuleBase_IViewer* aViewer = theWshop->viewer();
  connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)),
          this, SLOT(onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));
  connect(aViewer, SIGNAL(viewTransformed(int)),
          SLOT(onViewTransformed(int)));
  connect(aViewer, SIGNAL(viewCreated(ModuleBase_IViewWindow*)),
          SLOT(onViewCreated(ModuleBase_IViewWindow*)));
  myMenuMgr = new PartSet_MenuMgr(this);
  myCustomPrs = new PartSet_CustomPrs(theWshop);

  myOverconstraintListener = new PartSet_OverconstraintListener(theWshop);

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENTS_CLOSED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_LICENSE_VALID));

  registerSelectionFilter(SF_GlobalFilter, new PartSet_GlobalFilter(myWorkshop));
  registerSelectionFilter(SF_FilterInfinite, new PartSet_FilterInfinite(myWorkshop));
  Handle(PartSet_ResultGroupNameFilter) aCRFilter = new PartSet_ResultGroupNameFilter(myWorkshop);
  std::set<std::string> aCRGroupNames;
  aCRGroupNames.insert(ModelAPI_ResultConstruction::group());
  aCRFilter->setGroupNames(aCRGroupNames);
  registerSelectionFilter(SF_ResultGroupNameFilter, aCRFilter);

  setDefaultConstraintShown();

  Config_PropManager::registerProp("Visualization", "result_body_color", "Result color",
    Config_Prop::Color, ModelAPI_ResultBody::DEFAULT_COLOR());

  Config_PropManager::registerProp("Visualization", "result_group_color", "Group color",
    Config_Prop::Color, ModelAPI_ResultGroup::DEFAULT_COLOR());

  Config_PropManager::registerProp("Visualization", "result_construction_color",
    "Construction color",
    Config_Prop::Color,
    ModelAPI_ResultConstruction::DEFAULT_COLOR());

  Config_PropManager::registerProp("Visualization", "result_part_color", "Part color",
    Config_Prop::Color, ModelAPI_ResultPart::DEFAULT_COLOR());

  Config_PropManager::registerProp("Visualization", "result_field_color", "Field color",
    Config_Prop::Color, ModelAPI_ResultField::DEFAULT_COLOR());

  Config_PropManager::registerProp("Visualization", "operation_parameter_color",
                          "Reference shape wireframe color in operation", Config_Prop::Color,
                          PartSet_CustomPrs::OPERATION_PARAMETER_COLOR());
  Config_PropManager::registerProp("Visualization", "operation_result_color",
                          "Result shape wireframe color in operation", Config_Prop::Color,
                          PartSet_CustomPrs::OPERATION_RESULT_COLOR());
  Config_PropManager::registerProp("Visualization", "operation_highlight_color",
                          "Multi selector item color in operation", Config_Prop::Color,
                          PartSet_CustomPrs::OPERATION_HIGHLIGHT_COLOR());
  Config_PropManager::registerProp("Visualization", "operation_remove_feature_color",
                          "Color of removed feature in operation", Config_Prop::Color,
                          PartSet_CustomPrs::OPERATION_REMOVE_FEATURE_COLOR());
  Config_PropManager::registerProp("Visualization", "sketch_preview_plane",
                          "Color of sketch plane", Config_Prop::Color,
                          PartSet_CustomPrs::OPERATION_SKETCH_PLANE());

  Config_PropManager::registerProp("Visualization", "hidden_face_transparency",
                                   "Hidden faces transparency",
                                   Config_Prop::DblSpin,
                                   "0.8");

  std::ostringstream aStream;
  aStream << SketcherPrs_Tools::getDefaultArrowSize();
  Config_PropManager::registerProp("Visualization", "dimension_arrow_size",
    "Dimension arrow size", Config_Prop::IntSpin, aStream.str());

  Config_PropManager::registerProp("Visualization", "dimension_font", "Dimension font",
    Config_Prop::String, "Times-bold");

  aStream.str("");
  aStream.clear();
  aStream << SketcherPrs_Tools::getDefaultTextHeight();
  Config_PropManager::registerProp("Visualization", "dimension_value_size",
    "Dimension value size", Config_Prop::IntSpin, aStream.str());

  Config_PropManager::registerProp("Visualization", "sketch_dimension_color",
    "Dimension color",
    Config_Prop::Color, SKETCH_DIMENSION_COLOR);

  Config_PropManager::registerProp("Visualization", "feature_objectbrowser_color",
    "Feature items in Object Browser",
    Config_Prop::Color, FEATURE_ITEM_COLOR);

  Config_PropManager::registerProp("Visualization", "zoom_trihedron_arrows",
    "Keep trihedron arrows view size constant", Config_Prop::Boolean, "false");

  Config_PropManager::registerProp("Visualization", "use_auto_rotation",
    "Enable automatic rotation", Config_Prop::Boolean, "false");

  Config_PropManager::registerProp("Visualization", "axis_arrow_size",
    "Trihedron arrows constant size", Config_Prop::IntSpin, "10");

  Config_PropManager::registerProp("Shortcuts", "add_parameter_shortcut",
    "Add parameter in parameters manager dialog",
    Config_Prop::Shortcut, "Ctrl+A");

  Config_PropManager::registerProp("Windows", "use_hide_faces_panel",
    "Use HideFaces panel in operations", Config_Prop::Boolean, "false");
}

//******************************************************
PartSet_Module::~PartSet_Module()
{
  delete myCustomPrs;
  delete myOverconstraintListener;
  delete myRoot;
}

//******************************************************
void PartSet_Module::createFeatures()
{
  ModuleBase_IModule::createFeatures();

  // send signal to initialization plugin about the state of the preferences: to create part or not
  bool aCreate = ModuleBase_Preferences::resourceMgr()->booleanValue(
    ModuleBase_Preferences::GENERAL_SECTION, "create_init_part", true);
  Events_MessageBool aCreateMsg(Events_Loop::eventByName(EVENT_CREATE_PART_ON_START), aCreate);
  aCreateMsg.send();

  myRoot = new PartSet_RootNode();
  myRoot->setWorkshop(workshop());
  ModuleBase_IModule::loadProprietaryPlugins();
}


//******************************************************
void PartSet_Module::storeSelection()
{
  // cash is used only to restore selection, so it should be filled in storeSelection and
  // after applying immediatelly cleared in restoreSelection
  myCurrentSelection.clear();
  sketchMgr()->storeSelection(PartSet_SketcherMgr::ST_SelectType, myCurrentSelection);
}

//******************************************************
void PartSet_Module::restoreSelection()
{
  // cash is used only to restore selection, so it should be filled in storeSelection and
  // after applying immediatelly cleared in restoreSelection
  sketchMgr()->restoreSelection(myCurrentSelection);
  myCurrentSelection.clear();
}

//******************************************************
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
  aFactory->registerValidator("PartSet_AngleSelection", new PartSet_AngleSelection);
  aFactory->registerValidator("PartSet_EqualSelection", new PartSet_EqualSelection);
  aFactory->registerValidator("PartSet_CollinearSelection", new PartSet_CollinearSelection);
  aFactory->registerValidator("PartSet_MiddlePointSelection", new PartSet_MiddlePointSelection);
  aFactory->registerValidator("PartSet_DifferentObjects", new PartSet_DifferentObjectsValidator);
  aFactory->registerValidator("PartSet_DifferentPoints", new PartSet_DifferentPointsValidator);
  aFactory->registerValidator("PartSet_CoincidentAttr", new PartSet_CoincidentAttr);
  aFactory->registerValidator("PartSet_MultyTranslationSelection",
    new PartSet_MultyTranslationSelection);
  aFactory->registerValidator("PartSet_SplitSelection", new PartSet_SplitSelection);
  aFactory->registerValidator("PartSet_ProjectionSelection", new PartSet_ProjectionSelection);
  aFactory->registerValidator("PartSet_IntersectionSelection", new PartSet_IntersectionSelection);
}

//******************************************************
void PartSet_Module::connectToPropertyPanel(ModuleBase_ModelWidget* theWidget,
                                            const bool isToConnect)
{
  mySketchMgr->connectToPropertyPanel(theWidget, isToConnect);
}

//******************************************************
void PartSet_Module::operationCommitted(ModuleBase_Operation* theOperation)
{
  if (sketchMgr()->isNestedSketchOperation(theOperation)) {
    mySketchMgr->commitNestedSketch(theOperation);
  }
  /// deactivate of overconstraint listener should be performed after Sketch commit (#2176)
  if (PartSet_SketcherMgr::isSketchOperation(theOperation))
    overconstraintListener()->setActive(false);

  /// Restart sketcher operations automatically
  if (!mySketchReentrantMgr->operationCommitted(theOperation)) {
    getWorkshop()->selector()->clearSelection();
  }
}

//******************************************************
void PartSet_Module::operationAborted(ModuleBase_Operation* theOperation)
{
  /// Restart sketcher operations automatically
  mySketchReentrantMgr->operationAborted(theOperation);
  /// deactivate of overconstraint listener should be performed after Sketch abort (#2176)
  if (PartSet_SketcherMgr::isSketchOperation(theOperation))
    overconstraintListener()->setActive(false);

  getWorkshop()->selector()->clearSelection();
}

//******************************************************
void PartSet_Module::operationStarted(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                                (theOperation);
  if (aFOperation) {
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(workshop());
    XGUI_PropertyPanel* aPropertyPanel = aWorkshop->propertyPanel();

    ModuleBase_ModelWidget* aFilledWidget = 0;
    bool aPostonedWidgetActivation = false;

    FeaturePtr aFeature = aFOperation->feature();
    /// Restart sketcher operations automatically
    /// it is important to call method of sketch reentrant manager before filling of PP
    /// because it fills some created feature attributes, these new values should be used
    /// to fill the property panel
    mySketchReentrantMgr->operationStarted(theOperation);

    aWorkshop->fillPropertyPanel(aFOperation);
    // filling the operation values by the current selection
    // if the operation can be committed after the controls filling, the method perform should
    // be stopped. Otherwise unnecessary presentations can be shown(e.g. operation prs in sketch)
    bool isOperationCommitted = false;
    if (!aFOperation->isEditOperation()) {
      std::string aGreedAttributeId = ModuleBase_Tools::findGreedAttribute(workshop(), aFeature);
      // if there is a greed attribute, automatic commit by preselection
      // for this feature is prohibited
      aFilledWidget = aFOperation->activateByPreselection(aGreedAttributeId);
      if (currentOperation() != aFOperation)
        isOperationCommitted = true;
      else {
        if (aGreedAttributeId.empty()) {
          // a signal should be emitted before the next widget activation
          // because, the activation of the next widget will give a focus to the widget.
          // As a result the value of the widget is initialized.
          // And commit may happens until the value is entered.
          if (aFilledWidget) {
            if (mySketchReentrantMgr->canBeCommittedByPreselection())
              isOperationCommitted = mySketchMgr->operationActivatedByPreselection();
            // activate the next obligatory widget
            if (!isOperationCommitted)
              aPropertyPanel->activateNextWidget(aFilledWidget);
          }
        }
        else { // there is a greed widget
          const QList<ModuleBase_ModelWidget*>& aWidgets = aPropertyPanel->modelWidgets();
          std::string aFirstAttributeId = aWidgets.front()->attributeID();
          // activate next widget after greeded if it is the first widget in the panel
          // else the first panel widget is already activated by operation start
          if (aFirstAttributeId == aGreedAttributeId)
            aPostonedWidgetActivation = true;
        }
      }
    } if (!isOperationCommitted) {
      aWorkshop->connectToPropertyPanel(true);
      updateSketcherOnStart(aFOperation);
      updatePresentationsOnStart(aFOperation);

      // the objects of the current operation should be deactivated
      QObjectPtrList anObjects;
      anObjects.append(aFeature);
      std::list<ResultPtr> aResults;
      ModelAPI_Tools::allResults(aFeature, aResults);
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
        anObjects.append(*aIt);
      }
      QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
      for (; anIt != aLast; anIt++)
        aWorkshop->deactivateActiveObject(*anIt, false);
      if (anObjects.size() > 0) {
        XGUI_Displayer* aDisplayer = aWorkshop->displayer();
        aDisplayer->updateViewer();
      }
      /// state of command actions should be updated after displayed objects modification because
      /// deactivation(for example) of objects may influence on selection in the viewer
      /// State of command actions may depend on selection in the viewer(e.g. Sketch)
      workshop()->updateCommandStatus();
    }
    if (aPostonedWidgetActivation) {
      // if the widget is an empty in the chain of activated widgets, the current operation
      // is restarted. It should be performed after functionality of the operation starting
      aPropertyPanel->activateNextWidget(aFilledWidget);
    }
  }
}

//******************************************************
void PartSet_Module::updateSketcherOnStart(ModuleBase_Operation* theOperation)
{
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->startSketch(theOperation);
  }
  // It is switched off because of
  // Task #3067: 5.2.2 Drawing in the sketcher: change the mouse cursor arrow
  else if (sketchMgr()->isNestedSketchOperation(theOperation)) {
    mySketchMgr->startNestedSketch(theOperation);
  }
}

//******************************************************
void PartSet_Module::updatePresentationsOnStart(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (aFOperation) {
    myCustomPrs->activate(aFOperation->feature(), ModuleBase_IModule::CustomizeArguments, false);
    myCustomPrs->activate(aFOperation->feature(), ModuleBase_IModule::CustomizeResults, false);
  }
}

//******************************************************
void PartSet_Module::operationResumed(ModuleBase_Operation* theOperation)
{
  ModuleBase_IModule::operationResumed(theOperation);

  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (aFOperation) {
    myCustomPrs->activate(aFOperation->feature(), ModuleBase_IModule::CustomizeArguments, false);
    myCustomPrs->activate(aFOperation->feature(), ModuleBase_IModule::CustomizeResults, false);
  }
}

//******************************************************
void PartSet_Module::operationStopped(ModuleBase_Operation* theOperation)
{
  bool isModifiedArgs = myCustomPrs->deactivate(ModuleBase_IModule::CustomizeArguments, false);
  bool isModifiedResults = myCustomPrs->deactivate(ModuleBase_IModule::CustomizeResults, false);
  bool isModified = isModifiedArgs || isModifiedResults;

  if (sketchMgr()->isNestedSketchOperation(theOperation)) {
    mySketchMgr->stopNestedSketch(theOperation);
  }
  else if (PartSet_SketcherMgr::isSketchOperation(theOperation))
    setDefaultConstraintShown();

  //VSV: Viewer is updated on feature update and redisplay
  if (isModified) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
    aDisplayer->updateViewer();
  }

  QMap<PartSet_Tools::ConstraintVisibleState, bool>::const_iterator
    anIt = myHasConstraintShown.begin(), aLast = myHasConstraintShown.end();
  for (; anIt != aLast; anIt++) {
    mySketchMgr->updateBySketchParameters(anIt.key(), anIt.value());
  }
}

//******************************************************
ModuleBase_Operation* PartSet_Module::currentOperation() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  return anOpMgr->currentOperation();
}

//******************************************************
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

//******************************************************
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

//******************************************************
bool PartSet_Module::canApplyAction(const ObjectPtr& theObject, const QString& theActionId) const
{
  bool aValid = true;
  if (theActionId == "MOVE_CMD" || theActionId == "MOVE_SPLIT_CMD") {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature) {
      ResultPtr aResult = ModuleBase_Tools::firstResult(aFeature);
      // part features can not be moved in the history.
      if (aResult.get() && aResult->groupName() == ModelAPI_ResultPart::group())
        aValid = false;
    }
  }
  return aValid;
}

//******************************************************
bool PartSet_Module::canEraseObject(const ObjectPtr& theObject) const
{
  // the sketch manager put the restriction to the objects erase
  return mySketchMgr->canEraseObject(theObject);
}

//******************************************************
bool PartSet_Module::canDisplayObject(const ObjectPtr& theObject) const
{
  // the sketch manager put the restriction to the objects display
  return mySketchMgr->canDisplayObject(theObject);
}

//******************************************************
bool PartSet_Module::canUsePreselection(const QString& thePreviousOperationKind,
                                        const QString& theStartedOperationKind)
{
  if (ModuleBase_IModule::canUsePreselection(thePreviousOperationKind, theStartedOperationKind))
    return true;

  return mySketchMgr->isNestedSketchFeature(theStartedOperationKind);
}

/*void PartSet_Module::processHiddenObject(const std::list<ObjectPtr>& theObjects)
{
  mySketchMgr->processHiddenObject(theObjects);
}*/

//******************************************************
bool PartSet_Module::canActivateSelection(const ObjectPtr& theObject) const
{
  bool aCanActivate = ModuleBase_IModule::canActivateSelection(theObject);

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = sketchMgr()->isNestedSketchOperation(anOperation);
  if (isSketchOp || isNestedOp) {
    // in active sketch operation it is possible to activate operation object in selection
    // in the edit operation, e.g. points of the line can be moved when the line is edited
    ModuleBase_OperationFeature* aFOperation =
      dynamic_cast<ModuleBase_OperationFeature*>(anOperation);
    aCanActivate = aCanActivate || (aFOperation && aFOperation->isEditOperation());
  }
  return aCanActivate;
}

//******************************************************
bool PartSet_Module::addViewerMenu(const QMap<QString, QAction*>& theStdActions,
                                   QWidget* theParent,
                                   QMap<int, QAction*>& theMenuActions) const
{
  return myMenuMgr->addViewerMenu(theStdActions, theParent, theMenuActions);
}

//******************************************************
void PartSet_Module::updateViewerMenu(const QMap<QString, QAction*>& theStdActions)
{
  myMenuMgr->updateViewerMenu(theStdActions);
}

//******************************************************
bool PartSet_Module::isActionEnableStateFixed(const int theActionId) const
{
  bool isEnabledFixed = false;
  if (theActionId == XGUI_ActionsMgr::AcceptAll &&
      mySketchReentrantMgr->isInternalEditActive())
    isEnabledFixed = true;
  return isEnabledFixed;
}

//******************************************************
QString PartSet_Module::getFeatureError(const FeaturePtr& theFeature)
{
  QString anError = ModuleBase_IModule::getFeatureError(theFeature);
  if (anError.isEmpty())
    anError = sketchMgr()->getFeatureError(theFeature);

  return anError;
}

//******************************************************
void PartSet_Module::grantedOperationIds(ModuleBase_Operation* theOperation,
                                         QStringList& theIds) const
{
  myMenuMgr->grantedOperationIds(theOperation, theIds);

  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    XGUI_Workshop* aWorkshop = getWorkshop();
    theIds.append(aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text());
  }
}

//******************************************************
void PartSet_Module::activeSelectionModes(QIntList& theModes)
{
  if (mySketchMgr->activeSketch().get())
    PartSet_SketcherMgr::sketchSelectionModes(mySketchMgr->activeSketch(), theModes);
  else
    theModes = XGUI_Tools::workshop(myWorkshop)->viewerSelectionModes();
}

//******************************************************
void PartSet_Module::moduleSelectionModes(int theModesType, QIntList& theModes)
{
  customSubShapesSelectionModes(theModes);
  //theModes.append(XGUI_Tools::workshop(myWorkshop)->viewerSelectionModes());
  //myWorkshop->module()->activeSelectionModes(theModes);
}

//******************************************************
void PartSet_Module::moduleSelectionFilters(const QIntList& theFilterTypes,
                                            SelectMgr_ListOfFilter& theSelectionFilters)
{
  bool isSketchActive = mySketchMgr->activeSketch().get();

  std::map<ModuleBase_SelectionFilterType, Handle(SelectMgr_Filter)>::const_iterator aFiltersIt =
    mySelectionFilters.begin();
  for (; aFiltersIt != mySelectionFilters.end(); aFiltersIt++) {
    int aFilterType = aFiltersIt->first;
    // do not add not participating filters in given parameters
    if (!theFilterTypes.contains(aFilterType))
      continue;

    // using sketch filters only if sketch operation is active
    if (!isSketchActive &&
        mySketchMgr->sketchSelectionFilter((ModuleBase_SelectionFilterType)aFilterType))
      continue;

    // using filtering of construction results only from faces panel
    if (aFilterType == SF_ResultGroupNameFilter)
      continue;

    theSelectionFilters.Append(aFiltersIt->second);
  }
}

//******************************************************
void PartSet_Module::customSubShapesSelectionModes(QIntList& theModes)
{
  if (theModes.contains(TopAbs_FACE))
    theModes.append(SketcherPrs_Tools::Sel_Sketch_Face);
  if (theModes.contains(TopAbs_WIRE))
    theModes.append(SketcherPrs_Tools::Sel_Sketch_Wire);

  if (mySketchMgr->activeSketch().get())
    PartSet_SketcherMgr::sketchSelectionModes(mySketchMgr->activeSketch(), theModes);
}

//******************************************************
void PartSet_Module::getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theSelected,
                                      ObjectPtr& theObject, AttributePtr& theAttribute)
{
  ObjectPtr anObject = theSelected->object();
  GeomShapePtr aShape = theSelected->shape();

  theAttribute = findAttribute(anObject, aShape);
  // TODO: try to create result if object is an external object
  theObject = anObject;
}

//******************************************************
bool PartSet_Module::isMouseOverWindow()
{
  return mySketchMgr->isMouseOverWindow();
}

//******************************************************
bool PartSet_Module::isSketchNeutralPointActivated() const
{
  bool isNeutralPoint = true;
  if (sketchReentranceMgr()->isInternalEditActive())
    isNeutralPoint = false;
  if (myIsOperationIsLaunched)
    isNeutralPoint = false;

  return isNeutralPoint;
}

//******************************************************
void PartSet_Module::closeDocument()
{
  myActivePartIndex = QModelIndex();
  SketcherPrs_PositionMgr::get()->clearAll();
}

//******************************************************
void PartSet_Module::clearViewer()
{
  myCustomPrs->clearPrs();

  XGUI_Workshop* aWorkshop = getWorkshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  aDisplayer->deactivateSelectionFilters(false);
}

//******************************************************
void PartSet_Module::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
  // we have to manually activate the sketch label in edit mode
  if (PartSet_SketcherMgr::isSketchOperation(aFOperation) &&  (aFOperation->isEditOperation()))
    aPanel->activateWidget(aPanel->modelWidgets().first());
}

//******************************************************
bool PartSet_Module::createWidgets(const FeaturePtr& theFeature, const QString& theXmlRepr,
                                   QList<ModuleBase_ModelWidget*>& theWidgets) const
{
  bool aProcessed = false;

  XGUI_Workshop* aWorkshop = getWorkshop();
  XGUI_PropertyPanel* aPropertyPanel = aWorkshop->propertyPanel();
  if (mySketchMgr->activeSketch().get() && aPropertyPanel) {
    ModuleBase_ISelection* aSelection = workshop()->selection();
    // click on a point in sketch leads here with the point is highlighted, not yet selected
    QList<ModuleBase_ViewerPrsPtr> aPreselection = aSelection->getHighlighted();
    if (aPreselection.size() == 1) {
      ModuleBase_ViewerPrsPtr aSelectedPrs = aPreselection[0];
      ObjectPtr anObject = aSelectedPrs->object();

      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      GeomShapePtr aShape = aSelectedPrs->shape();
      // click on the digit of dimension constrain comes here
      // with an empty shape, so we need the check
      if (aFeature == theFeature && aShape.get() && !aShape->isNull()) {
        // if feature has only one result and shape of result is equal to selected shape
        // this attribute is not processed. It is a case of Sketch Point.
        if (aFeature->results().size() == 1) {
          ResultPtr aResult = aFeature->results().front();
          if (aResult.get() && aResult->shape()->isEqual(aShape))
            return aProcessed;
        }
        const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
        std::pair<AttributePtr, int> anAttribute =
            PartSet_Tools::findAttributeBy2dPoint(anObject, aTDShape, mySketchMgr->activeSketch());
        if (anAttribute.first.get()) {
          ModuleBase_WidgetFactory aFactory(theXmlRepr.toStdString(), workshop());

          const std::string anAttributeId = anAttribute.first->id();
          aFactory.createWidget(aPropertyPanel->contentWidget(), anAttributeId);

          theWidgets = aFactory.getModelWidgets();
          // it is possible that the point does not present in XML definition,
          // in this case, we assume that it is not processed by this module
          // e.g. "Intersection point" feature
          aProcessed = !theWidgets.isEmpty();
        }
      }
    }
  }
  return aProcessed;
}

//******************************************************
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
    QList<ModuleBase_ViewerPrsPtr> aSelected = aSelect->getSelected();
    if (aSelected.size() == 1) {
      ModuleBase_ViewerPrsPtr aPrs = aSelected.first();
      ObjectPtr aObject = aPrs->object();
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (aFeature) {
        std::string aId = aFeature->getKind();
        if ((aId == SketchPlugin_ConstraintRadius::ID()) ||
            (aId == SketchPlugin_ConstraintLength::ID()) ||
            (aId == SketchPlugin_ConstraintDistance::ID()) ||
            (aId == SketchPlugin_ConstraintAngle::ID())) {
          editFeature(aFeature);
        }
      }
    }
  }
}

//******************************************************
void PartSet_Module::onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  anOpMgr->onKeyReleased(theWnd->viewPort(), theEvent);
}

//******************************************************
ModuleBase_ModelWidget* PartSet_Module::createWidgetByType(const std::string& theType,
                                                           QWidget* theParent,
                                                           Config_WidgetAPI* theWidgetApi)
{
  ModuleBase_IWorkshop* aWorkshop = workshop();
  ModuleBase_ModelWidget* aWgt = NULL;
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aLabelWgt = new PartSet_WidgetSketchLabel(theParent, aWorkshop,
                                                               theWidgetApi, myHasConstraintShown);
    connect(aLabelWgt, SIGNAL(planeSelected(const std::shared_ptr<GeomAPI_Pln>&)),
      mySketchMgr, SLOT(onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>&)));
    connect(aLabelWgt, SIGNAL(showConstraintToggled(int, bool)),
      mySketchMgr, SLOT(onShowConstraintsToggle(int, bool)));
    connect(aLabelWgt, SIGNAL(showFreePoints(bool)), mySketchMgr, SLOT(onShowPoints(bool)));
    connect(aLabelWgt, SIGNAL(autoConstraints(bool)),
      sketchReentranceMgr(), SLOT(onAutoConstraints(bool)));
    aLabelWgt->setShowPointsState(mySketchMgr->isShowFreePointsShown());
    aWgt = aLabelWgt;
  } else if (theType == "sketch-2dpoint_selector") {
    PartSet_WidgetPoint2D* aPointWgt = new PartSet_WidgetPoint2D(theParent, aWorkshop,
                                                                 theWidgetApi);
    aPointWgt->setSketch(mySketchMgr->activeSketch());
    connect(aPointWgt, SIGNAL(vertexSelected()), sketchReentranceMgr(), SLOT(onVertexSelected()));
    aWgt = aPointWgt;
  }else if (theType == "sketch-2dpoint_flyout_selector") {
    PartSet_WidgetPoint2DFlyout* aPointWgt = new PartSet_WidgetPoint2DFlyout(theParent, aWorkshop,
                                                                             theWidgetApi);
    aPointWgt->setSketch(mySketchMgr->activeSketch());
    connect(aPointWgt, SIGNAL(vertexSelected()), sketchReentranceMgr(), SLOT(onVertexSelected()));
    aWgt = aPointWgt;
  } else if (theType == "sketch_shape_selector") {
    PartSet_WidgetShapeSelector* aShapeSelectorWgt =
                          new PartSet_WidgetShapeSelector(theParent, aWorkshop, theWidgetApi);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  } else if (theType == "sketch_multi_selector") {
    PartSet_WidgetMultiSelector* aShapeSelectorWgt =
                          new PartSet_WidgetMultiSelector(theParent, aWorkshop, theWidgetApi);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  }
  else if (theType == "sketch_feature_point_selector") {
    PartSet_WidgetFeaturePointSelector* aPointSelectorWgt =
            new PartSet_WidgetFeaturePointSelector(theParent, aWorkshop, theWidgetApi);
    aPointSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aPointSelectorWgt;
  }
  else if (theType == "sketch-bspline_selector") {
    PartSet_WidgetBSplinePoints* aBSplineWgt =
        new PartSet_WidgetBSplinePoints(theParent, aWorkshop, theWidgetApi);
    aBSplineWgt->setSketch(mySketchMgr->activeSketch());
    aWgt = aBSplineWgt;
  }
  else if (theType == WDG_DOUBLEVALUE_EDITOR) {
    aWgt = new PartSet_WidgetEditor(theParent, aWorkshop, theWidgetApi);
  } else if (theType == "export_file_selector") {
    aWgt = new PartSet_WidgetFileSelector(theParent, aWorkshop, theWidgetApi);
  } else if (theType == "sketch_launcher") {
    aWgt = new PartSet_WidgetSketchCreator(theParent, this, theWidgetApi);
  }
  else if (theType == "module_choice") {
    aWgt = new ModuleBase_WidgetChoice(theParent, theWidgetApi);
    connect(aWgt, SIGNAL(itemSelected(ModuleBase_ModelWidget*, int)),
      this, SLOT(onChoiceChanged(ModuleBase_ModelWidget*, int)));
  } else if (theType == "bspline-panel") {
    PartSet_BSplineWidget* aPanel = new PartSet_BSplineWidget(theParent, theWidgetApi);
    //aPanel->setFeature(theFeature);
    aWgt = aPanel;
  }
  return aWgt;
}

//******************************************************
ModuleBase_ModelWidget* PartSet_Module::activeWidget() const
{
  ModuleBase_ModelWidget* anActiveWidget = 0;

  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (!aOperation)
    return anActiveWidget;

  ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
  if (!aPanel)
    return anActiveWidget;

  return aPanel->activeWidget(true);
}

//******************************************************
bool PartSet_Module::deleteObjects()
{
  bool isProcessed = false;

  XGUI_Workshop* aWorkshop = getWorkshop();
  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();

  //SessionPtr aMgr = ModelAPI_Session::get();
  // 1. check whether the delete should be processed in the module
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = sketchMgr()->isNestedSketchOperation(anOperation);
  if (isSketchOp || isNestedOp) {
    isProcessed = true;
    // 2. find selected presentations
    // selected objects should be collected before the current operation abort because
    // the abort leads to selection lost on constraint objects. It can be corrected after #386 issue
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

    // 3. start operation
    QString aDescription = aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text();
    ModuleBase_Operation* anOpAction = new ModuleBase_Operation(aDescription, this);

    // the active nested sketch operation should be aborted unconditionally
    // the Delete action should be additionally granted for the Sketch operation
    // in order to do not abort/commit it
    bool isCommitted;
    if (!anOpMgr->canStartOperation(anOpAction->id(), isCommitted))
      return true; // the objects are processed but can not be deleted

    anOpMgr->startOperation(anOpAction);

    // WORKAROUND, should be done to avoid viewer highlight update after deletetion of objects
    // the problem is in AIS Dimensions recompute
    // if a line and the dim are removed, line is the first
    // it causes the AIS recompute, where the base line is null,
    // the result is empty AIS in the viewer
    XGUI_Tools::workshop(myWorkshop)->selector()->clearSelection();

    // 4. delete features
    // sketch feature should be skipped, only sub-features can be removed
    // when sketch operation is active
    aWorkshop->deleteFeatures(aSketchObjects);
    // 5. stop operation
    anOpMgr->commitOperation();
  }
  return isProcessed;
}

//******************************************************
void PartSet_Module::editFeature(FeaturePtr theFeature)
{
  storeConstraintsState(theFeature->getKind());
  ModuleBase_IModule::editFeature(theFeature);
}

//******************************************************
bool PartSet_Module::canCommitOperation() const
{
  return true;
}

//******************************************************
void PartSet_Module::launchOperation(const QString& theCmdId, const bool& isStartAfterCommitOnly)
{
  myIsOperationIsLaunched = true;
  storeConstraintsState(theCmdId.toStdString());
  updateConstraintsState(theCmdId.toStdString());

  ModuleBase_IModule::launchOperation(theCmdId, isStartAfterCommitOnly);

  myIsOperationIsLaunched = false;
}

//******************************************************
void PartSet_Module::storeConstraintsState(const std::string& theFeatureKind)
{
  if (myWorkshop->currentOperation() &&
      myWorkshop->currentOperation()->id().toStdString() == SketchPlugin_Sketch::ID()) {
    const QMap<PartSet_Tools::ConstraintVisibleState, bool>& aShownStates =
                                                  mySketchMgr->showConstraintStates();
    myHasConstraintShown = aShownStates;
  }
}

//******************************************************
void PartSet_Module::updateConstraintsState(const std::string& theFeatureKind)
{
  if (PartSet_SketcherMgr::constraintsIdList().contains(theFeatureKind.c_str()) ||
      PartSet_SketcherMgr::replicationsIdList().contains(theFeatureKind.c_str())) {
    // Show constraints if a constraint was anOperation
    mySketchMgr->updateBySketchParameters(PartSet_Tools::Geometrical, true);
    mySketchMgr->updateBySketchParameters(PartSet_Tools::Dimensional, true);
    mySketchMgr->updateBySketchParameters(PartSet_Tools::Expressions,
                                          myHasConstraintShown[PartSet_Tools::Expressions]);
  }
}

//******************************************************
void PartSet_Module::onObjectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS)
{
  Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get()) {
      bool aToUseZLayer = false;
      if (PartSet_Tools::findRefsToMeFeature(aFeature,SketchPlugin_Projection::ID()))
        aToUseZLayer = true;
      else {
        CompositeFeaturePtr aParent = ModelAPI_Tools::compositeOwner(aFeature);
        aToUseZLayer = (aParent.get() && (aParent->getKind() == SketchPlugin_Sketch::ID()));
      }
      if (aToUseZLayer) {
        Handle(AIS_InteractiveContext) aCtx = anAIS->GetContext();
        if (aFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID())
          aCtx->SetZLayer(anAIS, Graphic3d_ZLayerId_Top);
        else
          aCtx->SetZLayer(anAIS, myVisualLayerId);
      }
    }
  }
}

//******************************************************
void PartSet_Module::onBeforeObjectErase(ObjectPtr theObject, AISObjectPtr theAIS)
{
  // this is obsolete
  // it should be recomputed in order to disappear in the viewer if the corresponded object
  // is erased
  //if (myCustomPrs->isActive())
  //  myCustomPrs->redisplay(theObject, false);
}

//******************************************************
void PartSet_Module::onViewTransformed(int theTrsfType)
{
  // Set length of arrows constant in pixel size
  // if the operation is panning or rotate or panglobal then do nothing
  if ((theTrsfType == 1) || (theTrsfType == 3) || (theTrsfType == 4))
    return;
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
  if (aContext.IsNull())
    return;

  //Handle(V3d_View) aView = aViewer->activeView();

  XGUI_Workshop* aWorkshop = getWorkshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  Handle(V3d_Viewer) aV3dViewer = aContext->CurrentViewer();
  Handle(V3d_View) aView;
  double aScale = 0;
  V3d_ListOfView::Iterator aDefinedViews(aV3dViewer->DefinedViews());
  for (; aDefinedViews.More(); aDefinedViews.Next()) {
    Handle(V3d_View) aV = aDefinedViews.Value();
    double aS = aV->Scale();
    if (aS > aScale) {
      aScale = aS;
      aView = aV;
    }
  }
  if (aView.IsNull())
    return;

  bool isModified = false;
  double aLen = aView->Convert(SketcherPrs_Tools::getConfigArrowSize());
  ModuleBase_Operation* aCurrentOperation = myWorkshop->currentOperation();
  if (aCurrentOperation &&
    (PartSet_SketcherMgr::isSketchOperation(aCurrentOperation) ||
     sketchMgr()->isNestedSketchOperation(aCurrentOperation) ||
     (aCurrentOperation->id() == "Measurement")))
  {
    SketcherPrs_Tools::setArrowSize(aLen);
    const double aCurScale = aViewer->activeView()->Camera()->Scale();
    aViewer->SetScale(aViewer->activeView(), aCurScale);
#ifdef OPTIMIZE_PRS
    QList<Handle(AIS_InteractiveObject)> aPrsList = aDisplayer->displayedPresentations();
    foreach(Handle(AIS_InteractiveObject) aAisObj, aPrsList) {
#else
    QList<AISObjectPtr> aPrsList = aDisplayer->displayedPresentations();
    foreach(AISObjectPtr aAIS, aPrsList) {
      Handle(AIS_InteractiveObject) aAisObj = aAIS->impl<Handle(AIS_InteractiveObject)>();
#endif
      Handle(PrsDim_Dimension) aDim = Handle(PrsDim_Dimension)::DownCast(aAisObj);
      if (!aDim.IsNull()) {
        aDim->DimensionAspect()->ArrowAspect()->SetLength(aLen);
        aContext->Redisplay(aDim, false);
        isModified = true;
      }
    }
  }

  // Manage trihedron arrows
  if (Config_PropManager::boolean("Visualization", "zoom_trihedron_arrows")) {
    Handle(AIS_Trihedron) aTrihedron = aViewer->trihedron();
    if (!aTrihedron.IsNull()) {
      double aAxLen =
        aView->Convert(Config_PropManager::integer("Visualization", "axis_arrow_size"));
      Handle(Prs3d_DatumAspect) aDatumAspect = aTrihedron->Attributes()->DatumAspect();
      double aAxisLen = aDatumAspect->AxisLength(Prs3d_DP_XAxis);
      aDatumAspect->SetAttribute(Prs3d_DP_ShadingConeLengthPercent, aAxLen / aAxisLen);
      aTrihedron->Attributes()->SetDatumAspect(aDatumAspect);
      aContext->Redisplay(aTrihedron, false);
      isModified = true;
    }
  }
  if (isModified)
    aDisplayer->updateViewer();
}

//******************************************************
bool PartSet_Module::isCustomPrsActivated(const ModuleBase_CustomizeFlag& theFlag) const
{
  return myCustomPrs->isActive(theFlag);
}

//******************************************************
void PartSet_Module::activateCustomPrs(const FeaturePtr& theFeature,
                                       const ModuleBase_CustomizeFlag& theFlag,
                                       const bool theUpdateViewer)
{
  myCustomPrs->activate(theFeature, theFlag, theUpdateViewer);
}

//******************************************************
void PartSet_Module::deactivateCustomPrs(const ModuleBase_CustomizeFlag& theFlag,
                                         const bool theUpdateViewer)
{
  myCustomPrs->deactivate(theFlag, theUpdateViewer);
}

//******************************************************
//bool PartSet_Module::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
//                                           std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs)
//{
//  bool aCustomized = false;
//
//  XGUI_Workshop* aWorkshop = getWorkshop();
//  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
//  ObjectPtr anObject = aDisplayer->getObject(thePrs);
//  if (!anObject)
//    return aCustomized;
//
//  if (!theResult.get()) {
//    std::vector<int> aColor;
//    XGUI_CustomPrs::getDefaultColor(anObject, true, aColor);
//    if (!aColor.empty()) {
//      aCustomized = thePrs->setColor(aColor[0], aColor[1], aColor[2]);
//    }
//  }
//  // customize dimentional constrains
//  sketchMgr()->customisePresentation(anObject);
//
//  return aCustomized;
//}
//
////******************************************************
//bool PartSet_Module::afterCustomisePresentation(std::shared_ptr<ModelAPI_Result> theResult,
//                                                AISObjectPtr thePrs,
//                                                GeomCustomPrsPtr theCustomPrs)
//{
//  bool aCustomized = false;
//
//  XGUI_Workshop* aWorkshop = getWorkshop();
//  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
//  ObjectPtr anObject = aDisplayer->getObject(thePrs);
//  if (!anObject)
//    return aCustomized;
//
//  std::vector<int> aColor;
//  bool aUseCustomColor = true;
//  if (aUseCustomColor)
//    myOverconstraintListener->getCustomColor(anObject, aColor);
//  // customize sketch symbol presentation
//  Handle(AIS_InteractiveObject) anAISIO = thePrs->impl<Handle(AIS_InteractiveObject)>();
//  if (!anAISIO.IsNull()) {
//    if (!Handle(SketcherPrs_SymbolPrs)::DownCast(anAISIO).IsNull()) {
//      Handle(SketcherPrs_SymbolPrs) aPrs = Handle(SketcherPrs_SymbolPrs)::DownCast(anAISIO);
//      if (!aPrs.IsNull()) {
//        aPrs->SetCustomColor(aColor);
//        aCustomized = true;
//      }
//    } else if (!Handle(SketcherPrs_Coincident)::DownCast(anAISIO).IsNull()) {
//      Handle(SketcherPrs_Coincident) aPrs = Handle(SketcherPrs_Coincident)::DownCast(anAISIO);
//      if (!aPrs.IsNull()) {
//        aPrs->SetCustomColor(aColor);
//        aCustomized = true;
//      }
//    }
//  }
//  // customize sketch dimension constraint presentation
//  if (!aCustomized) {
//    if (!aColor.empty()) { // otherwise presentation has the default color
//      aCustomized = thePrs->setColor(aColor[0], aColor[1], aColor[2]);
//    }
//  }
//  return aCustomized;
//}

//******************************************************
bool PartSet_Module::customizeFeature(ObjectPtr theObject, const ModuleBase_CustomizeFlag& theFlag,
                                     const bool theUpdateViewer)
{
  bool isRedisplayed = false;
  if (myCustomPrs->isActive(theFlag))
    isRedisplayed = myCustomPrs->redisplay(theObject, theFlag, theUpdateViewer);

  return isRedisplayed;
}

//******************************************************
void PartSet_Module::customizeObjectBrowser(QWidget* theObjectBrowser)
{
  XGUI_ObjectsBrowser* aOB = dynamic_cast<XGUI_ObjectsBrowser*>(theObjectBrowser);
  if (aOB) {
    QLabel* aLabel = aOB->activeDocLabel();
    aLabel->installEventFilter(myMenuMgr);
    connect(aLabel, SIGNAL(customContextMenuRequested(const QPoint&)),
          SLOT(onActiveDocPopup(const QPoint&)));
    aOB->treeView()->setExpandsOnDoubleClick(false);
    connect(aOB->treeView(), SIGNAL(doubleClicked(const QModelIndex&)),
      SLOT(onTreeViewDoubleClick(const QModelIndex&)));

    Events_Loop* aLoop = Events_Loop::loop();
    aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  }
}

//******************************************************
void PartSet_Module::onActiveDocPopup(const QPoint& thePnt)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  QAction* aActivatePartAction = myMenuMgr->action("ACTIVATE_PARTSET_CMD");

  XGUI_Workshop* aWorkshop = getWorkshop();
  QLabel* aHeader = aWorkshop->objectBrowser()->activeDocLabel();

  aActivatePartAction->setEnabled((aMgr->activeDocument() != aMgr->moduleDocument()));

  QMenu aMenu;
  aMenu.addAction(aActivatePartAction);

#ifndef HAVE_SALOME
  if (aMgr->activeDocument() == aMgr->moduleDocument()) {
    DocumentPtr aDoc = aMgr->moduleDocument();
    int aNbParts = aDoc->size(ModelAPI_ResultPart::group());
    bool aHaveToActivate = false;
    for (int i = 0; i < aNbParts; i++) {
      ObjectPtr aObj = aDoc->object(ModelAPI_ResultPart::group(), i);
      ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
      if (!aPartRes->partDoc().get()) {
        aHaveToActivate = true;
        break;
      }
    }
    if (aHaveToActivate) {
      QAction* aActivateAllPartAction = myMenuMgr->action("ACTIVATE_ALL_PARTS_CMD");
      aMenu.addAction(aActivateAllPartAction);
    }
  }
#endif

  aMenu.exec(aHeader->mapToGlobal(thePnt));
}

//******************************************************
AISObjectPtr PartSet_Module::createPresentation(const ObjectPtr& theObject)
{
  Handle(AIS_InteractiveObject) anAISPrs = mySketchMgr->createPresentation(theObject);
  if (anAISPrs.IsNull()) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get()) {
      std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
      if (aShapePtr.get() != NULL)
        anAISPrs = new ModuleBase_ResultPrs(aResult);
    }
    else {
      FieldStepPtr aStep =
        std::dynamic_pointer_cast<ModelAPI_ResultField::ModelAPI_FieldStep>(theObject);
      if (aStep.get()) {
        anAISPrs = new PartSet_FieldStepPrs(aStep);
      }
    }
  }
  AISObjectPtr anAIS;
  if (!anAISPrs.IsNull()) {
    Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast(anAISPrs);
    if (!aShapePrs.IsNull())
      ModuleBase_Tools::setPointBallHighlighting((AIS_Shape*)aShapePrs.get());

    anAIS = AISObjectPtr(new GeomAPI_AISObject());
    anAIS->setImpl(new Handle(AIS_InteractiveObject)(anAISPrs));
    customizePresentation(theObject, anAIS);
  }
  return anAIS;
}

//******************************************************
double getResultDeflection(const ResultPtr& theResult)
{
  double aDeflection = ModelAPI_Tools::getDeflection(theResult);
  if (aDeflection < 0)
    aDeflection = PartSet_Tools::getDefaultDeflection(theResult);
  return aDeflection;
}

//******************************************************
double getResultTransparency(const ResultPtr& theResult)
{
  double aTransparency = ModelAPI_Tools::getTransparency(theResult);
  if (aTransparency < 0)
    aTransparency = PartSet_Tools::getDefaultTransparency();
  return aTransparency;
}

static AttributeImagePtr findImage(const ObjectPtr& theResult)
{
  AttributeImagePtr anImageAttr;

  if (theResult.get()) {
    ResultBodyPtr aResultBody =
      std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
    if (aResultBody.get()) {
      anImageAttr = aResultBody->data()->image(ModelAPI_ResultBody::IMAGE_ID());
      if (!anImageAttr.get() || !anImageAttr->hasTexture()) {
        // try to find an image attribute in parents
        ObjectPtr aParent = theResult->document()->parent(theResult);
        anImageAttr = findImage(aParent);
      }
    }
  }

  return anImageAttr;
}

//******************************************************
void PartSet_Module::setTexture(const AISObjectPtr& thePrs,
                                const ResultPtr& theResult)
{
  ResultBodyPtr aResultBody =
    std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (!aResultBody.get())
    return;

  AttributeImagePtr anImageAttr = findImage(theResult);
  if (!anImageAttr.get() || !anImageAttr->hasTexture())
    return;

  int aWidth, aHeight;
  std::string aFormat;
  std::list<unsigned char> aByteList;
  anImageAttr->texture(aWidth, aHeight, aByteList, aFormat);

  Handle(AIS_InteractiveObject) anAIS = thePrs->impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    /// set color to white and change material aspect,
    /// in order to keep a natural apect of the image.
    thePrs->setColor(255, 255, 255);
    Quantity_Color myShadingColor(NCollection_Vec3<float>(1.,  1., 1.));
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(anAIS);
    if (!anAISShape.IsNull())
    {
      auto myDrawer = anAISShape->Attributes();

      myDrawer->ShadingAspect()->SetColor(myShadingColor);
      myDrawer->ShadingAspect()->Aspect()->SetDistinguishOn();
      Graphic3d_MaterialAspect aMatAspect(Graphic3d_NOM_PLASTIC);
      aMatAspect.SetTransparency(0.0);
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(aMatAspect);
      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(aMatAspect);

      //aPixmap = OCCViewer_Utilities::imageToPixmap( px.toImage());
      Handle(Image_PixMap) aPixmap = new Image_PixMap();
#if OCC_VERSION_LARGE < 0x07070000
      aPixmap->InitTrash(Image_PixMap::ImgBGRA, aWidth, aHeight);
#else
      aPixmap->InitTrash(Image_Format_BGRA, aWidth, aHeight);
#endif
      std::list<unsigned char>::iterator aByteIter = aByteList.begin();
      for (int aLine = 0; aLine < aHeight; ++aLine) {
        // convert pixels from ARGB to renderer-compatible RGBA
        for (int aByte = 0; aByte < aWidth; ++aByte) {
          Image_ColorBGRA& aPixmapBytes = aPixmap->ChangeValue<Image_ColorBGRA>(aLine, aByte);

          aPixmapBytes.b() = (Standard_Byte) *aByteIter++;
          aPixmapBytes.g() = (Standard_Byte) *aByteIter++;
          aPixmapBytes.r() = (Standard_Byte) *aByteIter++;
          aPixmapBytes.a() = (Standard_Byte) *aByteIter++;
        }
      }

      anAISShape->Attributes()->ShadingAspect()->Aspect()->SetTextureMap
          (new Graphic3d_Texture2Dmanual(aPixmap));
      anAISShape->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn();

      anAISShape->SetDisplayMode(AIS_Shaded);
    }
  }
}

//******************************************************
void PartSet_Module::customizePresentation(const ObjectPtr& theObject,
                                           const AISObjectPtr& thePrs) const
{
  if (mySketchMgr->isObjectOfSketch(theObject)) {
    mySketchMgr->customizeSketchPresentation(theObject, thePrs);
  }
  else {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aResult.get()) {
      std::vector<int> aColor;
      bool isSameDoc = (ModelAPI_Session::get()->activeDocument() == aResult->document());
      // Get user defined color for the object
      ModelAPI_Tools::getColor(aResult, aColor);
      if (isSameDoc) {
        bool isCustomized = false;
        if (aColor.empty() && aFeature.get()) {
          GeomCustomPrsPtr aCustPrs = std::dynamic_pointer_cast<GeomAPI_ICustomPrs>(aFeature);
          if (aCustPrs.get()) {
            isCustomized = aCustPrs->customisePresentation(aResult, thePrs);
          }
        }
        if (!isCustomized) {
          if (aColor.empty()) {
            PartSet_Tools::getDefaultColor(aResult, false, aColor);
          }
          thePrs->setColor(aColor[0], aColor[1], aColor[2]);
        }
      }
      else {
        if (aColor.empty()) {
          PartSet_Tools::getDefaultColor(aResult, false, aColor);
        }
        QColor aQColor(aColor[0], aColor[1], aColor[2]);
        QColor aNewColor =
          QColor::fromHsvF(aQColor.hueF(), aQColor.saturationF() / 3., aQColor.valueF());
        thePrs->setColor(aNewColor.red(), aNewColor.green(), aNewColor.blue());
      }
      thePrs->setDeflection(getResultDeflection(aResult));
      thePrs->setTransparency(getResultTransparency(aResult));

      /// set texture parameters, if any
      setTexture(thePrs, aResult);
    }
    if (aFeature.get() && (aFeature->getKind() == SketchPlugin_Sketch::ID())) {
        thePrs->setWidth(2);
    }
  }
}


//******************************************************
ObjectPtr PartSet_Module::findPresentedObject(const AISObjectPtr& theAIS) const
{
  ObjectPtr anObject;
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation) {
    /// If last line finished on vertex the lines creation sequence has to be break
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    if (aPanel) {
      ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
      // if there is an active widget, find the presented object in it
      if (!anActiveWidget)
        anActiveWidget = aPanel->preselectionWidget();

      ModuleBase_WidgetValidated* aWidgetValidated = dynamic_cast<ModuleBase_WidgetValidated*>
                                                                             (anActiveWidget);
      if (aWidgetValidated)
        anObject = aWidgetValidated->findPresentedObject(theAIS);
    }
  }
  return anObject;
}

//******************************************************
bool PartSet_Module::canBeShaded(Handle(AIS_InteractiveObject) theAIS) const
{
  bool aCanBeShaged = true;

  Handle(PartSet_ResultSketchPrs) aPrs = Handle(PartSet_ResultSketchPrs)::DownCast(theAIS);
  if (!aPrs.IsNull())
    aCanBeShaged = false;

  return aCanBeShaged;
}

//******************************************************
void PartSet_Module::addObjectBrowserMenu(QMenu* theMenu) const
{
  QObjectPtrList aObjects = myWorkshop->selection()->selectedObjects();
  int aSelected = aObjects.size();
  SessionPtr aMgr = ModelAPI_Session::get();
  QAction* aActivatePartAction = myMenuMgr->action("ACTIVATE_PART_CMD");

  bool hasResult = false;
  bool hasFeature = false;
  bool hasParameter = false;
  bool hasCompositeOwner = false;
  bool hasResultInHistory = false;
  bool hasFolder = false;
  bool hasGroupsOnly = false;
  ModuleBase_Tools::checkObjects(aObjects, hasResult, hasFeature, hasParameter,
                                  hasCompositeOwner, hasResultInHistory, hasFolder, hasGroupsOnly);

  ModuleBase_Operation* aCurrentOp = myWorkshop->currentOperation();
  if (aSelected == 1) {
    ObjectPtr aObject = aObjects.first();
    if (aObject) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      bool isPart = aPart.get() ||
        (aFeature.get() && (aFeature->getKind() == PartSetPlugin_Part::ID()));
      if (isPart) {
        DocumentPtr aPartDoc;
        if (!aPart.get()) {
          aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFeature->firstResult());
        }
        if (aPart.get()) // this may be null is Part feature is disabled
          aPartDoc = aPart->partDoc();

        theMenu->addAction(aActivatePartAction);
        aActivatePartAction->setEnabled((aMgr->activeDocument() != aPartDoc));

      } else if (aObject->document() == aMgr->activeDocument()) {
        if (hasParameter || hasFeature) {

          bool isEnabled = true;
          if (aFeature.get()) {
            // disable Edit menu for not editable features
            isEnabled = aFeature->isEditable();

            // disable Edit menu for groups under ImportResult feature
            if (aFeature->getKind() == "Group") {
              std::shared_ptr<ModelAPI_CompositeFeature> anOwner =
                ModelAPI_Tools::compositeOwner (aFeature);
              if (anOwner.get() && anOwner->getKind() == "ImportResult") {
                isEnabled = false;
              }
            }
          }

          myMenuMgr->action("EDIT_CMD")->setEnabled(isEnabled);

          if (isEnabled) {
            theMenu->addAction(myMenuMgr->action("EDIT_CMD"));
            if (aCurrentOp && aFeature.get()) {
              if (aCurrentOp->id().toStdString() == aFeature->getKind())
                myMenuMgr->action("EDIT_CMD")->setEnabled(false);
            }
          }
        }
      }
    }
  } else {
    if (hasFeature) {
      myMenuMgr->action("EDIT_CMD")->setEnabled(aCurrentOp == 0);
      theMenu->addAction(myMenuMgr->action("EDIT_CMD"));
      theMenu->addSeparator();
    }
  }
  bool aNotDeactivate = (aCurrentOp == 0);
  if (!aNotDeactivate) {
    aActivatePartAction->setEnabled(false);
  }
}

//******************************************************
#define EXPAND_PARENT(OBJ) \
QModelIndex aObjIndex = aDataModel->objectIndex(OBJ); \
if (aObjIndex.isValid()) { \
  QModelIndex aParent = aObjIndex.parent(); \
  int aCount = aDataModel->rowCount(aParent); \
  if (aCount == 1) \
    aTreeView->setExpanded(aParent, true); \
}

//******************************************************
void PartSet_Module::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENTS_CLOSED)) {
    closeDocument();
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENT_CHANGED)) {
    SessionPtr aMgr = ModelAPI_Session::get();
    if (!aMgr->hasModuleDocument()) // if document is closed, do not call the document creation
      return;
    // Do not change activation of parts if an operation active
    static QStringList aAllowActivationList;
    if (aAllowActivationList.isEmpty())
      aAllowActivationList <<
      QString(PartSetPlugin_Part::ID().c_str()) <<
      QString(PartSetPlugin_Duplicate::ID().c_str()) <<
      QString(PartSetPlugin_Remove::ID().c_str());
    if (myWorkshop->currentOperation() &&
      (!aAllowActivationList.contains(myWorkshop->currentOperation()->id())))
      return;
    XGUI_Workshop* aWorkshop = getWorkshop();
    bool needUpdate = false;
    XGUI_DataTree* aTreeView = 0;
    if (aWorkshop->objectBrowser()) {
      aTreeView = aWorkshop->objectBrowser()->treeView();
      QLabel* aLabel = aWorkshop->objectBrowser()->activeDocLabel();
      QPalette aPalet = aLabel->palette();

      DocumentPtr aActiveDoc = aMgr->activeDocument();

      // Clear active part index if there is no Part documents
      // It could be not null if document was closed and opened a new
      // without closeDocument call
      if (aMgr->allOpenedDocuments().size() <= 1)
        myActivePartIndex = QModelIndex();

      XGUI_DataModel* aDataModel = aWorkshop->objectBrowser()->dataModel();
      QModelIndex aOldActive = myActivePartIndex;
      myActivePartIndex = aDataModel->documentRootIndex(aActiveDoc, 0);
      if (myActivePartIndex.isValid()) {
        needUpdate = aTreeView->isExpanded(myActivePartIndex);
        if (!needUpdate)
          aTreeView->setExpanded(myActivePartIndex, true);
      }
      if ((aOldActive != myActivePartIndex) && (aOldActive.isValid()))
        aTreeView->setExpanded(aOldActive, false);

      aLabel->setPalette(aPalet);
    }
    aWorkshop->updateCommandStatus();

    // Update displayed objects in order to update active color
    XGUI_Displayer* aDisplayer = aWorkshop->displayer();
    QObjectPtrList aObjects = aDisplayer->displayedObjects();
    bool aHidden;
    foreach(ObjectPtr aObj, aObjects) {
      aHidden = !aObj->data() || !aObj->data()->isValid() ||
        aObj->isDisabled() || (!aObj->isDisplayed());
      if (!aHidden) {
        aDisplayer->redisplay(aObj, false);
      }
    }
    aDisplayer->updateViewer();
    // Update tree items if they are expanded
    if (needUpdate) {
      aTreeView->viewport()->update(aTreeView->viewport()->rect());
    }
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    ObjectPtr aConstrObj;
    ObjectPtr aResultObj;
    std::set<ObjectPtr>::const_iterator aIt;
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
      ObjectPtr aObject = (*aIt);
      if ((!aResultObj.get()) && (aObject->groupName() == ModelAPI_ResultBody::group())
          && (aObject->document() != aRootDoc))
        aResultObj = aObject;
      if ((!aConstrObj.get()) && (aObject->groupName() == ModelAPI_ResultConstruction::group())
          && (aObject->document() != aRootDoc))
        aConstrObj = aObject;
      if (aResultObj.get() && aConstrObj.get())
        break;
    }

    if (aResultObj.get() || aConstrObj.get()) {
      XGUI_Workshop* aWorkshop = getWorkshop();
      XGUI_DataTree* aTreeView = aWorkshop->objectBrowser()->treeView();
      XGUI_DataModel* aDataModel = aWorkshop->objectBrowser()->dataModel();

      if (aResultObj.get()) {
        EXPAND_PARENT(aResultObj)
      }
      if (aConstrObj.get()) {
        EXPAND_PARENT(aConstrObj)
      }
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY)) {
    CompositeFeaturePtr aSketch = mySketchMgr->activeSketch();
    if (aSketch.get()) {
      ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
      if (PartSet_SketcherMgr::isSketchOperation(anOperation) &&
        mySketchMgr->previewSketchPlane()->isDisplayed())
        mySketchMgr->previewSketchPlane()->createSketchPlane(aSketch, myWorkshop);
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_LICENSE_VALID)) {
    std::shared_ptr<ModelAPI_FeaturesLicenseValidMessage> aMsg =
      std::dynamic_pointer_cast<ModelAPI_FeaturesLicenseValidMessage>(theMessage);
    myFeaturesValidLicense.insert(aMsg->features().begin(), aMsg->features().end());
    processProprietaryFeatures();
  }
}

//******************************************************
void PartSet_Module::onTreeViewDoubleClick(const QModelIndex& theIndex)
{
  if (myWorkshop->currentOperation()) // Do not change activation of parts if an operation active
    return;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (!theIndex.isValid()) {
    // It seems that this code is obsolete
    //aMgr->setActiveDocument(aMgr->moduleDocument());
    return;
  }
  if (theIndex.column() != 1) // Use only first column
    return;

  XGUI_Workshop* aWorkshop = getWorkshop();
  XGUI_DataModel* aDataModel = aWorkshop->objectBrowser()->dataModel();
  // De not use non editable Indexes
  if ((aDataModel->flags(theIndex) & Qt::ItemIsSelectable) == 0)
    return;
  ObjectPtr aObj = aDataModel->object(theIndex);

  ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
  if (!aPart.get()) { // Probably this is Feature
    FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aPartFeature.get() && (aPartFeature->getKind() == PartSetPlugin_Part::ID())) {
      aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult());
    }
    if (aObj.get())
    {
      if (!aPart.get() && aObj->groupName() == ModelAPI_ResultParameter::group())
      {
        QObjectPtrList aSelectedObjects = aWorkshop->objectBrowser()->selectedObjects();
        FeaturePtr aFeature;
        ResultParameterPtr aParam;
        foreach(ObjectPtr aSelected, aSelectedObjects) {
          aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aSelected);
          if (aParam.get())
            break;
        }
        if (aParam.get())
          aFeature = ModelAPI_Feature::feature(aParam);

        if (aFeature.get())
          editFeature(aFeature);
      }
    }
  }
  if (aPart.get()) { // if this is a part
    if (aPart->partDoc() == aMgr->activeDocument()) {
      myMenuMgr->activatePartSet();
    } else {
      myMenuMgr->activatePart(aPart);
    }
  }
}

//******************************************************
void PartSet_Module::onViewCreated(ModuleBase_IViewWindow*)
{
  // z layer is created for all started operations in order to visualize operation AIS presentation
  // over the object
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull())
    return;

  Handle(V3d_Viewer) aViewer = aContext->CurrentViewer();
  if (myVisualLayerId == 0) {
    if (myVisualLayerId == 0)
      aViewer->AddZLayer(myVisualLayerId);
  } else {
    TColStd_SequenceOfInteger aZList;
    aViewer->GetAllZLayers(aZList);
    bool aFound = false;
    for (int i = 1; i <= aZList.Length(); i++) {
      if (aZList(i) == myVisualLayerId) {
        aFound = true;
        break;
      }
    }
    if (!aFound)
      aViewer->AddZLayer(myVisualLayerId);
  }
  // if there is an active operation with validated widget,
  // the filters of this widget should be activated in the created view
  myWorkshop->selectionActivate()->updateSelectionFilters();
  myWorkshop->selectionActivate()->updateSelectionModes();
}

//******************************************************
void PartSet_Module::widgetStateChanged(int thePreviousState)
{
  mySketchMgr->widgetStateChanged(thePreviousState);
}

//******************************************************
bool PartSet_Module::processEnter(const std::string& thePreviousAttributeID)
{
  return mySketchReentrantMgr->processEnter(thePreviousAttributeID);
}

//******************************************************
void PartSet_Module::beforeOperationStopped(ModuleBase_Operation* theOperation)
{
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->stopSketch(theOperation);
  }
}

//******************************************************
GeomShapePtr PartSet_Module::findShape(const AttributePtr& theAttribute)
{
  GeomShapePtr aGeomShape;

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if (anOperation && sketchMgr()->isNestedSketchOperation(anOperation)) {
    aGeomShape = PartSet_Tools::findShapeBy2DPoint(theAttribute, myWorkshop);
  }
  return aGeomShape;
}

//******************************************************
AttributePtr PartSet_Module::findAttribute(const ObjectPtr& theObject,
                                           const GeomShapePtr& theGeomShape)
{
  AttributePtr anAttribute;
  GeomShapePtr aGeomShape = theGeomShape;
  if (!aGeomShape.get()) {
    // processing shape of result, e.g. sketch circle center is selected, this is a result
    // the geom shape is empty, the shape of result should be used
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get()) {
      aGeomShape = aResult->shape();
    }
  }

  if (aGeomShape.get()) {
    TopoDS_Shape aTDSShape = aGeomShape->impl<TopoDS_Shape>();
    std::pair<AttributePtr, int> anAttrAndIndex =
        PartSet_Tools::findAttributeBy2dPoint(theObject, aTDSShape, mySketchMgr->activeSketch());
    return anAttrAndIndex.first;
  }
  return anAttribute;
}

//******************************************************
std::shared_ptr<Events_Message> PartSet_Module::reentrantMessage()
{
  return sketchReentranceMgr()->reentrantMessage();
}

//******************************************************
void PartSet_Module::setReentrantPreSelection(const std::shared_ptr<Events_Message>& theMessage)
{
  sketchReentranceMgr()->setReentrantPreSelection(theMessage);
}

//******************************************************
void PartSet_Module::onChoiceChanged(ModuleBase_ModelWidget* theWidget,
                                     int theIndex)
{
  ModuleBase_WidgetChoice* aChoiceWidget = dynamic_cast<ModuleBase_WidgetChoice*>(theWidget);
  if (!aChoiceWidget)
    return;

  QString aChoiceTitle = aChoiceWidget->getPropertyPanelTitle(theIndex);
  if (!aChoiceTitle.isEmpty()) {
    ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
    if (!aOperation)
      return;
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    if (aPanel)
      aPanel->setWindowTitle(aChoiceTitle);
  }
}

//******************************************************
XGUI_Workshop* PartSet_Module::getWorkshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  return aConnector->workshop();
}

void PartSet_Module::setDefaultConstraintShown()
{
  myHasConstraintShown[PartSet_Tools::Geometrical] = true;
  myHasConstraintShown[PartSet_Tools::Dimensional] = true;
  myHasConstraintShown[PartSet_Tools::Expressions] = false;
}

//******************************************************
ModuleBase_ITreeNode* PartSet_Module::rootNode() const
{
  return myRoot;
}

//******************************************************
void PartSet_Module::disableCustomMode(ModuleBase_CustomizeFlag theMode) {
  myCustomPrs->disableCustomMode(theMode);
}

//******************************************************
void PartSet_Module::enableCustomModes() {
  myCustomPrs->enableCustomModes();
}

//******************************************************
void PartSet_Module::onRemoveConflictingConstraints()
{
  const std::set<ObjectPtr>& aConstraints = myOverconstraintListener->objectsToRemove();
  std::set<ObjectPtr>::const_iterator anIt;

  XGUI_Workshop* aWorkshop = getWorkshop();
  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();

  bool isAllowToNotify = ModuleBase_Preferences::resourceMgr()->booleanValue(SKETCH_TAB_NAME,
    "notify_change_constraint");

  if (isAllowToNotify) {
    anIt = aConstraints.begin();
    std::string aText("Conflict in constraints: \n");

    for (; anIt != aConstraints.end(); anIt++)
    {
      ObjectPtr anObject = *anIt;
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
      TCollection_AsciiString aStr(aFeature->name().c_str());
      std::string aName(aStr.ToCString());
      aText += aName + "\n";
    }

    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
    ModuleBase_Tools::warningAboutConflict(aConnector->desktop(),
      aText);
  }

  ModuleBase_Operation* anOp = anOpMgr->currentOperation();
  if (sketchMgr()->isNestedSketchOperation(anOp)) {
    std::set<FeaturePtr> aFeatures;
    for (anIt = aConstraints.cbegin(); anIt != aConstraints.cend(); anIt++)
      aFeatures.insert(ModelAPI_Feature::feature(*anIt));

    ModelAPI_Tools::removeFeaturesAndReferences(aFeatures);
  }
  else {
    QObjectPtrList anObjectsList;
    for (anIt = aConstraints.cbegin(); anIt != aConstraints.cend(); anIt++)
      anObjectsList.append(*anIt);

    QString aDescription = aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text();
    ModuleBase_Operation* anOpAction = new ModuleBase_Operation(aDescription);

    anOpMgr->startOperation(anOpAction);
    aWorkshop->deleteFeatures(anObjectsList);
    anOpMgr->commitOperation();
  }
  ModuleBase_Tools::flushUpdated(sketchMgr()->activeSketch());
}
