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

#include "PartSet_MenuMgr.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_Tools.h"

#include <PartSetPlugin_Part.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_DataModel.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_ObjectsBrowser.h>
#include <XGUI_ViewerProxy.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultParameter.h>

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QApplication>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

PartSet_MenuMgr::PartSet_MenuMgr(PartSet_Module* theModule)
  : QObject(theModule), myModule(theModule), myPrevId(-1)
{
  createActions();
}


QAction* PartSet_MenuMgr::action(const QString& theId) const
{
  if (myActions.contains(theId))
    return myActions[theId];
  return 0;
}

void PartSet_MenuMgr::addAction(const QString& theId, QAction* theAction)
{
  if (myActions.contains(theId))
    qCritical("A command with Id = '%s' already defined!", qPrintable(theId));
  theAction->setData(theId);
  connect(theAction, SIGNAL(triggered(bool)), this, SLOT(onAction(bool)));
  myActions[theId] = theAction;
}

void PartSet_MenuMgr::createActions()
{
  QAction* aAction;

  QWidget* aParent = myModule->workshop()->desktop();
  aAction = ModuleBase_Tools::createAction(QIcon(), tr("Auxiliary"), aParent);
  aAction->setCheckable(true);
  addAction("AUXILIARY_CMD", aAction);

  aAction = ModuleBase_Tools::createAction(QIcon(":icons/activate.png"), tr("Activate"), aParent,
                                           this, SLOT(onActivatePart(bool)));
  myActions["ACTIVATE_PART_CMD"] = aAction;

  // Activate PartSet
  aAction = ModuleBase_Tools::createAction(QIcon(":icons/activate.png"), tr("Activate"), aParent,
                        this, SLOT(onActivatePartSet(bool)));
  myActions["ACTIVATE_PARTSET_CMD"] = aAction;

  aAction = ModuleBase_Tools::createAction(QIcon(":icons/edit.png"), tr("Edit..."), aParent,
                         this, SLOT(onEdit(bool)));
  myActions["EDIT_CMD"] = aAction;

  aAction = ModuleBase_Tools::createAction(QIcon(":icons/activate.png"), tr("Load all parts"),
    aParent, this, SLOT(onActivateAllParts()));
  myActions["ACTIVATE_ALL_PARTS_CMD"] = aAction;
}


void PartSet_MenuMgr::onAction(bool isChecked)
{
  QAction* aAction = static_cast<QAction*>(sender());
  QString anId = aAction->data().toString();

  if (anId == "AUXILIARY_CMD") {
    setAuxiliary(isChecked);
  }
}

