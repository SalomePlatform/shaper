// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_IDocumentDataModel.cpp
// Created:     28 Apr 2015
// Author:      Vitaly SMETANNIKOV

#include "XGUI_DataModel.h"

#include <ModuleBase_IconFactory.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultPart.h>

#include <Config_FeatureMessage.h>

#include <Events_Loop.h>
#include <Events_Error.h>

#include <QIcon>
#include <QBrush>

#define ACTIVE_COLOR QColor(0,72,140)
#define PASSIVE_COLOR Qt::black

/// Returns ResultPart object if the given object is a Part feature
/// Otherwise returns NULL
ResultPartPtr getPartResult(ModelAPI_Object* theObj)
{
  ModelAPI_Feature* aFeature = dynamic_cast<ModelAPI_Feature*>(theObj);
  if (aFeature) {
    ResultPtr aRes = aFeature->firstResult();
    if (aRes.get() && (aRes->groupName() == ModelAPI_ResultPart::group())) {
      return std::dynamic_pointer_cast<ModelAPI_ResultPart>(aRes);
    }
  }
  return ResultPartPtr();
}

/// Returns pointer on document if the given object is document object
ModelAPI_Document* getSubDocument(void* theObj)
{
  ModelAPI_Document* aDoc = dynamic_cast<ModelAPI_Document*>((ModelAPI_Entity*)theObj);
  return aDoc;
}




// Constructor *************************************************
XGUI_DataModel::XGUI_DataModel(QObject* theParent) : QAbstractItemModel(theParent)
{
  myXMLReader.readAll();

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
}

