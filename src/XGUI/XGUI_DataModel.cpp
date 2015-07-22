// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_IDocumentDataModel.cpp
// Created:     28 Apr 2015
// Author:      Vitaly SMETANNIKOV

#include "XGUI_DataModel.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>

#include <Config_FeatureMessage.h>

#include <Events_Loop.h>

#include <QIcon>

XGUI_DataModel::XGUI_DataModel(QObject* theParent) : QAbstractItemModel(theParent)
{
  myXMLReader.readAll();

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->registerListener(this, Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
}

//******************************************************
void XGUI_DataModel::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  std::string aRootType = myXMLReader.rootType();
  int aNbFolders = myXMLReader.rootFoldersNumber();

  // Created object event *******************
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    std::set<ObjectPtr>::const_iterator aIt;
    std::string aObjType;
    for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
      ObjectPtr aObject = (*aIt);
      aObjType = aObject->groupName();
      DocumentPtr aDoc = aObject->document();
      if (aDoc == aRootDoc) {
        int aRow = aRootDoc->size(aObjType) - 1;
        if (aRow != -1) {
          if (aObjType == aRootType) {
            insertRow(aRow + aNbFolders);
          } else {
            int aFolderId = myXMLReader.rootFolderId(aObjType);
            if (aFolderId != -1) {
              insertRow(aRow, createIndex(aFolderId, 0, -1));
            }
          } 
        }
      }
    }
  }
}

//******************************************************
void XGUI_DataModel::clear()
{

}

//******************************************************
void XGUI_DataModel::rebuildDataTree()
{

}

//******************************************************
ObjectPtr XGUI_DataModel::object(const QModelIndex& theIndex) const
{
  return ObjectPtr();
}

//******************************************************
QModelIndex XGUI_DataModel::objectIndex(const ObjectPtr theObject) const
{
  //std::string aType = theObject->groupName();
  //DocumentPtr aDoc = theObject->document();
  //int aRow = aDoc->index(theObject);
  //if (aRow == -1)
    return QModelIndex();
  //return createIndex(aRow, 0, theObject.get());
}

//******************************************************
QVariant XGUI_DataModel::data(const QModelIndex& theIndex, int theRole) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  int aNbFolders = myXMLReader.rootFoldersNumber();
  int theIndexRow = theIndex.row();

  if ((theIndex.column() == 1) ) {
    //if (theIndexRow >= aNbFolders) {
    //  if (theRole == Qt::DecorationRole) {
    //    return QIcon(":pictures/arrow.png");
    //  }
    //}
    return QVariant();
  }

  int aParentRow = theIndex.internalId();
  if (aParentRow == -1) { // First level of items
    if (theIndexRow < aNbFolders) { // A folder
      switch (theRole) {
        case Qt::DisplayRole:
          return QString(myXMLReader.rootFolderName(theIndexRow).c_str()) + 
            QString(" (%1)").arg(rowCount(theIndex));
        case Qt::DecorationRole:
          return QIcon(myXMLReader.rootFolderIcon(theIndexRow).c_str());
      }
    } else {
      ObjectPtr aObj = aRootDoc->object(myXMLReader.rootType(), theIndexRow - aNbFolders);
      switch (theRole) {
        case Qt::DisplayRole:
          return aObj->data()->name().c_str();
        //case Qt::DecorationRole:
        //  return featureIcon(aFeature);
      }
    }
  } else { // Content of folders
    if (aParentRow < aNbFolders) {
      std::string aType = myXMLReader.rootFolderType(aParentRow);
      ObjectPtr aObj = aRootDoc->object(aType, theIndexRow);
      switch (theRole) {
        case Qt::DisplayRole:
          return aObj->data()->name().c_str();
      }
    }
  }
  return QVariant();
}

//******************************************************
QVariant XGUI_DataModel::headerData(int theSection, Qt::Orientation theOrient, int theRole) const
{
  return QVariant();
}

//******************************************************
int XGUI_DataModel::rowCount(const QModelIndex& theParent) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  if (!aSession->hasModuleDocument())
    return 0;
  DocumentPtr aRootDoc = aSession->moduleDocument();
  int aNbFolders = myXMLReader.rootFoldersNumber();

  int aNbItems = 0;
  std::string aType = myXMLReader.rootType();
  if (!aType.empty())
    aNbItems = aRootDoc->size(aType);

  if (!theParent.isValid())
    return aNbFolders + aNbItems;

  int aParentPos = theParent.row();
  if (aParentPos < aNbFolders) {
    // This is number of items under folder
    aType = myXMLReader.rootFolderType(aParentPos);
    return aRootDoc->size(aType);
  }
  return 0;
}

//******************************************************
int XGUI_DataModel::columnCount(const QModelIndex& theParent) const
{
  return 2;
}

//******************************************************
QModelIndex XGUI_DataModel::index(int theRow, int theColumn, const QModelIndex &theParent) const
{
  if (!theParent.isValid())
    return createIndex(theRow, theColumn, -1);

  return createIndex(theRow, theColumn, theParent.row());
}

//******************************************************
QModelIndex XGUI_DataModel::parent(const QModelIndex& theIndex) const
{
  if (theIndex.isValid() && (theIndex.internalId() != -1)) {
    return createIndex(theIndex.internalId(), theIndex.column(), -1);
  }
  return QModelIndex();
}

//******************************************************
bool XGUI_DataModel::hasChildren(const QModelIndex& theParent) const
{
  int aNbFolders = myXMLReader.rootFoldersNumber();
  if (!theParent.isValid() && aNbFolders)
    return true;
  if (theParent.internalId() == -1) {
    if (theParent.row() < aNbFolders) {
      std::string aType = myXMLReader.rootFolderType(theParent.row());
      if (!aType.empty()) {
        SessionPtr aSession = ModelAPI_Session::get();
        DocumentPtr aRootDoc = aSession->moduleDocument();
        return aRootDoc->size(aType) > 0;
      }
    }
  }
  return false;
}

//******************************************************
bool XGUI_DataModel::insertRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginInsertRows(theParent, theRow, theRow + theCount - 1);
  endInsertRows();

  return true;
}

//******************************************************
bool XGUI_DataModel::removeRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginRemoveRows(theParent, theRow, theRow + theCount - 1);
  endRemoveRows();
  return true;
}