bool PartSet_MenuMgr::addViewerMenu(const QMap<QString, QAction*>& theStdActions,
                                    QWidget* theParent,
                                    QMap<int, QAction*>& theMenuActions) const
{
  int anIndex = 0;

  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();
  if (!PartSet_SketcherMgr::isSketchOperation(anOperation) &&
      !myModule->sketchMgr()->isNestedSketchOperation(anOperation))
    return false;

  myCoinsideLines.clear();
  ModuleBase_ISelection* aSelection = myModule->workshop()->selection();

  bool aIsDetach = false;
  bool hasAttribute = false;
  bool hasFeature = false;

  QList<ModuleBase_ViewerPrsPtr> aPrsList = aSelection->getSelected(ModuleBase_ISelection::Viewer);
  if (aPrsList.size() > 1) {
    hasFeature = true;
  } else if (aPrsList.size() == 1) {
    ResultPtr aResult;
    FeaturePtr aFeature;
    foreach(ModuleBase_ViewerPrsPtr aPrs, aPrsList) {
      aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs->object());
      if (aResult.get() != NULL) {
        const GeomShapePtr& aShape = aPrs->shape();
        if (aShape.get() && aShape->isEqual(aResult->shape()))
          hasFeature = true;
        else
          hasAttribute = true;
      } else {
        aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aPrs->object());
        hasFeature = (aFeature.get() != NULL);
      }
    }

    const GeomShapePtr& aShape = aPrsList.first()->shape();
    if (aShape.get() && !aShape->isNull() && aShape->shapeType() == GeomAPI_Shape::VERTEX) {
      // Find 2d coordinates
      FeaturePtr aSketchFea = myModule->sketchMgr()->activeSketch();
      if (aSketchFea->getKind() == SketchPlugin_Sketch::ID()) {
        const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
        gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aTDShape));
        std::shared_ptr<GeomAPI_Pnt> aPnt3d(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
        std::shared_ptr<GeomAPI_Pnt2d> aSelPnt = PartSet_Tools::convertTo2D(aSketchFea, aPnt3d);

        // Find coincident in these coordinates
        ObjectPtr aObj = aPrsList.first()->object();
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
        FeaturePtr aCoincident = PartSet_Tools::findFirstCoincidence(aFeature, aSelPnt);
        // If we have coincidence then add Detach menu
        if (aCoincident.get() != NULL) {
          QList<FeaturePtr> aCoins;
          mySelectedFeature = aCoincident;
          QList<bool> anIsAttributes;
          PartSet_Tools::findCoincidences(mySelectedFeature, myCoinsideLines, aCoins,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                          anIsAttributes);
          PartSet_Tools::findCoincidences(mySelectedFeature, myCoinsideLines, aCoins,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                          anIsAttributes);
          if (myCoinsideLines.size() > 0) {
            aIsDetach = true;
            QMenu* aSubMenu = new QMenu(tr("Detach"), theParent);
            theMenuActions[anIndex++] = aSubMenu->menuAction();
            QAction* aAction;
            int i = 0;
            foreach (FeaturePtr aCoins, myCoinsideLines) {
              QString anItemText = aCoins->data()->name().c_str();
#ifdef _DEBUG
              if (anIsAttributes[i])
                anItemText += " [attribute]";
#endif
              aAction = aSubMenu->addAction(anItemText);
              aAction->setData(QVariant(i));
              i++;
            }
            connect(aSubMenu, SIGNAL(hovered(QAction*)), SLOT(onLineHighlighted(QAction*)));
            connect(aSubMenu, SIGNAL(aboutToHide()), SLOT(onDetachMenuHide()));
            connect(aSubMenu, SIGNAL(triggered(QAction*)), SLOT(onLineDetach(QAction*)));
          }
        }
      }
    }
  }
  if (!hasAttribute) {
    bool isAuxiliary;
    if (canSetAuxiliary(isAuxiliary)) {
      QAction* anAction = action("AUXILIARY_CMD");
      theMenuActions[anIndex++] = anAction;
      anAction->setChecked(isAuxiliary);
    }
  }

  if (!aIsDetach && hasFeature) {
    // Delete item should be the last in the list of actions
    theMenuActions[1000] = theStdActions["DELETE_CMD"];
  }

  return true;
}

void PartSet_MenuMgr::updateViewerMenu(const QMap<QString, QAction*>& theStdActions)
{
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

  bool isActiveSketch = PartSet_SketcherMgr::isSketchOperation(anOperation) ||
                        myModule->sketchMgr()->isNestedSketchOperation(anOperation);
  if (isActiveSketch) {
    theStdActions["WIREFRAME_CMD"]->setEnabled(false);
    theStdActions["SHADING_CMD"]->setEnabled(false);
    theStdActions["SHOW_ONLY_CMD"]->setEnabled(false);
    theStdActions["SHOW_CMD"]->setEnabled(false);
    theStdActions["HIDE_CMD"]->setEnabled(false);
    theStdActions["HIDEALL_CMD"]->setEnabled(false);
  }
}


void PartSet_MenuMgr::onLineHighlighted(QAction* theAction)
{
  if (myPrevId != -1) {
    // Restore color for previous object
    setLineColor(myPrevId, myColor, false);
  }
  myPrevId = theAction->data().toInt();
  myColor = setLineColor(myPrevId, Qt::white, true);
}

