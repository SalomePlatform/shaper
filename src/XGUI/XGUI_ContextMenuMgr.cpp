// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "XGUI_ContextMenuMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_Selection.h"
#include "XGUI_SelectionActivate.h"
#include "XGUI_DataModel.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_ActionsMgr.h"

#ifndef HAVE_SALOME
#include <AppElements_MainWindow.h>
#endif

//#include "PartSetPlugin_Part.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_Folder.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_ResultField.h>

#include <Config_DataModelReader.h>

#include <ModuleBase_IModule.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_OperationAction.h>
#include <ModuleBase_ViewerPrs.h>

#include <QAction>
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMdiArea>
#include <QMainWindow>
#include <QModelIndex>

XGUI_ContextMenuMgr::XGUI_ContextMenuMgr(XGUI_Workshop* theParent)
    : QObject(theParent),
      myWorkshop(theParent),
      mySeparator1(0), mySeparator2(0), mySeparator3(0)
{
}

XGUI_ContextMenuMgr::~XGUI_ContextMenuMgr()
{
}

void XGUI_ContextMenuMgr::createActions()
{
#ifdef HAVE_SALOME
  QMainWindow* aDesktop = myWorkshop->salomeConnector()->desktop();
#else
  QMainWindow* aDesktop = myWorkshop->mainWindow();
#endif

  QAction* aAction = ModuleBase_Tools::createAction(QIcon(":pictures/delete.png"), tr("Delete"),
                                                    aDesktop);
  aDesktop->addAction(aAction);

  addAction("DELETE_CMD", aAction);
  aAction->setShortcutContext(Qt::ApplicationShortcut);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/rename_edit.png"), tr("Rename"),
                                           aDesktop, this, SLOT(onRename()));
  addAction("RENAME_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/move.png"),
                                           XGUI_Workshop::MOVE_TO_END_COMMAND, this);
  addAction("MOVE_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/clean_history.png"),
                                           tr("Clean history"), aDesktop);
  addAction("CLEAN_HISTORY_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/color.png"), tr("Color..."), aDesktop);
  addAction("COLOR_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(""), tr("Deflection..."), aDesktop);
  addAction("DEFLECTION_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/transparency.png"),
                                           tr("Transparency..."), aDesktop);
  addAction("TRANSPARENCY_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/eye_pencil.png"), tr("Show"), aDesktop);
  addAction("SHOW_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/eye_pencil.png"), tr("Show only"),
                                           aDesktop);
  addAction("SHOW_ONLY_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/eye_pencil_closed.png"), tr("Hide"),
                                           aDesktop);
  addAction("HIDE_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/eye_pencil_closed.png"), tr("Hide all"),
                                           aDesktop);
  addAction("HIDEALL_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/shading.png"), tr("Shading"), aDesktop);
  addAction("SHADING_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/wireframe.png"), tr("Wireframe"),
                                           aDesktop);
  addAction("WIREFRAME_CMD", aAction);

  mySeparator1 = ModuleBase_Tools::createAction(QIcon(), "", aDesktop);
  mySeparator1->setSeparator(true);

  mySeparator2 = ModuleBase_Tools::createAction(QIcon(), "", aDesktop);
  mySeparator2->setSeparator(true);

  mySeparator3 = ModuleBase_Tools::createAction(QIcon(), "", aDesktop);
  mySeparator3->setSeparator(true);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/vertex.png"), tr("Vertices"), aDesktop,
                                           this, SLOT(onShapeSelection(bool)));
  aAction->setCheckable(true);
  addAction("SELECT_VERTEX_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/edge.png"), tr("Edges"), aDesktop,
                                           this, SLOT(onShapeSelection(bool)));
  aAction->setCheckable(true);
  addAction("SELECT_EDGE_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/face.png"), tr("Faces"), aDesktop,
                                           this, SLOT(onShapeSelection(bool)));
  aAction->setCheckable(true);
  addAction("SELECT_FACE_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/result.png"), tr("Results"), aDesktop,
                                           this, SLOT(onResultSelection(bool)));
  aAction->setCheckable(true);
  addAction("SELECT_RESULT_CMD", aAction);

  aAction->setChecked(true);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/find_result.png"),
                                           tr("Select results"), aDesktop);
  addAction("SHOW_RESULTS_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/find_result.png"),
                                           tr("Select parent feature"), aDesktop);
  addAction("SHOW_FEATURE_CMD", aAction);

#ifdef TINSPECTOR
  aAction = ModuleBase_Tools::createAction(QIcon(), tr("TInspector"), aDesktop);
  addAction("TINSPECTOR_VIEW", aAction);
