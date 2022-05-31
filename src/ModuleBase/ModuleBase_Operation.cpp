// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QTimer>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_Operation::ModuleBase_Operation(const QString& theId, QObject* theParent)
    : QObject(theParent),
      myIsModified(false),
      myPropertyPanel(NULL),
  myHideFacesVisibilityState(false)
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
    ModuleBase_IPropertyPanel* aPanel = propertyPanel();
    if (aPanel)
      aPanel->onAcceptData();

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

void ModuleBase_Operation::onValueStateChanged(int /*thePreviousState*/)
{
  if (propertyPanel()) {
    ModuleBase_ModelWidget* aWidget = propertyPanel()->activeWidget();
    if (aWidget) {
      if (aWidget->getValueState() == ModuleBase_ModelWidget::ModifiedInPP)
        myIsModified = true;
    }
  }
}

void ModuleBase_Operation::setPropertyPanel(ModuleBase_IPropertyPanel* theProp)
{
  myPropertyPanel = theProp;
}

bool ModuleBase_Operation::isGranted(QString theId) const
{
  return myGrantedIds.contains(theId);
}

bool  ModuleBase_Operation::isModified() const
{
  if (myDescription->hasXmlRepresentation()) {
    Config_WidgetAPI aWidgetApi(myDescription->xmlRepresentation().toStdString());
    if (!aWidgetApi.getBooleanAttribute(ABORT_CONFIRMATION, true))
      return false;
  }
  //if (myPropertyPanel)
  //  return myPropertyPanel->isModified();
  //return false;
  // Most of operation causes creation of a feature
  return true;
}