QColor PartSet_MenuMgr::setLineColor(int theId, const QColor theColor, bool theUpdate)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();

  FeaturePtr aLine = myCoinsideLines.at(myPrevId);
  std::list<ResultPtr>::const_iterator aIt;
  const std::list<ResultPtr>& aResults = aLine->results();
  QColor aColor;
  for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
    aColor = aDisplayer->setObjectColor((*aIt), theColor, false);
  }
  if (theUpdate)
    aDisplayer->updateViewer();
  return aColor;
}


void addRefCoincidentFeatures(const std::set<AttributePtr>& theRefList,
  std::shared_ptr<GeomAPI_Pnt2d>& theRefPnt,
  QObjectPtrList& theOutList)
{
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = theRefList.cbegin(); aIt != theRefList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      std::shared_ptr<GeomAPI_Pnt2d> aPnt = PartSet_Tools::getCoincedencePoint(aConstrFeature);
      if (aPnt.get() == NULL)
        return;
      gp_Pnt aP = aPnt->impl<gp_Pnt>();
      if (theRefPnt->isEqual(aPnt) && (!theOutList.contains(aConstrFeature))) {
        theOutList.append(aConstrFeature);
      }
    }
  }
}

void PartSet_MenuMgr::onLineDetach(QAction* theAction)
{
  int aId = theAction->data().toInt();
  FeaturePtr aLine = myCoinsideLines.at(aId);
  std::shared_ptr<GeomAPI_Pnt2d> aOrig = PartSet_Tools::getCoincedencePoint(mySelectedFeature);
  if (!aOrig.get())
    return;

  const std::set<AttributePtr>& aRefsList = aLine->data()->refsToMe();

  QObjectPtrList aToDelFeatures;

  addRefCoincidentFeatures(aRefsList, aOrig, aToDelFeatures);

  const std::list<ResultPtr>& aResults = aLine->results();
  std::list<ResultPtr>::const_iterator aResIt;
  for (aResIt = aResults.cbegin(); aResIt != aResults.cend(); aResIt++) {
    ResultPtr aResult = (*aResIt);
    const std::set<AttributePtr>& aRefList = aResult->data()->refsToMe();
    addRefCoincidentFeatures(aRefList, aOrig, aToDelFeatures);
  }
  if (aToDelFeatures.size() > 0) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

    ModuleBase_Operation* anOpAction =
      new ModuleBase_Operation(tr("Detach %1").arg(aLine->data()->name().c_str()), myModule);
    bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation);
    XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
    // the active nested sketch operation should be aborted unconditionally
    // the Delete action should be additionally granted for the Sketch operation
    // in order to do not abort/commit it
    bool isCommitted;
    if (!anOpMgr->canStartOperation(tr("Detach"), isCommitted))
      return; // the objects are processed but can not be deleted

    anOpMgr->startOperation(anOpAction);
    aWorkshop->deleteFeatures(aToDelFeatures);

    anOpMgr->commitOperation();
  }
  myCoinsideLines.clear();
}


void PartSet_MenuMgr::onDetachMenuHide()
{
  if (myPrevId != -1) {
    // Restore color for previous object
    setLineColor(myPrevId, myColor, false);
  }
  // Clear previous definitions
  myPrevId = -1;
}


