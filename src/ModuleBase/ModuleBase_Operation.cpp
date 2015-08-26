// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_Operation.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include "ModuleBase_Operation.h"

#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_IViewer.h"

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

#include <GeomAPI_Pnt2d.h>

#include <Events_Loop.h>

#include <QTimer>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_Operation::ModuleBase_Operation(const QString& theId, QObject* theParent)
    : QObject(theParent),
      myIsModified(false),
      myPropertyPanel(NULL)
{
  myDescription = new ModuleBase_OperationDescription(theId);
}

ModuleBase_Operation::~ModuleBase_Operation()
{
  delete myDescription;
}

QString ModuleBase_Operation::id() const
{
  return getDescription()->operationId();
}

bool ModuleBase_Operation::isValid() const
{
  return true;
}

bool ModuleBase_Operation::canBeCommitted() const
{
  return isValid();
}


void ModuleBase_Operation::start()
{
  myIsModified = false;
  /*
  QString anId = getDescription()->operationId();
  if (myIsEditing) {
    anId = anId.append(EditSuffix());
  }
  ModelAPI_Session::get()->startOperation(anId.toStdString());

  if (!myIsEditing) {
    FeaturePtr aFeature = createFeature();
    // if the feature is not created, there is no sense to start the operation
    if (aFeature.get() == NULL) {
      // it is necessary to abor the operation in the session and emit the aborted signal
      // in order to update commands status in the workshop, to be exact the feature action
      // to be unchecked
      abort();
      return;
    }
  }
  /// Set current feature and remeber old current feature
  if (myIsEditing) {
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aDoc = aMgr->activeDocument();
    myCurrentFeature = aDoc->currentFeature(true);
    aDoc->setCurrentFeature(feature(), false);
  }

  startOperation();
  emit started();
*/
}

void ModuleBase_Operation::postpone()
{
  postponeOperation();
  emit postponed();
}

void ModuleBase_Operation::resume()
{
  resumeOperation();
  emit resumed();
}

void ModuleBase_Operation::abort()
{
/*
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
  if (myPropertyPanel)
    myPropertyPanel->cleanContent();

  SessionPtr aMgr = ModelAPI_Session::get();
  if (myIsEditing) {
    DocumentPtr aDoc = aMgr->activeDocument();
    bool aIsOp = aMgr->isOperation();
    if (!aIsOp)
      aMgr->startOperation();
    aDoc->setCurrentFeature(myCurrentFeature, true);
    if (!aIsOp)
      aMgr->finishOperation();
    myCurrentFeature = FeaturePtr();
  }
  abortOperation();

  stopOperation();
  // is is necessary to deactivate current widgets before the model operation is aborted
  // because abort removes the feature and activated filters should not check it
  propertyPanel()->cleanContent();

  aMgr->abortOperation();
  emit stopped();
  // the viewer update should be unblocked in order to avoid the features blinking before they are
  // hidden
  aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));

  Events_Loop::loop()->send(aMsg);

  emit aborted();
*/
}

bool ModuleBase_Operation::commit()
{
/*  if (canBeCommitted()) {
    // the widgets of property panel should not process any events come from data mode
    // after commit clicked. Some signal such as redisplay/create influence on content
    // of the object browser and viewer context. Therefore it influence to the current
    // selection and if the active widget listens it, the attribute value is errnoneous
    // changed.
    if (myPropertyPanel)
      myPropertyPanel->cleanContent();

    SessionPtr aMgr = ModelAPI_Session::get();
    /// Set current feature and remeber old current feature
    if (myIsEditing) {
      DocumentPtr aDoc = aMgr->activeDocument();
      bool aIsOp = aMgr->isOperation();
      if (!aIsOp)
        aMgr->startOperation();
      aDoc->setCurrentFeature(myCurrentFeature, true);
      if (!aIsOp)
        aMgr->finishOperation();
      myCurrentFeature = FeaturePtr();
    }
    commitOperation();
    aMgr->finishOperation();

    stopOperation();
    emit stopped();
    emit committed();

    afterCommitOperation();
    return true;
  }
*/
  return false;
}

void ModuleBase_Operation::onValuesChanged()
{
  myIsModified = true;
}

void ModuleBase_Operation::setPropertyPanel(ModuleBase_IPropertyPanel* theProp) 
{ 
  myPropertyPanel = theProp; 
}

bool ModuleBase_Operation::isGranted(QString theId) const
{
  return false;
}