#endif

  // Features folders actions
  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/create_folder.png"),
                                           tr("Insert a folder before"), aDesktop);
  addAction("INSERT_FOLDER_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/insert_folder_before.png"),
                                           tr("Move into the previous folder"), aDesktop);
  addAction("ADD_TO_FOLDER_BEFORE_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/insert_folder_after.png"),
                                           tr("Move into the next folder"), aDesktop);
  addAction("ADD_TO_FOLDER_AFTER_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/move_out_before.png"),
                                           tr("Move out before the folder"), aDesktop);
  addAction("ADD_OUT_FOLDER_BEFORE_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/move_out_after.png"),
                                           tr("Move out after the folder"), aDesktop);
  addAction("ADD_OUT_FOLDER_AFTER_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/normal-view-inversed.png"),
                                           tr("Set view by inverted normal to face"), aDesktop);
  addAction("SET_VIEW_INVERTEDNORMAL_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":pictures/normal-view.png"),
                                           tr("Set view by normal to face"), aDesktop);
  addAction("SET_VIEW_NORMAL_CMD", aAction);

  buildObjBrowserMenu();
  buildViewerMenu();
}

void XGUI_ContextMenuMgr::addAction(const QString& theId, QAction* theAction)
{
  if (myActions.contains(theId))
    qCritical("A command with Id = '%s' already defined!", qPrintable(theId));
  theAction->setData(theId);
  connect(theAction, SIGNAL(triggered(bool)), this, SLOT(onAction(bool)));
  myActions[theId] = theAction;
}

QAction* XGUI_ContextMenuMgr::action(const QString& theId) const
{
  if (myActions.contains(theId))
    return myActions[theId];
  return 0;
}

QAction* XGUI_ContextMenuMgr::actionByName(const QString& theName) const
{
  foreach(QAction* eachAction, myActions) {
    if (eachAction->text() == theName) {
      return eachAction;
    }
  }
  return NULL;
}

QStringList XGUI_ContextMenuMgr::actionIds() const
{
  return myActions.keys();
}

void XGUI_ContextMenuMgr::onAction(bool isChecked)
{
  QAction* aAction = static_cast<QAction*>(sender());
  emit actionTriggered(aAction->data().toString(), isChecked);
}

void XGUI_ContextMenuMgr::updateCommandsStatus()
{
}

void XGUI_ContextMenuMgr::onContextMenuRequest(QContextMenuEvent* theEvent)
{
  QMenu* aMenu = new QMenu();
  if (sender() == myWorkshop->objectBrowser()) {
    updateObjectBrowserMenu();
    addObjBrowserMenu(aMenu);
  } else if (sender() == myWorkshop->viewer()) {
    updateViewerMenu();
    addViewerMenu(aMenu);
  }

  if (aMenu && (aMenu->actions().size() > 0)) {
    // it is possible that some objects should do something before and after the popup menu exec
    // e.g. a sketch manager changes an internal flag on this signals in order to do not hide
    // a created entity
    emit beforeContextMenu();
    aMenu->exec(theEvent->globalPos());
    emit afterContextMenu();
    delete aMenu;
  }
}