//******************************************************
void XGUI_DataModel::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  std::string aRootType = myXMLReader.rootType();
  std::string aSubType = myXMLReader.subType();
  int aNbFolders = myXMLReader.rootFoldersNumber();
  int aNbSubFolders = myXMLReader.subFoldersNumber();

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
        if (aObjType == aRootType) {
          insertRow(aRow + aNbFolders + 1);
        } else {
          int aFolderId = myXMLReader.rootFolderId(aObjType);
          if (aFolderId != -1) {
            insertRow(aRow, createIndex(aFolderId, 0, -1));
          }
        } 
      } else {
        // Object created in sub-document
        QModelIndex aDocRoot = findDocumentRootIndex(aDoc.get());
        if (aDocRoot.isValid()) {
          int aRow = aDoc->size(aObjType) - 1;
          if (aObjType == aSubType) {
            insertRow(aRow + aNbSubFolders, aDocRoot);
          } else {
            int aFolderId = myXMLReader.subFolderId(aObjType);
            if (aFolderId != -1) {
              insertRow(aRow, createIndex(aFolderId, 0, aDoc.get()));
            }
          }
        } 
#ifdef _DEBUG
        else {
          Events_Error::send("Problem with Data Model definition of sub-document");
        }
#endif
      }
    }
    // Deleted object event ***********************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
    std::shared_ptr<ModelAPI_ObjectDeletedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
    DocumentPtr aDoc = aUpdMsg->document();
    std::set<std::string> aGroups = aUpdMsg->groups();
    std::set<std::string>::const_iterator aIt;
    for (aIt = aGroups.begin(); aIt != aGroups.end(); ++aIt) {
      std::string aGroup = (*aIt);
      if (aDoc == aRootDoc) {  // If root objects
        int aRow = aRootDoc->size(aGroup);
        if (aGroup == aRootType) {
          removeRow(aRow + aNbFolders);
        } else {
          int aFolderId = myXMLReader.rootFolderId(aGroup);
          if (aFolderId != -1) {
            QModelIndex aFolderIndex = createIndex(aFolderId, 0, -1);
            removeRow(aRow, aFolderIndex);
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
  if (theIndex.internalId() < 0) // this is a folder
    return ObjectPtr();
  ModelAPI_Object* aObj = (ModelAPI_Object*)theIndex.internalPointer();
  if (getSubDocument(aObj)) // the selected index is a folder of sub-document
    return ObjectPtr();

  // We can not create the ObjectPtr directly because the pointer will be deleted 
  // with deletion of the ObjectPtr because its counter become to 0.
  DocumentPtr aDoc = aObj->document();
  std::string aType = aObj->groupName();

  ObjectPtr aObjPtr;
  for (int i = 0; i < aDoc->size(aType); i++) {
    aObjPtr = aDoc->object(aType, i);
    if (aObjPtr.get() == aObj)
      return aObjPtr;
  }
  return ObjectPtr();
}

//******************************************************
QModelIndex XGUI_DataModel::objectIndex(const ObjectPtr theObject) const
{
  std::string aType = theObject->groupName();
  DocumentPtr aDoc = theObject->document();
  int aRow = aDoc->index(theObject);
  if (aRow == -1)
    return QModelIndex();

  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if (aDoc == aRootDoc && myXMLReader.rootType() == aType) { 
    // The object from root document
    aRow += myXMLReader.rootFoldersNumber();
  } else if (myXMLReader.subType() == aType) { 
    // The object from sub document
    aRow += myXMLReader.subFoldersNumber();
  }
  return createIndex(aRow, 0, theObject.get());
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

  int aParentId = theIndex.internalId();
  if (aParentId == -1) { // root folders
    switch (theRole) {
      case Qt::DisplayRole:
        return QString(myXMLReader.rootFolderName(theIndexRow).c_str()) + 
          QString(" (%1)").arg(rowCount(theIndex));
      case Qt::DecorationRole:
        return QIcon(myXMLReader.rootFolderIcon(theIndexRow).c_str());
      case Qt::ForegroundRole:
        if (aRootDoc->isActive())
          return QBrush(ACTIVE_COLOR);
        else
          return QBrush(PASSIVE_COLOR);
    }
  } else {
    ModelAPI_Document* aSubDoc = getSubDocument(theIndex.internalPointer());

    if (theRole == Qt::ForegroundRole) {
      bool aIsActive = false;
      if (aSubDoc)
        aIsActive = aSubDoc->isActive();
      else {
        ModelAPI_Object* aObj = (ModelAPI_Object*)theIndex.internalPointer();
        aIsActive = aObj->document()->isActive();
      }
      if (aIsActive)
        return QBrush(ACTIVE_COLOR);
      else
        return QBrush(PASSIVE_COLOR);
    }

    if (aSubDoc) { // this is a folder of sub document
      switch (theRole) {
        case Qt::DisplayRole:
          return QString(myXMLReader.subFolderName(theIndexRow).c_str()) + 
            QString(" (%1)").arg(rowCount(theIndex));
        case Qt::DecorationRole:
          return QIcon(myXMLReader.subFolderIcon(theIndexRow).c_str());
      }
    } else {
      ModelAPI_Object* aObj = (ModelAPI_Object*)theIndex.internalPointer();
      switch (theRole) {
      case Qt::DisplayRole:
        if (aObj->groupName() == ModelAPI_ResultParameter::group()) {
          ModelAPI_ResultParameter* aParam = dynamic_cast<ModelAPI_ResultParameter*>(aObj);
          AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
          QString aVal = QString::number(aValueAttribute->value());
          QString aTitle = QString(aObj->data()->name().c_str());
          return aTitle + " = " + aVal;
        }
        return aObj->data()->name().c_str();
      case Qt::DecorationRole:
        return ModuleBase_IconFactory::get()->getIcon(object(theIndex));
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

  if (!theParent.isValid()) {
    // Return number of items in root
    int aNbFolders = myXMLReader.rootFoldersNumber();
    int aNbItems = 0;
    std::string aType = myXMLReader.rootType();
    if (!aType.empty())
      aNbItems = aRootDoc->size(aType);
    return aNbFolders + aNbItems;
  }

  int aId = theParent.internalId();
  if (aId == -1) { 
    // this is a folder under root
    int aParentPos = theParent.row();
    std::string aType = myXMLReader.rootFolderType(aParentPos);
    //qDebug("### %s = %i\n", aType.c_str(), aRootDoc->size(aType));
    return aRootDoc->size(aType);
  } else {
    // It is an object which could have children
    ModelAPI_Document* aDoc = getSubDocument(theParent.internalPointer());
    if (aDoc) { 
      // a folder of sub-document
      std::string aType = myXMLReader.subFolderType(theParent.row());
      return aDoc->size(aType);
    } else {
      // Check for Part feature
      ModelAPI_Object* aObj = (ModelAPI_Object*)theParent.internalPointer();
      ResultPartPtr aPartRes = getPartResult(aObj);
      if (aPartRes.get()) {
        DocumentPtr aSubDoc = aPartRes->partDoc();
        int aNbSubFolders = myXMLReader.subFoldersNumber();
        int aNbSubItems = 0;
        std::string aSubType = myXMLReader.subType();
        if (!aSubType.empty())
          aNbSubItems = aSubDoc->size(aSubType);
        return aNbSubItems + aNbSubFolders;
      }
    }
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
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  int aNbFolders = myXMLReader.rootFoldersNumber();

  if (!theParent.isValid()) {
    if (theRow < aNbFolders) // Return first level folder index
      return createIndex(theRow, theColumn, -1);
    else { // return object under root index
      std::string aType = myXMLReader.rootType();
      int aObjId = theRow - aNbFolders;
      if (aObjId < aRootDoc->size(aType)) {
        ObjectPtr aObj = aRootDoc->object(aType, aObjId);
        QModelIndex aIndex = objectIndex(aObj);
        if (theColumn != 0)
          return createIndex(aIndex.row(), theColumn, aIndex.internalPointer());
        return aIndex;
      }
      return QModelIndex();
    }
  }
  int aId = theParent.internalId();
  int aParentPos = theParent.row();
  if (aId == -1) { // return object index inside of first level of folders
    std::string aType = myXMLReader.rootFolderType(aParentPos);
    if (theRow < aRootDoc->size(aType)) {
      ObjectPtr aObj = aRootDoc->object(aType, theRow);
      QModelIndex aIndex = objectIndex(aObj);
      if (theColumn != 0)
        return createIndex(aIndex.row(), theColumn, aIndex.internalPointer());
      return aIndex;
    }
  } else {
    // It is an object which could have children
    ModelAPI_Document* aDoc = getSubDocument(theParent.internalPointer());
    if (aDoc) { 
      // It is a folder of sub-document
      std::string aType = myXMLReader.subFolderType(aParentPos);
      if (theRow < aDoc->size(aType)) {
        ObjectPtr aObj = aDoc->object(aType, theRow);
        QModelIndex aIndex = objectIndex(aObj);
        if (theColumn != 0)
          return createIndex(aIndex.row(), theColumn, aIndex.internalPointer());
        return aIndex;
      }
    } else {
      ModelAPI_Object* aParentObj = (ModelAPI_Object*)theParent.internalPointer();

      // Check for Part feature
      ResultPartPtr aPartRes = getPartResult(aParentObj);
      if (aPartRes.get()) {
        DocumentPtr aSubDoc = aPartRes->partDoc();
        int aNbSubFolders = myXMLReader.subFoldersNumber();
        if (theRow < aNbSubFolders) { // Create a Folder of sub-document
          return createIndex(theRow, theColumn, aSubDoc.get());
        } else {
          // this is an object under sub document root
          std::string aType = myXMLReader.subType();
          ObjectPtr aObj = aSubDoc->object(aType, theRow - aNbSubFolders);
          QModelIndex aIndex = objectIndex(aObj);
          if (theColumn != 0)
            return createIndex(aIndex.row(), theColumn, aIndex.internalPointer());
          return aIndex;
        }
      }
    }
  }
  return QModelIndex();
}

//******************************************************
QModelIndex XGUI_DataModel::parent(const QModelIndex& theIndex) const
{
  int aId = theIndex.internalId();
  if (aId != -1) { // The object is not a root folder
    ModelAPI_Document* aDoc = getSubDocument(theIndex.internalPointer());
    if (aDoc) { 
      // It is a folder of sub-document
      return findDocumentRootIndex(aDoc);
    }
    ModelAPI_Object* aObj = (ModelAPI_Object*) theIndex.internalPointer();
    std::string aType = aObj->groupName();
    SessionPtr aSession = ModelAPI_Session::get();
    DocumentPtr aRootDoc = aSession->moduleDocument();
    DocumentPtr aSubDoc = aObj->document();
    if (aSubDoc == aRootDoc) {
      if (aType == myXMLReader.rootType())
        return QModelIndex();
      else {
        // return first level of folder index
        int aFolderId = myXMLReader.rootFolderId(aType);
        // Items in a one row must have the same parent
        return createIndex(aFolderId, 0, -1);
      }
    } else {
      if (aType == myXMLReader.subType())
        return findDocumentRootIndex(aSubDoc.get());
      else {
        // return first level of folder index
        int aFolderId = myXMLReader.subFolderId(aType);
        // Items in a one row must have the same parent
        return createIndex(aFolderId, 0, aSubDoc.get());
      }
    }
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
    std::string aType = myXMLReader.rootFolderType(theParent.row());
    if (!aType.empty()) {
      SessionPtr aSession = ModelAPI_Session::get();
      DocumentPtr aRootDoc = aSession->moduleDocument();
      return aRootDoc->size(aType) > 0;
    }
  } else {
    ModelAPI_Document* aDoc = getSubDocument(theParent.internalPointer());
    if (aDoc) { 
      // a folder of sub-document
      std::string aType = myXMLReader.subFolderType(theParent.row());
      return aDoc->size(aType) > 0;
    } else {
      // Check that it could be an object with children
      ModelAPI_Object* aObj = (ModelAPI_Object*)theParent.internalPointer();

      // Check for Part feature
      ResultPartPtr aPartRes = getPartResult(aObj);
      if (aPartRes.get())
        return true;
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

//******************************************************
Qt::ItemFlags XGUI_DataModel::flags(const QModelIndex& theIndex) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if (theIndex.internalId() > -1) {
    aFlags |= Qt::ItemIsEditable;
  }
  return aFlags;
}

//******************************************************
QModelIndex XGUI_DataModel::findDocumentRootIndex(ModelAPI_Document* theDoc) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if (myXMLReader.isAttachToResult()) { // If document is attached to result
    int aNb = aRootDoc->size(ModelAPI_ResultPart::group());
    ObjectPtr aObj;
    ResultPartPtr aPartRes;
    for (int i = 0; i < aNb; i++) {
      aObj = aRootDoc->object(ModelAPI_ResultPart::group(), i);
      aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
      if (aPartRes.get() && (aPartRes->partDoc().get() == theDoc)) {
        int aRow = i;
        if (myXMLReader.rootType() == ModelAPI_Feature::group())
          aRow += myXMLReader.rootFoldersNumber();
        return createIndex(aRow, 0, aObj.get());
      }
    }
  } else { // If document is attached to feature
    int aNb = aRootDoc->size(ModelAPI_Feature::group());
    ObjectPtr aObj;
    ResultPartPtr aPartRes;
    for (int i = 0; i < aNb; i++) {
      aObj = aRootDoc->object(ModelAPI_Feature::group(), i);
      aPartRes = getPartResult(aObj.get());
      if (aPartRes.get() && (aPartRes->partDoc().get() == theDoc)) {
        int aRow = i;
        if (myXMLReader.rootType() == ModelAPI_Feature::group())
          aRow += myXMLReader.rootFoldersNumber();
        return createIndex(aRow, 0, aObj.get());
      }
    }
  }
  return QModelIndex();
}

//******************************************************
QModelIndex XGUI_DataModel::documentRootIndex(DocumentPtr theDoc) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if (theDoc == aRootDoc)
    return QModelIndex();
  else 
    return findDocumentRootIndex(theDoc.get());
}