void PartSet_MenuMgr::setAuxiliary(const bool isChecked)
{
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

  CompositeFeaturePtr aSketch = myModule->sketchMgr()->activeSketch();
  bool isActiveSketch = PartSet_SketcherMgr::isSketchOperation(anOperation) ||
                        myModule->sketchMgr()->isNestedSketchOperation(anOperation);
  if (!isActiveSketch)
    return;

  QObjectPtrList anObjects;
  bool isUseTransaction = false;
  // 1. change auxiliary type of a created feature
  if (myModule->sketchMgr()->isNestedCreateOperation(anOperation, aSketch) &&
      PartSet_SketcherMgr::isEntity(anOperation->id().toStdString()) ) {
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (anOperation);
      if (aFOperation)
        anObjects.append(aFOperation->feature());
  }
  else {
    isUseTransaction = true;
    // 2. change auxiliary type of selected sketch entities
    ModuleBase_ISelection* aSelection = myModule->workshop()->selection();
    anObjects = aSelection->selectedPresentations();
  }

  QAction* anAction = action("AUXILIARY_CMD");
  //SessionPtr aMgr = ModelAPI_Session::get();
  ModuleBase_Operation* anOpAction = 0;
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  if (isUseTransaction) {
    anOpAction = new ModuleBase_Operation(anAction->text(), myModule);
    bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation);

    bool isCommitted;
    if (!anOpMgr->canStartOperation(anOpAction->id(), isCommitted))
      return; // the objects are processed but can not be deleted

    anOpMgr->startOperation(anOpAction);
  }
  static const Events_ID anVisualEvent = Events_Loop::eventByName(EVENT_VISUAL_ATTRIBUTES);
  if (anObjects.size() > 0) {
    QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
      if (aFeature.get() != NULL) {
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature.get() != NULL) {
          std::string anAttribute = SketchPlugin_SketchEntity::AUXILIARY_ID();

          std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr =
            std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
            aSketchFeature->data()->attribute(anAttribute));
          if (anAuxiliaryAttr)
            anAuxiliaryAttr->setValue(isChecked);
          ModelAPI_EventCreator::get()->sendUpdated(aSketchFeature, anVisualEvent);
        }
      }
    }
  }
  if (isUseTransaction)
    anOpMgr->commitOperation();

  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->flush(anVisualEvent);
}

bool PartSet_MenuMgr::canSetAuxiliary(bool& theValue) const
{
  bool anEnabled = false;
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

  CompositeFeaturePtr aSketch = myModule->sketchMgr()->activeSketch();
  bool isActiveSketch = PartSet_SketcherMgr::isSketchOperation(anOperation) ||
                        myModule->sketchMgr()->isNestedSketchOperation(anOperation);
  if (!isActiveSketch)
    return anEnabled;

  QObjectPtrList anObjects;
  // 1. change auxiliary type of a created feature
  if (myModule->sketchMgr()->isNestedCreateOperation(anOperation, aSketch) &&
    PartSet_SketcherMgr::isEntity(anOperation->id().toStdString()) ) {
    ModuleBase_OperationFeature* aFOperation =
      dynamic_cast<ModuleBase_OperationFeature*>(anOperation);
    if (aFOperation)
      anObjects.append(aFOperation->feature());
  }
  else {
    /// The operation should not be aborted here, because the method does not changed
    /// the auxilliary state, but checks the possibility to perform this
    ///if (myModule->sketchMgr()->isNestedSketchOperation(anOperation))
    ///  anOperation->abort();
    // 2. change auxiliary type of selected sketch entities
    ModuleBase_ISelection* aSelection = myModule->workshop()->selection();
    anObjects = aSelection->selectedPresentations();
  }

  bool isNotAuxiliaryFound = false;
  if (anObjects.size() > 0) {
    QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
    for (; anIt != aLast && !isNotAuxiliaryFound; anIt++) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
      if ((aFeature.get() != NULL) && PartSet_SketcherMgr::isEntity(aFeature->getKind())) {
        anEnabled = true;
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature.get() != NULL) {
          std::string anAttribute = SketchPlugin_SketchEntity::AUXILIARY_ID();

          std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr =
            std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
            aSketchFeature->data()->attribute(anAttribute));
          if (anAuxiliaryAttr)
            isNotAuxiliaryFound = !anAuxiliaryAttr->value();
        }
      }
    }
  }
  theValue = anObjects.size() && !isNotAuxiliaryFound;
  return anEnabled;
}