void XGUI_ContextMenuMgr::updateObjectBrowserMenu()
{
  foreach(QAction* aAction, myActions)
    aAction->setEnabled(false);

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QObjectPtrList aObjects = aSelMgr->selection()->selectedObjects();
  int aSelected = aObjects.size();
  if (aSelected > 0) {
    SessionPtr aMgr = ModelAPI_Session::get();
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    bool hasResult = false;
    bool hasFeature = false;
    bool hasParameter = false;
    bool hasCompositeOwner = false;
    bool hasResultInHistory = false;
    bool hasFolder = false;
    bool canBeDeleted = true;
    ModuleBase_Tools::checkObjects(aObjects, hasResult, hasFeature, hasParameter,
                                   hasCompositeOwner, hasResultInHistory, hasFolder);
    //Process Feature
    if (aSelected == 1) { // single selection
      ObjectPtr aObject = aObjects.first();
      if (aObject) {
        if (hasResult && myWorkshop->canBeShaded(aObject)) {
          XGUI_Displayer::DisplayMode aMode = aDisplayer->displayMode(aObject);
          if (aMode != XGUI_Displayer::NoMode) {
            action("WIREFRAME_CMD")->setEnabled(aMode == XGUI_Displayer::Shading);
            action("SHADING_CMD")->setEnabled(aMode == XGUI_Displayer::Wireframe);
          } else {
            action("WIREFRAME_CMD")->setEnabled(true);
            action("SHADING_CMD")->setEnabled(true);
          }
        }
        if (!hasFeature) {
          bool aHasSubResults = ModelAPI_Tools::hasSubResults(
                                            std::dynamic_pointer_cast<ModelAPI_Result>(aObject));
          if (aHasSubResults) {
            action("HIDE_CMD")->setEnabled(true);
            action("SHOW_CMD")->setEnabled(true);
          }
          else {
            if (aObject->isDisplayed()) {
              action("HIDE_CMD")->setEnabled(true);
            } else if (hasResult && (!hasParameter)) {
              action("SHOW_CMD")->setEnabled(true);
            }
          }
          if (!(hasParameter || hasFeature))
            action("SHOW_ONLY_CMD")->setEnabled(true);
        }
        else if (hasFeature && myWorkshop->canMoveFeature())
          action("MOVE_CMD")->setEnabled(true);

        if( aMgr->activeDocument() == aObject->document() )
        {
          action("RENAME_CMD")->setEnabled(true);
          if (aObject->groupName() == ModelAPI_ResultConstruction::group()) {
            ResultConstructionPtr aConstr = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObject);
            FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
            canBeDeleted = !(!(aFeature->isInHistory()) && aConstr->isInfinite());
            action("DELETE_CMD")->setEnabled(canBeDeleted);
          }
          else
            action("DELETE_CMD")->setEnabled(!hasCompositeOwner);
          action("CLEAN_HISTORY_CMD")->setEnabled(!hasCompositeOwner &&
                                                  (hasFeature || hasParameter));
        }
      }
      // end single selection
    } else { // multi-selection
      // parameter is commented because the actions are not in the list of result parameter actions
      if (hasResult /*&& (!hasParameter)*/) {
        action("SHOW_CMD")->setEnabled(true);
        action("HIDE_CMD")->setEnabled(true);
        action("SHOW_ONLY_CMD")->setEnabled(true);
        action("SHADING_CMD")->setEnabled(true);
        action("WIREFRAME_CMD")->setEnabled(true);

        foreach(ObjectPtr aObj, aObjects) {
          FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
          if (!aFeature->isInHistory()) {
            canBeDeleted = false;
            break;
          }
        }
        action("DELETE_CMD")->setEnabled(canBeDeleted);
      }
      if (hasFeature && myWorkshop->canMoveFeature())
        action("MOVE_CMD")->setEnabled(true);
    } // end multi-selection

    // Check folder management commands state if only features are selected
    if ((!hasResult) && hasFeature && (!hasParameter) && (!hasCompositeOwner) &&
      (!hasResultInHistory) && (!hasFolder)) {
      std::list<FeaturePtr> aFeatures = aSelMgr->getSelectedFeatures();
      if (aFeatures.size() > 0) { // Check that features do not include Parts
        QModelIndexList aIndexes = aSelMgr->selection()->selectedIndexes();
        QModelIndex aFirstIdx = aIndexes.first();
        QModelIndex aLastIdx = aIndexes.last();
        QModelIndex aParentIdx = aFirstIdx.parent();

        // if all selected are from the same level
        bool isSameParent = true;
        foreach(QModelIndex aIdx, aIndexes) {
          if (aIdx.parent() != aParentIdx) {
            isSameParent = false;
            break;
          }
        }
        if (isSameParent) {
          // Check is selection continuous
          XGUI_DataModel* aModel = myWorkshop->objectBrowser()->dataModel();
          DocumentPtr aDoc = aMgr->activeDocument();
          std::list<FeaturePtr> aFeatures = aSelMgr->getSelectedFeatures();

          bool isContinuos = true;
          if (aSelected > 1) {
            int aId = -1;
            foreach(FeaturePtr aF, aFeatures) {
              if (aId == -1)
                aId = aDoc->index(aF);
              else {
                aId++;
                if (aId != aDoc->index(aF)) {
                  isContinuos = false;
                  break;
                }
              }
            }
          }
          if (isContinuos) {
            ObjectPtr aDataObj = aModel->object(aParentIdx);

            ObjectPtr aPrevObj;
            if (aFirstIdx.row() > 0) {
              QModelIndex aPrevIdx = aFirstIdx.sibling(aFirstIdx.row() - 1, 0);
              aPrevObj = aModel->object(aPrevIdx);
            }

            ObjectPtr aNextObj;
            if (aLastIdx.row() < (aModel->rowCount(aParentIdx) - 1)) {
              QModelIndex aNextIdx = aFirstIdx.sibling(aLastIdx.row() + 1, 0);
              aNextObj = aModel->object(aNextIdx);
            }

            bool isPrevFolder = (aPrevObj.get() &&
              (aPrevObj->groupName() == ModelAPI_Folder::group()));
            bool isNextFolder = (aNextObj.get() &&
              (aNextObj->groupName() == ModelAPI_Folder::group()));
            bool isInFolder = (aDataObj.get() &&
              (aDataObj->groupName() == ModelAPI_Folder::group()));
            bool isOutsideFolder = !isInFolder;

            bool hasFirst = false;
            bool hasLast = false;
            if (isInFolder) {
              FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(aDataObj);
              FeaturePtr aFirstFeatureInFolder;
              AttributeReferencePtr aFirstFeatAttr =
                  aFolder->data()->reference(ModelAPI_Folder::FIRST_FEATURE_ID());
              if (aFirstFeatAttr)
                aFirstFeatureInFolder = ModelAPI_Feature::feature(aFirstFeatAttr->value());
              hasFirst = (aFirstFeatureInFolder == aFeatures.front());

              FeaturePtr aLastFeatureInFolder;
              AttributeReferencePtr aLastFeatAttr =
                  aFolder->data()->reference(ModelAPI_Folder::LAST_FEATURE_ID());
              if (aLastFeatAttr)
                aLastFeatureInFolder = ModelAPI_Feature::feature(aLastFeatAttr->value());
              hasLast = (aLastFeatureInFolder == aFeatures.back());
            }
            action("INSERT_FOLDER_CMD")->setEnabled(isOutsideFolder);
            action("ADD_TO_FOLDER_BEFORE_CMD")->setEnabled(isOutsideFolder && isPrevFolder);
            action("ADD_TO_FOLDER_AFTER_CMD")->setEnabled(isOutsideFolder && isNextFolder);
            action("ADD_OUT_FOLDER_BEFORE_CMD")->setEnabled(isInFolder && hasFirst);
            action("ADD_OUT_FOLDER_AFTER_CMD")->setEnabled(isInFolder && hasLast);
          }
        }
      }
    } // end folder management commands

    bool allActive = true;
    foreach( ObjectPtr aObject, aObjects )
      if( aMgr->activeDocument() != aObject->document() )  {
        allActive = false;
        break;
      }
    if (!hasCompositeOwner && allActive ) {
      if (hasResult || hasFeature || hasParameter)  // #2924 results can be erased
        action("DELETE_CMD")->setEnabled(canBeDeleted);
    }
    if (!hasCompositeOwner && allActive && (hasFeature|| hasParameter))
      action("CLEAN_HISTORY_CMD")->setEnabled(true);

    action("SHOW_RESULTS_CMD")->setEnabled(hasFeature);
    action("SHOW_FEATURE_CMD")->setEnabled(hasResult && hasResultInHistory);
  } // end selection processing

  // Show/Hide command has to be disabled for objects from non active document
  bool aDeactivate = false;
  foreach (ObjectPtr aObj, aObjects) {
    if (!aObj->document()->isActive()) {
      if ((aObj->document() != ModelAPI_Session::get()->moduleDocument()) ||
           aObj->groupName() == ModelAPI_ResultPart::group()) {
        aDeactivate = true;
        break;
      }
    }
  }
  if (aDeactivate) {
    // If at leas a one object can not be edited then Show/Hide has to be disabled
    action("SHOW_CMD")->setEnabled(false);
    action("HIDE_CMD")->setEnabled(false);
    action("SHOW_ONLY_CMD")->setEnabled(false);
  }

  action("COLOR_CMD")->setEnabled(myWorkshop->canChangeProperty("COLOR_CMD"));
  action("DEFLECTION_CMD")->setEnabled(myWorkshop->canChangeProperty("DEFLECTION_CMD"));
  action("TRANSPARENCY_CMD")->setEnabled(myWorkshop->canChangeProperty("TRANSPARENCY_CMD"));
  #ifdef _DEBUG
    #ifdef TINSPECTOR
      action("TINSPECTOR_VIEW")->setEnabled(true);
    #endif
  #endif


  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule)
    aModule->updateObjectBrowserMenu(myActions);
}

