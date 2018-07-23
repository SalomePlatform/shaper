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

#include "XGUI_DataModel.h"
#include "XGUI_ObjectsBrowser.h"

#include <ModuleBase_IconFactory.h>
#include <ModuleBase_ITreeNode.h>

#include <ModelAPI_Session.h>

#include <Config_FeatureMessage.h>

#include <Events_Loop.h>

#include <cassert>



// Constructor *************************************************
XGUI_DataModel::XGUI_DataModel(QObject* theParent) : QAbstractItemModel(theParent)//,
  //myIsEventsProcessingBlocked(false)
{
  XGUI_ObjectsBrowser* aOB = qobject_cast<XGUI_ObjectsBrowser*>(theParent);
  myWorkshop = aOB->workshop();

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_ORDER_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));
}

XGUI_DataModel::~XGUI_DataModel()
{
  clear();
}

//******************************************************
void XGUI_DataModel::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();
    QObjectPtrList aCreated;
    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aObjects.cbegin(); aIt != aObjects.cend(); aIt++) {
      if ((*aIt)->isInHistory())
        aCreated.append(*aIt);
    }
    QTreeNodesList aNodes = myRoot->objectCreated(aCreated);
    ModuleBase_ITreeNode* aParent;
    int aRow = 0;
    QModelIndex aParentIndex;
    foreach(ModuleBase_ITreeNode* aNode, aNodes) {
      aParent = aNode->parent();
      aRow = aParent->nodeRow(aNode);
      aParentIndex = getParentIndex(aNode, 0);
      insertRows(aRow, 1, aParentIndex);
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
      std::shared_ptr<ModelAPI_ObjectDeletedMessage> aUpdMsg =
          std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
      DocumentPtr aDoc = aUpdMsg->document();
      std::set<std::string> aMsgGroups = aUpdMsg->groups();
      std::set<std::string>::const_iterator aIt;
      for (aIt = aMsgGroups.cbegin(); aIt != aMsgGroups.cend(); aIt++)
        QTreeNodesList aList = myRoot->objectsDeleted(aDoc, (*aIt).c_str());
      rebuildDataTree();
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    QObjectPtrList aCreated;
    std::set<ObjectPtr>::const_iterator aIt;
    bool aRebuildAll = false;
    for (aIt = aObjects.cbegin(); aIt != aObjects.cend(); aIt++) {
      ObjectPtr aObj = (*aIt);
      if (!aObj->isInHistory())
        continue;

      if (aObj->data()->isValid()) {
        if (aObj->groupName() == ModelAPI_Folder::group()) {
          aRebuildAll = true;
          break;
        }
        aCreated.append(*aIt);
      }
    }
    if (aRebuildAll) {
      myRoot->update();
      rebuildDataTree();
    } else {
      foreach(ObjectPtr aObj, aCreated) {
        ModuleBase_ITreeNode* aNode = myRoot->subNode(aObj);
        if (aNode) {
          QModelIndex aFirstIdx = getIndex(aNode, 0);
          QModelIndex aLastIdx = getIndex(aNode, 2);
          dataChanged(aFirstIdx, aLastIdx);
        }
      }
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_ORDER_UPDATED)) {
    std::shared_ptr<ModelAPI_OrderUpdatedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_OrderUpdatedMessage>(theMessage);
    if (aUpdMsg->reordered().get()) {
      DocumentPtr aDoc = aUpdMsg->reordered()->document();
      std::string aGroup = aUpdMsg->reordered()->group();
      ModuleBase_ITreeNode* aNode = myRoot->findParent(aDoc, aGroup.c_str());
      if (aNode) {
        aNode->update();
        updateSubTree(aNode);
      }
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENT_CHANGED)) {
    DocumentPtr aDoc = ModelAPI_Session::get()->activeDocument();
    ModuleBase_ITreeNode* aRoot = myRoot->findRoot(aDoc);
    if (aRoot) {
      updateSubTree(aRoot);
    }
  }
}

//******************************************************
void XGUI_DataModel::clear()
{
  beginResetModel();
  endResetModel();
}

//******************************************************
void XGUI_DataModel::rebuildDataTree()
{
  beginResetModel();
  endResetModel();
  emit treeRebuilt();
}