void PartSet_MenuMgr::onActivatePart(bool)
{
  if (myModule->workshop()->currentOperation())
    return;
  QObjectPtrList aObjects = myModule->workshop()->selection()->selectedObjects();
  if (aObjects.size() > 0) {
    ObjectPtr aObj = aObjects.first();
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (!aPart.get()) {
      FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
      if (aPartFeature.get() && (aPartFeature->getKind() == PartSetPlugin_Part::ID())) {
        aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult());
      }
    }
    if (aPart.get()) {
      activatePart(aPart);
      myModule->workshop()->updateCommandStatus();
    }
  }
}

void PartSet_MenuMgr::activatePart(ResultPartPtr thePart) const
{
  bool isFirstLoad = !thePart->partDoc().get();
  thePart->activate();
  if (isFirstLoad) {
    XGUI_Workshop* aWorkshop = myModule->getWorkshop();
    XGUI_ObjectsBrowser* aObjBrowser = aWorkshop->objectBrowser();
    DocumentPtr aDoc = thePart->partDoc();
    std::list<bool> aStates;
    aDoc->restoreNodesState(aStates);
    aObjBrowser->setStateForDoc(aDoc, aStates);
  }
}

void PartSet_MenuMgr::onActivateAllParts()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isOperation())
    return;

  DocumentPtr aDoc = aMgr->moduleDocument();
  int aNbParts = aDoc->size(ModelAPI_ResultPart::group());
  bool isActivated = false;
  QList<ResultPartPtr> aPartsToLoad;
  for (int i = 0; i < aNbParts; i++) {
    ObjectPtr aObj = aDoc->object(ModelAPI_ResultPart::group(), i);
    ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (!aPartRes->partDoc().get())
      aPartsToLoad.append(aPartRes);
  }
  if (!aPartsToLoad.isEmpty()) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    aMgr->startOperation("All Parts loading");
    foreach(ResultPartPtr aPartRes, aPartsToLoad) {
      aPartRes->loadPart();
    }
    aMgr->finishOperation();

    XGUI_Workshop* aWorkshop = myModule->getWorkshop();
    XGUI_ObjectsBrowser* aObjBrowser = aWorkshop->objectBrowser();
    aObjBrowser->update();
    aWorkshop->viewer()->update();
    aWorkshop->updateCommandStatus();
    QApplication::restoreOverrideCursor();
  }
}

void PartSet_MenuMgr::onActivatePartSet(bool)
{
  if (myModule->workshop()->currentOperation())
    return;
  activatePartSet();
}

void PartSet_MenuMgr::activatePartSet() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isNewTransaction = !aMgr->isOperation();
  // activation may cause changes in current features in document, so it must be in transaction
  if (isNewTransaction)
    aMgr->startOperation("Activation");
  aMgr->setActiveDocument(aMgr->moduleDocument());
  if (isNewTransaction)
    aMgr->finishOperation();

  myModule->workshop()->updateCommandStatus();
  myModule->workshop()->viewer()->update();
}

void PartSet_MenuMgr::grantedOperationIds(ModuleBase_Operation* theOperation,
                                          QStringList& theIds) const
{
  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    theIds.append(tr("Detach"));
    theIds.append(tr("Auxiliary"));
  }
}

void PartSet_MenuMgr::onEdit(bool)
{
  QObjectPtrList aObjects = myModule->workshop()->selection()->selectedObjects();
  FeaturePtr aFeature;
  foreach(ObjectPtr aObj, aObjects) {
    aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aFeature.get())
      break;
  }
  if (aFeature.get() == NULL) {
    ResultParameterPtr aParam;
    foreach(ObjectPtr aObj, aObjects) {
      aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aObj);
      if (aParam.get())
        break;
    }
    if (aParam.get() != NULL)
      aFeature = ModelAPI_Feature::feature(aParam);
  }
  if (aFeature.get() != NULL)
    myModule->editFeature(aFeature);
}

bool PartSet_MenuMgr::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theEvent->type() == QEvent::MouseButtonDblClick) {
    SessionPtr aMgr = ModelAPI_Session::get();
    if (aMgr->activeDocument() != aMgr->moduleDocument())
      activatePartSet();
  }
  return QObject::eventFilter(theObj, theEvent);
}