void XGUI_ContextMenuMgr::updateViewerMenu()
{
  foreach(QAction* aAction, myActions)
    aAction->setEnabled(false);

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  QList<ModuleBase_ViewerPrsPtr> aPrsList =
    aSelMgr->selection()->getSelected(ModuleBase_ISelection::Viewer);
  if (aPrsList.size() > 0) {
    bool isVisible = false;
    bool isShading = false;
    bool canBeShaded = false;
    bool hasPlanar = false;
    ObjectPtr aObject;
    foreach(ModuleBase_ViewerPrsPtr aPrs, aPrsList) {
      aObject = aPrs->object();
      if (!aObject.get())
        continue;
      GeomShapePtr aShape = aPrs->shape();
      if (aObject->isDisplayed()) {
        isVisible = true;
        canBeShaded = myWorkshop->displayer()->canBeShaded(aObject);
        isShading =
          (myWorkshop->displayer()->displayMode(aObject) == XGUI_Displayer::Shading);
      }
      if (aShape.get()) {
        if (aShape->isPlanar()) {
          hasPlanar = true;
        }
      }
    }
    if (isVisible) {
      if (canBeShaded) {
        XGUI_Displayer::DisplayMode aMode = aDisplayer->displayMode(aObject);
        if (aMode != XGUI_Displayer::NoMode) {
          action("WIREFRAME_CMD")->setEnabled(aMode == XGUI_Displayer::Shading);
          action("SHADING_CMD")->setEnabled(aMode == XGUI_Displayer::Wireframe);
        } else {
          action("WIREFRAME_CMD")->setEnabled(true);
          action("SHADING_CMD")->setEnabled(true);
        }
      }
      action("SHOW_ONLY_CMD")->setEnabled(true);
      action("HIDE_CMD")->setEnabled(true);
    } else
      action("SHOW_CMD")->setEnabled(true);

    action("SET_VIEW_NORMAL_CMD")->setEnabled(hasPlanar);
    action("SET_VIEW_INVERTEDNORMAL_CMD")->setEnabled(hasPlanar);
  }
  //issue #2159 Hide all incomplete behavior
#ifdef HAVE_SALOME
    action("HIDEALL_CMD")->setEnabled(true);
#else
  if (myWorkshop->displayer()->objectsCount() > 0)
    action("HIDEALL_CMD")->setEnabled(true);
#endif

  // Update selection menu
  QIntList aModes = myWorkshop->selectionActivate()->activeSelectionModes();
  action("SELECT_VERTEX_CMD")->setEnabled(true);
  action("SELECT_EDGE_CMD")->setEnabled(true);
  action("SELECT_FACE_CMD")->setEnabled(true);
  action("SELECT_RESULT_CMD")->setEnabled(true);

  action("SELECT_RESULT_CMD")->setChecked(false);
  action("SELECT_VERTEX_CMD")->setChecked(false);
  action("SELECT_EDGE_CMD")->setChecked(false);
  action("SELECT_FACE_CMD")->setChecked(false);
  action("SELECT_RESULT_CMD")->setChecked(false);
  if (aModes.count() == 0) {
    action("SELECT_RESULT_CMD")->setChecked(true);
  } else {
    foreach(int aMode, aModes) {
      switch (aMode) {
      case TopAbs_VERTEX:
        action("SELECT_VERTEX_CMD")->setChecked(true);
        break;
      case TopAbs_EDGE:
        action("SELECT_EDGE_CMD")->setChecked(true);
        break;
      case TopAbs_FACE:
        action("SELECT_FACE_CMD")->setChecked(true);
        break;
      default:
        action("SELECT_RESULT_CMD")->setChecked(true);
      }
    }
  }

  if (myWorkshop->canChangeProperty("COLOR_CMD"))
    action("COLOR_CMD")->setEnabled(true);

  if (myWorkshop->canChangeProperty("DEFLECTION_CMD"))
    action("DEFLECTION_CMD")->setEnabled(true);

  if (myWorkshop->canChangeProperty("TRANSPARENCY_CMD"))
    action("TRANSPARENCY_CMD")->setEnabled(true);

  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule)
    aModule->updateViewerMenu(myActions);
}

