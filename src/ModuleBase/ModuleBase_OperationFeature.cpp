// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_OperationFeature.h"

#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Pnt2d.h>

#include <Events_Loop.h>

#include <QTimer>

// the define to check the activated object as a sub-feature by argument of
// the operation feature. E.g. rectangle feature(operation), line(in argument) to be not activated
#define DEBUG_DO_NOT_ACTIVATE_SUB_FEATURE
#ifdef DEBUG_DO_NOT_ACTIVATE_SUB_FEATURE
#include <ModelAPI_AttributeRefList.h>
#endif

//#define DEBUG_OPERATION_START

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_OperationFeature::ModuleBase_OperationFeature(const QString& theId, QObject* theParent)
: ModuleBase_Operation(theId, theParent), myIsEditing(false), myNeedToBeAborted(false)
{
}

ModuleBase_OperationFeature::~ModuleBase_OperationFeature()
{
  clearPreselection();
}

void ModuleBase_OperationFeature::setEditOperation(const bool& isEditState
                                                   /*const bool theRestartTransaction*/)
{
  bool isCurrentEditState = isEditOperation();
  if (isCurrentEditState == isEditState)
    return;

  /*
  // this case is obsolete as it was not approved for reentrant sketch operation
  // it was implemented when isEditState did not exist and only edit operation can be set
  if (theRestartTransaction) {
    // finsh previous create operation
    emit beforeCommitted();
    SessionPtr aMgr = ModelAPI_Session::get();
    ModelAPI_Session::get()->finishOperation();

    // start new edit operation
    myIsEditing = true;
    QString anId = getDescription()->operationId();
    if (myIsEditing) {
      anId = anId.append(EditSuffix());
    }
    ModelAPI_Session::get()->startOperation(anId.toStdString());
    emit beforeStarted();
  } else*/
  myIsEditing = isEditState;

  propertyPanel()->setEditingMode(isEditOperation());
}

FeaturePtr ModuleBase_OperationFeature::feature() const
{
  return myFeature;
}

bool ModuleBase_OperationFeature::isValid() const
{
  if (!myFeature || !myFeature->data()->isValid())
    return true; // rename operation
  if (myFeature->isAction())
    return true;

  std::string anError = ModelAPI_Tools::getFeatureError(myFeature);
  //ModuleBase_Tools::translate(myFeature->getKind(), anError);
  return anError.empty();
}

void ModuleBase_OperationFeature::startOperation()
{
  FeaturePtr aFeature = feature();
  if (!aFeature.get() || !isEditOperation())
    return;

  if (aFeature.get() && isEditOperation())
    aFeature->setStable(false);

  myVisualizedObjects.clear();
  // store hidden result features
  std::list<ResultPtr> aResults;
  ModelAPI_Tools::allResults(aFeature, aResults);
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    ObjectPtr anObject = *aIt;
    if (anObject.get() && !anObject->isDisplayed()) {
      myVisualizedObjects.insert(*aIt);
      anObject->setDisplayed(true);
    }
  }
  if (!aFeature->isDisplayed()) {
    myVisualizedObjects.insert(aFeature);
    aFeature->setDisplayed(true);
  }
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

void ModuleBase_OperationFeature::stopOperation()
{
  FeaturePtr aFeature = feature();
  if (!aFeature.get() || !isEditOperation())
    return;

  // store hidden result features
  std::list<ResultPtr> aResults;
  ModelAPI_Tools::allResults(aFeature, aResults);
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    ObjectPtr anObject = *aIt;
    if (anObject.get() && myVisualizedObjects.find(anObject) != myVisualizedObjects.end()) {
      anObject->setDisplayed(false);
    }
  }
  if (myVisualizedObjects.find(aFeature) != myVisualizedObjects.end()) {
    aFeature->setDisplayed(false);
  }
  if (myVisualizedObjects.size() > 0)
    Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

