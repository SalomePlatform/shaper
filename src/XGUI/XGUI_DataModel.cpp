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

#include "XGUI_DataModel.h"
#include "XGUI_ObjectsBrowser.h"

#include <ModuleBase_IconFactory.h>
#include <ModuleBase_ITreeNode.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_ResultField.h>

#include <Config_FeatureMessage.h>

#include <Events_Loop.h>

#include <cassert>

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif



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
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
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
      if ((*aIt)->groupName() == ModelAPI_ResultPart::group()) {
        emit beforeTreeRebuild();
        myRoot->update();
        rebuildDataTree();
        emit treeRebuilt();
        return;
      }
    }
    if (aCreated.length() == 0)
      return;

    emit beforeTreeRebuild();
    QTreeNodesList aNodes = myRoot->objectCreated(aCreated);
    ModuleBase_ITreeNode* aParent;
    int aRow = 0;
    QModelIndex aParentIndex1, aParentIndex2;
    ObjectPtr aObj;
    bool aRebuildAll = false;

    foreach(ModuleBase_ITreeNode* aNode, aNodes) {
      aObj = aNode->object();
      aParent = aNode->parent();
      if (aObj.get() && (aObj->groupName() == ModelAPI_Folder::group())) {
        aParent->update();
        aRebuildAll = true;
      }
      else {
        aRow = aParent->nodeRow(aNode);
        aParentIndex1 = getParentIndex(aNode, 0);
        aParentIndex2 = getParentIndex(aNode, 2);
        insertRows(aRow, 1, aParentIndex1);
        dataChanged(aParentIndex1, aParentIndex2);
      }
    }
    if (aRebuildAll)
      rebuildDataTree();

    emit treeRebuilt();
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
    std::shared_ptr<ModelAPI_ObjectDeletedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
    const std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string>>& aMsgGroups =
      aUpdMsg->groups();
    QTreeNodesList aList;
    std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string>>::const_iterator aIt;
    emit beforeTreeRebuild();
    for (aIt = aMsgGroups.cbegin(); aIt != aMsgGroups.cend(); aIt++) {
      aList.append(myRoot->objectsDeleted(aIt->first, aIt->second.c_str()));
    }
    // Remove obsolete nodes
    QTreeNodesList aRemaining;
    foreach(ModuleBase_ITreeNode* aNode, aList) {
      if (myRoot->hasSubNode(aNode))
        aRemaining.append(aNode);
    }
    // Update remaining nodes
    foreach(ModuleBase_ITreeNode* aNode, aRemaining) {
      if (aNode->parent())
        aNode->parent()->update();
    }
    rebuildDataTree();
    emit treeRebuilt();
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    QObjectPtrList aCreated;
    std::set<ObjectPtr>::const_iterator aIt;
    bool aRebuildAll = false;
    emit beforeTreeRebuild();
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
    }
    else {
      QSet<ModuleBase_ITreeNode*> aParents;
      foreach(ObjectPtr aObj, aCreated) {
        ModuleBase_ITreeNode* aNode = myRoot->subNode(aObj);
        if (aNode) {
          if (aNode->parent()) {
            if (aNode->parent() == myRoot) {
              aParents.clear();
              aParents.insert(myRoot);
              break;
            }
            else {
              aNode = aNode->parent();
            }
          }
          aParents.insert(aNode);
        }
      }
      foreach(ModuleBase_ITreeNode* aNode, aParents) {
        aNode->update();
      }
    }
    rebuildDataTree();
    emit treeRebuilt();
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_ORDER_UPDATED)) {
    std::shared_ptr<ModelAPI_OrderUpdatedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_OrderUpdatedMessage>(theMessage);
    if (aUpdMsg->reordered().get()) {
      DocumentPtr aDoc = aUpdMsg->reordered()->document();
      std::string aGroup = aUpdMsg->reordered()->group();
      ModuleBase_ITreeNode* aNode = myRoot->findParent(aDoc, aGroup.c_str());
      if (aNode) {
        emit beforeTreeRebuild();
        aNode->update();
        rebuildDataTree();
        emit treeRebuilt();
      }
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENT_CHANGED)) {
    if (ModelAPI_Session::get()->hasModuleDocument()) {
      DocumentPtr aDoc = ModelAPI_Session::get()->activeDocument();
      ModuleBase_ITreeNode* aRoot = myRoot->findRoot(aDoc);
      if (aRoot) {
        updateSubTree(aRoot);
      }
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
      std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    QObjectPtrList aCreated;
    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aObjects.cbegin(); aIt != aObjects.cend(); aIt++) {
      ObjectPtr aObj = (*aIt);
      if (aObj->groupName() == ModelAPI_ResultField::group()) {
        aCreated.append(aObj);
      }
    }
    if (aCreated.length() == 0)
      return;
    emit beforeTreeRebuild();
    foreach(ObjectPtr aObj, aCreated) {
      ModuleBase_ITreeNode* aNode = myRoot->subNode(aObj);
      if (aNode) {
        int aOldNb = aNode->childrenCount();
        aNode->update();
        int aNewNb = aNode->childrenCount();

        QModelIndex aFirstIdx = getIndex(aNode, 0);
        QModelIndex aLastIdx = getIndex(aNode, 2);

        if (aNewNb > aOldNb) {
          insertRows(aOldNb - 1, aNewNb - aOldNb, aFirstIdx);
        }
        else if (aNewNb < aOldNb) {
          if (aNewNb)
            removeRows(aNewNb - 1, aOldNb - aNewNb, aFirstIdx);
          else if (aOldNb)
            removeRows(0, aOldNb, aFirstIdx);
        }
        dataChanged(aFirstIdx, aLastIdx);
      }
    }
    emit treeRebuilt();
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


//******************************************************
DocumentPtr XGUI_DataModel::document(const QModelIndex& theIndex) const
{
  ModuleBase_ITreeNode* aNode = (ModuleBase_ITreeNode*)theIndex.internalPointer();
  return aNode->document();
}


//******************************************************
bool XGUI_DataModel::hasNode(ModuleBase_ITreeNode* theNode) const
{
  return myRoot->hasSubNode(theNode);
}