void XGUI_ContextMenuMgr::connectObjectBrowser()
{
  connect(myWorkshop->objectBrowser(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequest(QContextMenuEvent*)));
}

void XGUI_ContextMenuMgr::connectViewer()
{
  connect(myWorkshop->viewer(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequest(QContextMenuEvent*)));
}


void XGUI_ContextMenuMgr::buildObjBrowserMenu()
{
  QAction* aSeparator = ModuleBase_Tools::createAction(QIcon(), "", myWorkshop->desktop());
  aSeparator->setSeparator(true);

  QActionsList aList;

  // Result construction menu
  aList.append(action("SHOW_CMD"));
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(mySeparator1);
  aList.append(action("RENAME_CMD"));
  aList.append(action("COLOR_CMD"));
  aList.append(action("DEFLECTION_CMD"));
  aList.append(action("TRANSPARENCY_CMD"));
  aList.append(action("SHOW_FEATURE_CMD"));
  aList.append(mySeparator2);
  aList.append(action("DELETE_CMD"));
  myObjBrowserMenus[ModelAPI_ResultConstruction::group()] = aList;

  //-------------------------------------
  // Result body menu
  aList.clear();
  aList.append(action("WIREFRAME_CMD"));
  aList.append(action("SHADING_CMD"));
  aList.append(mySeparator1); // this separator is not shown as this action is added after show only
  // qt list container contains only one instance of the same action
  aList.append(action("SHOW_CMD"));
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(mySeparator2);
  aList.append(action("RENAME_CMD"));
  aList.append(action("COLOR_CMD"));
  aList.append(action("DEFLECTION_CMD"));
  aList.append(action("TRANSPARENCY_CMD"));
  aList.append(action("SHOW_FEATURE_CMD"));
  aList.append(mySeparator3);
  aList.append(action("DELETE_CMD"));
  myObjBrowserMenus[ModelAPI_ResultBody::group()] = aList;
  // Group menu
  myObjBrowserMenus[ModelAPI_ResultGroup::group()] = aList;
  myObjBrowserMenus[ModelAPI_ResultField::group()] = aList;
  // Result part menu
  myObjBrowserMenus[ModelAPI_ResultPart::group()] = aList;
  //-------------------------------------
  // Feature menu
  aList.clear();
  aList.append(action("RENAME_CMD"));
  aList.append(action("SHOW_RESULTS_CMD"));
  aList.append(action("MOVE_CMD"));
  aList.append(mySeparator1);
  aList.append(action("INSERT_FOLDER_CMD"));
  aList.append(action("ADD_TO_FOLDER_BEFORE_CMD"));
  aList.append(action("ADD_TO_FOLDER_AFTER_CMD"));
  aList.append(mySeparator2);
  aList.append(action("ADD_OUT_FOLDER_BEFORE_CMD"));
  aList.append(action("ADD_OUT_FOLDER_AFTER_CMD"));
  aList.append(mySeparator3);
  aList.append(action("CLEAN_HISTORY_CMD"));
  aList.append(action("DELETE_CMD"));
  myObjBrowserMenus[ModelAPI_Feature::group()] = aList;

  aList.clear();
  aList.append(action("RENAME_CMD"));
  aList.append(mySeparator1);
  aList.append(action("CLEAN_HISTORY_CMD"));
  aList.append(action("DELETE_CMD"));
  myObjBrowserMenus[ModelAPI_ResultParameter::group()] = aList;
  //-------------------------------------

  aList.clear();
  aList.append(action("RENAME_CMD"));
  aList.append(action("DELETE_CMD"));
  myObjBrowserMenus[ModelAPI_Folder::group()] = aList;

  //---------------------------------------
  // Step objects menu
  aList.clear();
  aList.append(action("SHOW_CMD"));
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  myObjBrowserMenus[ModelAPI_ResultField::ModelAPI_FieldStep::group()] = aList;
}