FeaturePtr ModuleBase_OperationFeature::createFeature(const bool theFlushMessage)
{
  if (myParentFeature.get()) {
    myFeature = myParentFeature->addFeature(getDescription()->operationId().toStdString());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_Session::get()->activeDocument();
    myFeature = aDoc->addFeature(getDescription()->operationId().toStdString());
  }
  if (myFeature) {  // TODO: generate an error if feature was not created
    setIsModified(true);
    // Model update should call "execute" of a feature.
    //myFeature->execute();
    // Init default values
    /*QList<ModuleBase_ModelWidget*> aWidgets = getDescription()->modelWidgets();
     QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
     for (; anIt != aLast; anIt++) {
     (*anIt)->storeValue(aFeature);
     }*/
  }

  if (theFlushMessage) {
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
  return myFeature;
}

void ModuleBase_OperationFeature::setFeature(FeaturePtr theFeature)
{
  myFeature = theFeature;
  myIsEditing = true;
}

bool ModuleBase_OperationFeature::hasObject(ObjectPtr theObj) const
{
  FeaturePtr aFeature = feature();
  if (aFeature) {
    if (aFeature == theObj)
      return true;
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
      ResultPtr aResult = *aIt;
      if (theObj == aResult)
         return true;
    }
#ifdef DEBUG_DO_NOT_ACTIVATE_SUB_FEATURE
    if (aFeature->isMacro()) {
      // macro feature may refers to sub-features,
      // which also should be deactivated when the operation
      // is active, e.g. rectangle'lines.
      FeaturePtr anObjectFeature = ModelAPI_Feature::feature(theObj);
      std::list<AttributePtr> anAttributes = aFeature->data()->attributes(
                                              ModelAPI_AttributeRefList::typeId());
      std::list<AttributePtr>::const_iterator
        anIt = anAttributes.begin(), aLast = anAttributes.end();
      bool aFoundObject = false;
      for (; anIt != aLast && !aFoundObject; anIt++) {
        std::shared_ptr<ModelAPI_AttributeRefList> aCurSelList =
                               std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIt);
        for (int i = 0, aNb = aCurSelList->size(); i < aNb && !aFoundObject; i++) {
          ObjectPtr anObject = aCurSelList->object(i);
          FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
          if (aFeature.get()) {
            aFoundObject = anObjectFeature == aFeature;
          }
        }
      }
      return aFoundObject;
    }
#endif
  }
  return false;
}

bool ModuleBase_OperationFeature::isDisplayedOnStart(ObjectPtr theObject)
{
  return myVisualizedObjects.find(theObject) != myVisualizedObjects.end();
}

bool ModuleBase_OperationFeature::start()
{
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::start -- begin");
#endif
  setIsModified(false);
  QString anId = getDescription()->operationId();
  if (myIsEditing) {
    anId = anId.append(EditSuffix());
  }
  ModelAPI_Session::get()->startOperation(anId.toStdString());

  emit beforeStarted();
  startOperation();

  if (!myIsEditing) {
    FeaturePtr aFeature = createFeature();
    // if the feature is not created, there is no sense to start the operation
    if (aFeature.get() == NULL) {
      // it is necessary to abor the operation in the session and emit the aborted signal
      // in order to update commands status in the workshop, to be exact the feature action
      // to be unchecked
      abort();
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::start -- end : IMPOSSIBLE to start");
#endif
      return false;
    }
  }
  //Already called startOperation();
  emit started();
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::start -- end");
#endif
  return true;
}

void ModuleBase_OperationFeature::abort()
{
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::abort -- begin");
#endif

  emit beforeAborted();

  // the viewer update should be blocked in order to avoid the features blinking before they are
  // hidden
  std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
      new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  Events_Loop::loop()->send(aMsg);

  // the widgets of property panel should not process any events come from data mode
  // after abort clicked. Some signal such as redisplay/create influence on content
  // of the object browser and viewer context. Therefore it influence to the current
  // selection and if the active widget listens it, the attribute value is errnoneous
  // changed.
  ModuleBase_IPropertyPanel* aPropertyPanel = propertyPanel();
  if (aPropertyPanel)
    aPropertyPanel->cleanContent();

  if (myFeature.get())
    myFeature->setStable(true);

  abortOperation();
  stopOperation();

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->abortOperation();
  emit stopped();
  // the viewer update should be unblocked in order to avoid the features blinking before they are
  // hidden
  aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));

  Events_Loop::loop()->send(aMsg);

  emit aborted();
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::abort -- end");
#endif
}

bool ModuleBase_OperationFeature::commit()
{
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::commit -- begin");
#endif
  ModuleBase_IPropertyPanel* aPanel = propertyPanel();
  if (aPanel) {
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    if (anActiveWidget && anActiveWidget->getValueState() == ModuleBase_ModelWidget::ModifiedInPP) {
      anActiveWidget->storeValue();
    }
  }
  if (canBeCommitted()) {
    emit beforeCommitted();
    // the widgets of property panel should not process any events come from data mode
    // after commit clicked. Some signal such as redisplay/create influence on content
    // of the object browser and viewer context. Therefore it influence to the current
    // selection and if the active widget listens it, the attribute value is errnoneous
    // changed.
    ModuleBase_IPropertyPanel* aPropertyPanel = propertyPanel();
    if (aPropertyPanel)
      aPropertyPanel->cleanContent();

    myFeature->setStable(true);

    SessionPtr aMgr = ModelAPI_Session::get();
    /// Set current feature and remeber old current feature

    commitOperation();
    aMgr->finishOperation();

    stopOperation();
    emit stopped();
    emit committed();

    afterCommitOperation();
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::commit -- end");
#endif
    return true;
  }
#ifdef DEBUG_OPERATION_START
  qDebug("ModuleBase_OperationFeature::commit -- end : IMPOSSIBLE to commit");
#endif
  return false;
}

