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

const QStringList& ModuleBase_Operation::grantedOperationIds() const
{
  return myGrantedIds;
}

void ModuleBase_Operation::setGrantedOperationIds(const QStringList& theList)
{
  myGrantedIds = theList;
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

bool ModuleBase_Operation::start()
{
  myIsModified = false;

  ModelAPI_Session::get()->startOperation(id().toStdString());

  startOperation();
  emit started();

  return true;
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
  // the viewer update should be blocked in order to avoid the features blinking before they are
  // hidden
  //std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
  //    new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  //Events_Loop::loop()->send(aMsg);

  ModelAPI_Session::get()->abortOperation();

  emit stopped();
  // the viewer update should be unblocked in order to avoid the features blinking before they are
  // hidden
  //aMsg = std::shared_ptr<Events_Message>(
  //              new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));
  //Events_Loop::loop()->send(aMsg);

  emit aborted();
}

bool ModuleBase_Operation::commit()
{
  if (canBeCommitted()) {
    SessionPtr aMgr = ModelAPI_Session::get();

    commitOperation();
    aMgr->finishOperation();

    stopOperation();
    emit stopped();
    emit committed();

    afterCommitOperation();
    return true;
  }
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
  return myGrantedIds.contains(theId);
}