void XGUI_ContextMenuMgr::buildViewerMenu()
{
  QActionsList aList;
  // Result construction menu
  aList.append(action("COLOR_CMD"));
  aList.append(action("DEFLECTION_CMD"));
  aList.append(action("TRANSPARENCY_CMD"));
  aList.append(mySeparator3);
  aList.append(action("SET_VIEW_NORMAL_CMD"));
  aList.append(action("SET_VIEW_INVERTEDNORMAL_CMD"));
  aList.append(mySeparator1);
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(action("HIDE_CMD"));
  myViewerMenu[ModelAPI_ResultConstruction::group()] = aList;
  // Result part menu
  myViewerMenu[ModelAPI_ResultPart::group()] = aList;
  //-------------------------------------
  // Result body menu
  aList.clear();
  aList.append(action("WIREFRAME_CMD"));
  aList.append(action("SHADING_CMD"));
  aList.append(mySeparator2);
  aList.append(action("COLOR_CMD"));
  aList.append(action("DEFLECTION_CMD"));
  aList.append(action("TRANSPARENCY_CMD"));
  aList.append(mySeparator3);
  aList.append(action("SET_VIEW_NORMAL_CMD"));
  aList.append(action("SET_VIEW_INVERTEDNORMAL_CMD"));
  aList.append(mySeparator1);
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(action("HIDE_CMD"));
  myViewerMenu[ModelAPI_ResultBody::group()] = aList;
  // Group menu
  myViewerMenu[ModelAPI_ResultGroup::group()] = aList;
  myViewerMenu[ModelAPI_ResultField::group()] = aList;
  //-------------------------------------
  // Step objects menu
  aList.clear();
  aList.append(action("HIDE_CMD"));
  myViewerMenu[ModelAPI_ResultField::ModelAPI_FieldStep::group()] = aList;
}


