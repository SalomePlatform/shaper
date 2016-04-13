// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_MenuMgr.cpp
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#include <XGUI_MenuMgr.h>
#include <XGUI_Workshop.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_OperationMgr.h>

#include <Events_Loop.h>
#include <Config_FeatureMessage.h>
#include <Config_Keywords.h>

#ifndef HAVE_SALOME
#include <AppElements_Workbench.h>
#include <AppElements_Command.h>
#include <AppElements_MainMenu.h>
#include <AppElements_MainWindow.h>
#include <AppElements_MenuGroupPanel.h>
#include <AppElements_Button.h>
#else
#include <XGUI_SalomeConnector.h>
#endif

#include <ModuleBase_IModule.h>

#include <QObject>
#include <QAction>
#include <QDebug>

XGUI_MenuMgr::XGUI_MenuMgr(XGUI_Workshop* theWorkshop)
: myWorkshop(theWorkshop)
{
  Events_Loop* aLoop = Events_Loop::loop();

  aLoop->registerListener(this, Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
}

void XGUI_MenuMgr::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  //A message to start feature creation received.
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(Config_FeatureMessage::GUI_EVENT())) {
    std::shared_ptr<Config_FeatureMessage> aFeatureMsg =
       std::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (!aFeatureMsg->isInternal()) {
      addFeature(aFeatureMsg);
    }
  }
}

void XGUI_MenuMgr::addFeature(const std::shared_ptr<Config_FeatureMessage>& theMessage)
{
  if (!theMessage) {
#ifdef _DEBUG
    qDebug() << "XGUI_WorkshopListener::addFeature: NULL message.";
#endif
    return;
  }

  ActionInfo aFeatureInfo;
  aFeatureInfo.initFrom(theMessage);

  QString aWchName = QString::fromStdString(theMessage->workbenchId());
  QStringList aNestedFeatures =
      QString::fromStdString(theMessage->nestedFeatures()).split(" ", QString::SkipEmptyParts);
  QList<QAction*> aNestedActList;
  bool isColumnButton = !aNestedFeatures.isEmpty();
  if (isColumnButton) {
    QString aNestedActions = QString::fromStdString(theMessage->actionsWhenNested());
    XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();
    XGUI_ActionsMgr* anActionsMgr = myWorkshop->actionsMgr();
    if (aNestedActions.contains(FEATURE_WHEN_NESTED_ACCEPT)) {
      QAction* anAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll, NULL);
      QObject::connect(anAction, SIGNAL(triggered()), anOperationMgr, SLOT(commitAllOperations()));
      aNestedActList << anAction;
    }
    if (aNestedActions.contains(FEATURE_WHEN_NESTED_ABORT)) {
      QAction* anAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AbortAll, NULL);
      QObject::connect(anAction, SIGNAL(triggered()), anOperationMgr, SLOT(abortAllOperations()));
      aNestedActList << anAction;
    }
  }

#ifdef HAVE_SALOME
  XGUI_SalomeConnector* aSalomeConnector = myWorkshop->salomeConnector();
  QAction* aAction;
  if (isColumnButton) {
    aAction = aSalomeConnector->addFeatureOfNested(aWchName, aFeatureInfo, aNestedActList);
  } else {
    //Issue #650: in the SALOME mode the tooltip should be same as text
    aFeatureInfo.toolTip = aFeatureInfo.text;
    aAction = aSalomeConnector->addFeature(aWchName, aFeatureInfo);
  }
  aSalomeConnector->setFeatureInfo(aFeatureInfo.id, theMessage);

  myWorkshop->actionsMgr()->addCommand(aAction);
  myWorkshop->module()->actionCreated(aAction);
#else 
  //Find or create Workbench
  AppElements_MainMenu* aMenuBar = myWorkshop->mainWindow()->menuObject();
  AppElements_Workbench* aPage = aMenuBar->findWorkbench(aWchName);
  if (!aPage) {
    aPage = myWorkshop->addWorkbench(aWchName);
  }
  //Find or create Group
  QString aGroupName = QString::fromStdString(theMessage->groupId());
  AppElements_MenuGroupPanel* aGroup = aPage->findGroup(aGroupName);
  if (!aGroup) {
    aGroup = aPage->addGroup(aGroupName);
  }
  // Check if hotkey sequence is already defined:
  XGUI_ActionsMgr* anActionsMgr = myWorkshop->actionsMgr();
  QKeySequence aHotKey = anActionsMgr->registerShortcut(aFeatureInfo.shortcut);
  if(aHotKey != aFeatureInfo.shortcut) {
    aFeatureInfo.shortcut = aHotKey;
  }
  AppElements_Command* aCommand = aGroup->addFeature(theMessage);
  // Enrich created button with accept/abort buttons if necessary
  AppElements_Button* aButton = aCommand->button();
  if (aButton->isColumnButton()) {
    aButton->setAdditionalButtons(aNestedActList);
  }
  myWorkshop->actionsMgr()->addCommand(aCommand);
  myWorkshop->module()->actionCreated(aCommand);
#endif
}