ModuleBase_ModelWidget* ModuleBase_OperationFeature::activateByPreselection(
                                              const std::string& theGreedAttributeId)
{
  ModuleBase_ModelWidget* aWidget = 0;
  if (myPreSelection.empty())
    return aWidget;

  ModuleBase_IPropertyPanel* aPropertyPanel = propertyPanel();
  ModuleBase_ModelWidget* aFilledWgt = 0;
  if (aPropertyPanel) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPropertyPanel->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
    ModuleBase_ModelWidget* aWgt = 0;
    if (!aWidgets.empty()) {
      // equal vertices should not be used here
      ModuleBase_ISelection::filterSelectionOnEqualPoints(myPreSelection);

      if (!theGreedAttributeId.empty()) {
        // set preselection to greed widget
        for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
          aWgt = (*aWIt);
          if (aWgt->attributeID() == theGreedAttributeId) {
            aPropertyPanel->setPreselectionWidget(aWgt);
            if (aWgt->setSelection(myPreSelection, true)) {
              aPropertyPanel->setPreselectionWidget(NULL);
              aFilledWgt = aWgt;
              break;
            }
            else { // do not process invalid for greed widget selection
              break;
            }
          }
        }
      }
      else {
        bool isSet = false;
        // 1. apply the selection to controls
        for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
          aWgt = (*aWIt);
          if (!aWgt->canAcceptFocus())
            continue;
          aPropertyPanel->setPreselectionWidget(aWgt);
          if (myPreSelection.empty() || !aWgt->setSelection(myPreSelection, true)) {
            isSet = false;
            break;
          } else {
            isSet = true;
            aFilledWgt = aWgt;
          }
        }
      }
      aPropertyPanel->setPreselectionWidget(NULL);
      // in order to redisplay object in the viewer, the update/redisplay signals should be flushed
      // it is better to perform it not in setSelection of each widget, but do it here,
      // after the preselection is processed
      ModuleBase_Tools::flushUpdated(myFeature);
    }
  }
  clearPreselection();

  return aFilledWgt;
}

void ModuleBase_OperationFeature::setParentFeature(CompositeFeaturePtr theParent)
{
  myParentFeature = theParent;
}

CompositeFeaturePtr ModuleBase_OperationFeature::parentFeature() const
{
  return myParentFeature;
}

void ModuleBase_OperationFeature::setPreviousCurrentFeature(const FeaturePtr& theFeature)
{
  myPreviousCurrentFeature = theFeature;
}

FeaturePtr ModuleBase_OperationFeature::previousCurrentFeature()
{
  return myPreviousCurrentFeature;
}

void ModuleBase_OperationFeature::initSelection(
  const QList<ModuleBase_ViewerPrsPtr>& thePreSelected)
{
  QObjectPtrList aCurrentFeatureResults;

  // Check that the selected result are not results of operation feature
  FeaturePtr aFeature = feature();
  if (aFeature) {
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt)
      aCurrentFeatureResults.append(*aIt);
  }

  if (aCurrentFeatureResults.empty()) /// filtering of selection is not necessary
    setPreselection(thePreSelected);
  else { // create preselection list without results of current feature
    QList<ModuleBase_ViewerPrsPtr> aPreSelected;
    foreach (ModuleBase_ViewerPrsPtr aPrs, thePreSelected) {
      if ((!aCurrentFeatureResults.contains(aPrs->object())) && (aPrs->object() != aFeature))
        aPreSelected.append(aPrs);
    }
    setPreselection(aPreSelected);
  }
}

void ModuleBase_OperationFeature::setPreselection(const QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  clearPreselection();
  myPreSelection = theValues;
}

void ModuleBase_OperationFeature::clearPreselection()
{
  myPreSelection.clear();
}

void ModuleBase_OperationFeature::setPropertyPanel(ModuleBase_IPropertyPanel* theProp)
{
  ModuleBase_Operation::setPropertyPanel(theProp);

  theProp->setEditingMode(isEditOperation());

  if (theProp) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = theProp->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
    for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
      ModuleBase_ModelWidget* aWgt = (*aWIt);
      connect(aWgt, SIGNAL(valuesChanged()), this, SLOT(onValuesChanged()));
      connect(aWgt, SIGNAL(valueStateChanged(int)), this, SLOT(onValueStateChanged(int)));
    }
  }

  // Do not activate widgets by default if the current operation is editing operation
  // Because we don't know which widget is going to be edited.
  if (!isEditOperation()) {
    // 4. activate the first obligatory widget
    theProp->activateNextWidget(NULL);
  }
  else {
    // set focus on Ok button in order to operation manager could process Enter press
    if (theProp)
      theProp->setFocusOnOkButton();
  }
}