void XGUI_ContextMenuMgr::addObjBrowserMenu(QMenu* theMenu) const
{
  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) {
    theMenu->addSeparator();
    aModule->addObjectBrowserMenu(theMenu);
  }

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QObjectPtrList aObjects = aSelMgr->selection()->selectedObjects();
  int aSelected = aObjects.size();
  QActionsList aActions;
  if (aSelected == 1) {
    ObjectPtr aObject = aObjects.first();
    std::string aName = aObject->groupName();
    if (myObjBrowserMenus.contains(aName))
      aActions = myObjBrowserMenus[aName];
  } else if (aSelected > 1) {
      aActions.append(action("WIREFRAME_CMD"));
      aActions.append(action("SHADING_CMD"));
      aActions.append(mySeparator1);
      aActions.append(action("SHOW_CMD"));
      aActions.append(action("HIDE_CMD"));
      aActions.append(action("SHOW_ONLY_CMD"));
      aActions.append(mySeparator2);
      aActions.append(action("ADD_TO_FOLDER_BEFORE_CMD"));
      aActions.append(action("ADD_TO_FOLDER_AFTER_CMD"));
      aActions.append(action("ADD_OUT_FOLDER_BEFORE_CMD"));
      aActions.append(action("ADD_OUT_FOLDER_AFTER_CMD"));
      aActions.append(mySeparator3);
      aActions.append(action("MOVE_CMD"));
      aActions.append(action("COLOR_CMD"));
      aActions.append(action("DEFLECTION_CMD"));
      aActions.append(action("TRANSPARENCY_CMD"));
      aActions.append(action("CLEAN_HISTORY_CMD"));
      aActions.append(action("DELETE_CMD"));
  }
#ifdef _DEBUG
  if (aSelected == 0) {
    #ifdef TINSPECTOR
    aActions.append(action("TINSPECTOR_VIEW"));
    #endif
  }
#endif
  theMenu->addActions(aActions);
  addFeatures(theMenu);

  // It is commented out because Object Browser does not have actions
  //theMenu->addSeparator();
  //theMenu->addActions(myWorkshop->objectBrowser()->actions());
}

void XGUI_ContextMenuMgr::addViewerMenu(QMenu* theMenu) const
{
  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QList<ModuleBase_ViewerPrsPtr> aPrsList =
    aSelMgr->selection()->getSelected(ModuleBase_ISelection::Viewer);
  int aSelected = aPrsList.size();
  QActionsList aActions;

  // Create selection menu
  XGUI_OperationMgr* aOpMgr = myWorkshop->operationMgr();
  if (!aOpMgr->hasOperation() &&
      myWorkshop->selectionActivate()->activeSelectionPlace() == XGUI_SelectionActivate::Workshop) {
    QMenu* aSelMenu = new QMenu(tr("Selection mode"), theMenu);
    aSelMenu->addAction(action("SELECT_VERTEX_CMD"));
    aSelMenu->addAction(action("SELECT_EDGE_CMD"));
    aSelMenu->addAction(action("SELECT_FACE_CMD"));
    //IMP: an attempt to use result selection with other selection modes
    //aSelMenu->addSeparator();
    aSelMenu->addAction(action("SELECT_RESULT_CMD"));
    theMenu->addMenu(aSelMenu);
    theMenu->addSeparator();
  }
  if (aSelected == 1) {
    ObjectPtr aObject = aPrsList.first()->object();
    if (aObject.get() != NULL) {
      std::string aName = aObject->groupName();
      if (myViewerMenu.contains(aName))
        aActions = myViewerMenu[aName];
    }
  } else if (aSelected > 1) {
    aActions.append(action("COLOR_CMD"));
    aActions.append(action("DEFLECTION_CMD"));
    aActions.append(action("TRANSPARENCY_CMD"));
    aActions.append(mySeparator1);
    aActions.append(action("SHOW_ONLY_CMD"));
    aActions.append(action("HIDE_CMD"));
  }
  // hide all is shown always even if selection in the viewer is empty
  aActions.append(action("HIDEALL_CMD"));
  theMenu->addActions(aActions);

  QMap<int, QAction*> aMenuActions;
  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) {
    if (aModule->addViewerMenu(myActions, theMenu, aMenuActions))
      theMenu->addSeparator();
  }

  // insert the module menu items on specific positions in the popup menu: some actions should be
  // in the begin of the list, Delete action should be the last by #1343 issue
  QList<QAction*> anActions = theMenu->actions();
  int anActionsSize = anActions.size();
  QAction* aFirstAction = anActions[0];
  QMap<int, QAction*>::const_iterator anIt = aMenuActions.begin(), aLast = aMenuActions.end();
  for (; anIt != aLast; anIt++) {
    if (anIt.key() > anActionsSize)
      theMenu->addAction(anIt.value());
    else
      theMenu->insertAction(aFirstAction, *anIt);
  }