//******************************************************
ObjectPtr XGUI_DataModel::object(const QModelIndex& theIndex) const
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    return aNode->object();
  }
  return ObjectPtr();
}

//******************************************************
QModelIndex XGUI_DataModel::objectIndex(const ObjectPtr theObject, int theColumn) const
{
  ModuleBase_ITreeNode* aNode = myRoot->subNode(theObject);
  if (aNode) {
    return getIndex(aNode, theColumn);
  }
  return QModelIndex();
}

//******************************************************
QVariant XGUI_DataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    return aNode->data(theIndex.column(), theRole);
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
  ModuleBase_ITreeNode* aParentNode = (theParent.isValid()) ?
    (ModuleBase_ITreeNode*)theParent.internalPointer() : myRoot;
  return aParentNode->childrenCount();
}

//******************************************************
int XGUI_DataModel::columnCount(const QModelIndex& theParent) const
{
  return 3;
}

//******************************************************
QModelIndex XGUI_DataModel::index(int theRow, int theColumn, const QModelIndex &theParent) const
{
  int aa = theParent.row();
  ModuleBase_ITreeNode* aParentNode = (theParent.isValid()) ?
    (ModuleBase_ITreeNode*)theParent.internalPointer() : myRoot;
  ModuleBase_ITreeNode* aSubNode = aParentNode->subNode(theRow);
  assert(aSubNode);
  return createIndex(theRow, theColumn, aSubNode);
}

//******************************************************
QModelIndex XGUI_DataModel::parent(const QModelIndex& theIndex) const
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    return getParentIndex(aNode, 1);
  }
  return QModelIndex();
}

//******************************************************
bool XGUI_DataModel::hasChildren(const QModelIndex& theParent) const
{
  ModuleBase_ITreeNode* aParentNode = (theParent.isValid()) ?
    (ModuleBase_ITreeNode*)theParent.internalPointer() : myRoot;
  return aParentNode->childrenCount() > 0;
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

//******************************************************
Qt::ItemFlags XGUI_DataModel::flags(const QModelIndex& theIndex) const
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    return aNode->flags(theIndex.column());
  }
  return Qt::ItemFlags();
}


//******************************************************
QModelIndex XGUI_DataModel::documentRootIndex(DocumentPtr theDoc, int theColumn) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if (theDoc == aRootDoc)
    return QModelIndex();
  else {
    ModuleBase_ITreeNode* aDocNode = 0;
    foreach(ModuleBase_ITreeNode* aNode, myRoot->children()) {
      if (aNode->document() == theDoc) {
        aDocNode = aNode;
        break;
      }
    }
    if (aDocNode)
      return getIndex(aDocNode, theColumn);
  }
  return QModelIndex();
}

//******************************************************
bool XGUI_DataModel::hasHiddenState(const QModelIndex& theIndex)
{
  if (theIndex.isValid()) {
    ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
    return aNode->visibilityState() == ModuleBase_ITreeNode::Hidden;
  }
  return false;
}

//******************************************************
bool XGUI_DataModel::hasIndex(const QModelIndex& theIndex) const
{
  ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
  return myRoot->hasSubNode(aNode);
}

//******************************************************
QModelIndex XGUI_DataModel::getParentIndex(ModuleBase_ITreeNode* theNode, int thCol) const
{
  ModuleBase_ITreeNode* aParent = theNode->parent();
  if (aParent == myRoot) {
    return QModelIndex();
  } else {
    return getIndex(aParent, thCol);
  }
}

//******************************************************
QModelIndex XGUI_DataModel::getIndex(ModuleBase_ITreeNode* theNode, int thCol) const
{
  if (theNode == myRoot)
    return QModelIndex();
  int aRow = theNode->parent()->nodeRow(theNode);
  return createIndex(aRow, thCol, theNode);
}


//******************************************************
void XGUI_DataModel::updateSubTree(ModuleBase_ITreeNode* theParent)
{
  int aRows = theParent->childrenCount();
  if (aRows) {
    QModelIndex aParent = getIndex(theParent, 0);
    QModelIndex aFirstIdx = aParent.child(0, 0);
    QModelIndex aLastIdx = aParent.child(aRows - 1, 2);
    dataChanged(aFirstIdx, aLastIdx);
  }
}