#ifndef HAVE_SALOME
  theMenu->addSeparator();
  QMdiArea* aMDI = myWorkshop->mainWindow()->mdiArea();
  if (aMDI->actions().size() > 0) {
    QMenu* aSubMenu = theMenu->addMenu(tr("Windows"));
    aSubMenu->addActions(aMDI->actions());
  }
#endif
}

QStringList XGUI_ContextMenuMgr::actionObjectGroups(const QString& theName)
{
  QStringList aGroups;

  QMap<std::string, QActionsList>::const_iterator anIt = myObjBrowserMenus.begin(),
                                                  aLast = myObjBrowserMenus.end();
  for (; anIt != aLast; anIt++) {
    QString aGroupName(anIt.key().c_str());
    if (aGroups.contains(aGroupName))
      continue;
    QActionsList anActions = anIt.value();
    QActionsList::const_iterator anAIt = anActions.begin(), anALast = anActions.end();
    bool aFound = false;
    for (; anAIt != anALast && !aFound; anAIt++)
      aFound = (*anAIt)->data().toString() == theName;
    if (aFound)
      aGroups.append(aGroupName);
  }
  return aGroups;
}

void XGUI_ContextMenuMgr::onRename()
{
  QObjectPtrList anObjects = myWorkshop->selector()->selection()->selectedObjects();
  if (!myWorkshop->abortAllOperations())
    return;
  // restore selection in case if dialog box was shown
  myWorkshop->objectBrowser()->setObjectsSelected(anObjects);
  myWorkshop->objectBrowser()->onEditItem();
}

void XGUI_ContextMenuMgr::addFeatures(QMenu* theMenu) const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aActiveDoc = aMgr->activeDocument();

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  XGUI_ActionsMgr* aActionMgr = myWorkshop->actionsMgr();
  const Config_DataModelReader* aDataModelXML = myWorkshop->dataModelXMLReader();
  QModelIndexList aSelectedIndexes = aSelMgr->selection()->selectedIndexes();

  QString aName;
  int aLen = 0;
  bool aIsRoot = false;
  foreach(QModelIndex aIdx, aSelectedIndexes) {
    // Process only first column
    if (aIdx.column() == 1) {
      aIsRoot = !aIdx.parent().isValid();
      // Exit if the selected index belongs to non active document
      if (aIsRoot && (aActiveDoc != aMgr->moduleDocument()))
        return;

      // Get name of the selected index
      aName = aIdx.data().toString();
      aLen = aName.indexOf('(');
      if (aLen != -1) {
        aName = aName.left(--aLen);
      }
      std::string aFeaturesStr = aIsRoot?
        aDataModelXML->rootFolderFeatures(aName.toStdString()) :
        aDataModelXML->subFolderFeatures(aName.toStdString());
        if (aFeaturesStr.length() > 0) {
          QStringList aFeturesList =
            QString(aFeaturesStr.c_str()).split(",", QString::SkipEmptyParts);
          foreach(QString aFea, aFeturesList) {
            QAction* aAction = aActionMgr->action(aFea);
            if (aAction)
              theMenu->addAction(aAction);
          }
        }
    }
  }
}

#define UNCHECK_ACTION(NAME) \
{ QAction* aAction = action(NAME); \
bool isBlock = aAction->signalsBlocked(); \
aAction->blockSignals(true); \
aAction->setChecked(false); \
  aAction->blockSignals(isBlock); }


void XGUI_ContextMenuMgr::onResultSelection(bool theChecked)
{
  UNCHECK_ACTION("SELECT_VERTEX_CMD");
  UNCHECK_ACTION("SELECT_EDGE_CMD");
  UNCHECK_ACTION("SELECT_FACE_CMD");
}

void XGUI_ContextMenuMgr::onShapeSelection(bool theChecked)
{
  UNCHECK_ACTION("SHOW_RESULTS_CMD");
}
