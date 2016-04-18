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
#include <ModelAPI_Feature.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>

#include <Config_FeatureMessage.h>

#include <Events_Loop.h>
#include <Events_Error.h>

#include <QIcon>
#include <QBrush>

#define ACTIVE_COLOR Qt::black
//#define ACTIVE_COLOR QColor(0,72,140)
//#define PASSIVE_COLOR Qt::black

/// Returns ResultPart object if the given object is a Part feature
/// Otherwise returns NULL

#define SELECTABLE_COLOR QColor(80, 80, 80)
#define DISABLED_COLOR QColor(200, 200, 200)

ResultPartPtr getPartResult(ModelAPI_Object* theObj)
{
  ModelAPI_Feature* aFeature = dynamic_cast<ModelAPI_Feature*>(theObj);
  if (aFeature) {
    ResultPtr aRes = aFeature->firstResult();
    if (aRes.get() && (aRes->groupName() == ModelAPI_ResultPart::group())) {
      ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aRes);
      // Use only original parts, not a placement results
      if (aPartRes == aPartRes->original())
      return aPartRes;
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
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_ORDER_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));
}

//******************************************************
void XGUI_DataModel::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  std::string aRootType = myXMLReader.rootType();
  std::string aSubType = myXMLReader.subType();
  int aNbFolders = foldersCount();

  // Created object event *******************
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    std::set<ObjectPtr>::const_iterator aIt;
    std::string aObjType;
    for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
      ObjectPtr aObject = (*aIt);
      // We do not show objects which does not need to be shown in object browser
      if (!aObject->isInHistory())
        continue;

      aObjType = aObject->groupName();
      DocumentPtr aDoc = aObject->document();
      if (aDoc == aRootDoc) {
        // Check that new folders could appear
        QStringList aNotEmptyFolders = listOfShowNotEmptyFolders();
        foreach (QString aNotEmptyFolder, aNotEmptyFolders) {
          if ((aNotEmptyFolder.toStdString() == aObjType) && (aRootDoc->size(aObjType) == 1))
            // Appears first object in folder which can not be shown empty
            insertRow(myXMLReader.rootFolderId(aObjType));
        }
        // Insert new object
        int aRow = aRootDoc->size(aObjType) - 1;
        if (aRow != -1) {
          if (aObjType == aRootType) {
            insertRow(aRow + aNbFolders + 1);
          } else {
            int aFolderId = myXMLReader.rootFolderId(aObjType);
            if (aFolderId != -1) {
              insertRow(aRow, createIndex(aFolderId, 0, -1));
            }
          } 
        }
      } else {
        // Object created in sub-document
        QModelIndex aDocRoot = findDocumentRootIndex(aDoc.get());
        if (aDocRoot.isValid()) {
          // Check that new folders could appear
          QStringList aNotEmptyFolders = listOfShowNotEmptyFolders(false);
          foreach (QString aNotEmptyFolder, aNotEmptyFolders) {
            if ((aNotEmptyFolder.toStdString() == aObjType) && (aDoc->size(aObjType) == 1))
              // Appears first object in folder which can not be shown empty
              insertRow(myXMLReader.subFolderId(aObjType), aDocRoot);
          }
          int aRow = aDoc->index(aObject);
          if (aRow != -1) {
            int aNbSubFolders = foldersCount(aDoc.get());
            if (aObjType == aSubType) {
              // List of objects under document root
              insertRow(aRow + aNbSubFolders, aDocRoot);
            } else {
              // List of objects under a folder
              if (aRow != -1) {
                int aFolderId = folderId(aObjType, aDoc.get());
                if (aFolderId != -1) {
                  QModelIndex aParentFolder = createIndex(aFolderId, 0, aDoc.get());
                  insertRow(aRow, aParentFolder);
                  emit dataChanged(aParentFolder, aParentFolder);
                }
              }
            }
          }
        } 
#ifdef _DEBUG
        else
          Events_Error::send("Problem with Data Model definition of sub-document");
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
          // Process root folder
          removeRow(aRow + aNbFolders);
          rebuildBranch(aNbFolders, aRow);
        } else {
          // Process root sub-folder
          int aFolderId = myXMLReader.rootFolderId(aGroup);
          if (aFolderId != -1) {
            QModelIndex aFolderIndex = createIndex(aFolderId, 0, -1);
            removeRow(aRow, aFolderIndex);
            //rebuildBranch(0, aRow);
          }
        }
        // Check that some folders could erased
        QStringList aNotEmptyFolders = listOfShowNotEmptyFolders();
        foreach (QString aNotEmptyFolder, aNotEmptyFolders) {
          if ((aNotEmptyFolder.toStdString() == aGroup) && (aRootDoc->size(aGroup) == 0)) {
            // Appears first object in folder which can not be shown empty
            removeRow(myXMLReader.rootFolderId(aGroup));
            //rebuildBranch(0, aNbFolders + aDoc->size(myXMLReader.rootType()));
            break;
          }
        }
      } else {
        // Remove row for sub-document
        QModelIndex aDocRoot = findDocumentRootIndex(aDoc.get());
        if (aDocRoot.isValid()) {
          int aRow = aDoc->size(aGroup);
          int aNbSubFolders = foldersCount(aDoc.get());
          if (aGroup == aSubType) {
            // List of objects under document root
            removeRow(aRow + aNbSubFolders, aDocRoot);
            rebuildBranch(aNbSubFolders, aRow, aDocRoot);
          } else {
            // List of objects under a folder
            int aFolderId = folderId(aGroup, aDoc.get());
            if (aFolderId != -1) {
              QModelIndex aFolderRoot = createIndex(aFolderId, 0, aDoc.get());
              removeRow(aRow, aFolderRoot);
              //rebuildBranch(0, aRow, aFolderRoot);
            }
          }
          // Check that some folders could disappear
          QStringList aNotEmptyFolders = listOfShowNotEmptyFolders(false);
          int aSize = aDoc->size(aGroup);
          foreach (QString aNotEmptyFolder, aNotEmptyFolders) {
            if ((aNotEmptyFolder.toStdString() == aGroup) && (aSize == 0)) {
              // Appears first object in folder which can not be shown empty
              removeRow(myXMLReader.subFolderId(aGroup), aDocRoot);
              //rebuildBranch(0, aNbSubFolders + aDoc->size(myXMLReader.subType()), aDocRoot);
              break;
            }
          }
        }
      }
    }
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    std::set<ObjectPtr>::const_iterator aIt;
    std::string aObjType;
    for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
      ObjectPtr aObject = (*aIt);
      QModelIndex aIndex = objectIndex(aObject);
      if (aIndex.isValid())
        emit dataChanged(aIndex, aIndex);
    }
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_ORDER_UPDATED)) {
    std::shared_ptr<ModelAPI_OrderUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_OrderUpdatedMessage>(theMessage);
    DocumentPtr aDoc = aUpdMsg->document();
    std::string aGroup = aUpdMsg->group();

    QModelIndex aParent;
    int aStartId = 0;
    if (aDoc == aRootDoc) {
      // Update a group under root
      if (aGroup == myXMLReader.rootType()) // Update objects under root
        aStartId = foldersCount();
      else // Update objects in folder under root 
        aParent = createIndex(folderId(aGroup), 0, -1);
    } else {
      // Update a sub-document
      if (aGroup == myXMLReader.subType()) {
        // Update sub-document root
        aParent = findDocumentRootIndex(aDoc.get());
        aStartId = foldersCount(aDoc.get());
      } else 
        // update folder in sub-document
        aParent = createIndex(folderId(aGroup, aDoc.get()), 0, aDoc.get());
    }
    int aChildNb = rowCount(aParent);
    rebuildBranch(aStartId, aChildNb - aStartId, aParent);
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENT_CHANGED)) {
    DocumentPtr aDoc = ModelAPI_Session::get()->activeDocument();
    if (aDoc != aRootDoc) {
      QModelIndex aDocRoot = findDocumentRootIndex(aDoc.get());
      if (aDocRoot.isValid())
        emit dataChanged(aDocRoot, aDocRoot);
      else 
        // We have got a new document
        rebuildDataTree();
//#ifdef _DEBUG
//      else
//        Events_Error::send("Problem with Data Model definition of sub-document");
//#endif
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
  beginResetModel();
  endResetModel();
}

//******************************************************
ObjectPtr XGUI_DataModel::object(const QModelIndex& theIndex) const
{
  if (theIndex.internalId() < 0) // this is a folder
    return ObjectPtr();
  ModelAPI_Object* aObj = (ModelAPI_Object*)theIndex.internalPointer();
  if (getSubDocument(aObj)) // the selected index is a folder of sub-document
    return ObjectPtr();

  return aObj->data()->owner();
}

//******************************************************
QModelIndex XGUI_DataModel::objectIndex(const ObjectPtr theObject) const
{
  std::string aType = theObject->groupName();
  DocumentPtr aDoc = theObject->document();
  int aRow = aDoc->index(theObject);
  if (aRow == -1) {
    // it could be a part of complex object
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    if (aFeature.get()) {
      CompositeFeaturePtr aCompFea = ModelAPI_Tools::compositeOwner(aFeature);
      if (aCompFea.get()) {
        for (int i = 0; i < aCompFea->numberOfSubs(true); i++) {
          if (aCompFea->subFeature(i, true) == theObject) {
            aRow = i;
            break;
          }
        }
      }
    } else {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult.get()) {
        ResultCompSolidPtr aCompRes = ModelAPI_Tools::compSolidOwner(aResult);
        if (aCompRes.get()) {
          for (int i = 0; i < aCompRes->numberOfSubs(true); i++) {
            if (aCompRes->subResult(i, true) == theObject) {
              aRow = i;
              break;
            }
          }
        }
      }
    }
    if (aRow == -1)
      return QModelIndex();
    else 
      return createIndex(aRow, 0, theObject.get());
  }
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if (aDoc == aRootDoc && myXMLReader.rootType() == aType) { 
    // The object from root document
    aRow += foldersCount();
  } else if (myXMLReader.subType() == aType) { 
    // The object from sub document
    aRow += foldersCount(aDoc.get());
  }
  return createIndex(aRow, 0, theObject.get());
}

//******************************************************
QVariant XGUI_DataModel::data(const QModelIndex& theIndex, int theRole) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  int aNbFolders = foldersCount();
  int theIndexRow = theIndex.row();

  if ((theRole == Qt::DecorationRole) && (theIndex == lastHistoryIndex()))
    return QIcon(":pictures/arrow.png");

  if (theIndex.column() == 1)
    return QVariant();

  int aParentId = theIndex.internalId();
  if (aParentId == -1) { // root folders
    switch (theRole) {
      case Qt::DisplayRole:
        return QString(myXMLReader.rootFolderName(theIndexRow).c_str()) + 
          QString(" (%1)").arg(rowCount(theIndex));
      case Qt::DecorationRole:
        return ModuleBase_IconFactory::loadIcon(myXMLReader.rootFolderIcon(theIndexRow).c_str());
      case Qt::ForegroundRole:
        {
          Qt::ItemFlags aFlags = theIndex.flags();
          if (aFlags == Qt::ItemFlags())
            return QBrush(DISABLED_COLOR);
          if (!aFlags.testFlag(Qt::ItemIsEditable))
            return QBrush(SELECTABLE_COLOR);
        }
        return ACTIVE_COLOR;
    }
  } else { // an object or sub-document
    if (theRole == Qt::ForegroundRole) {
      Qt::ItemFlags aFlags = theIndex.flags();
      if (aFlags == Qt::ItemFlags())
        return QBrush(DISABLED_COLOR);
      if (!aFlags.testFlag(Qt::ItemIsEditable))
        return QBrush(SELECTABLE_COLOR);
      return ACTIVE_COLOR;
    }

    ModelAPI_Document* aSubDoc = getSubDocument(theIndex.internalPointer());
    if (aSubDoc) { // this is a folder of sub document
      QIntList aMissedIdx = missedFolderIndexes(aSubDoc);
      int aRow = theIndexRow;
      while (aMissedIdx.contains(aRow)) 
        aRow++;

      switch (theRole) {
        case Qt::DisplayRole:
          return QString(myXMLReader.subFolderName(aRow).c_str()) + 
            QString(" (%1)").arg(rowCount(theIndex));
        case Qt::DecorationRole:
          return ModuleBase_IconFactory::loadIcon(myXMLReader.subFolderIcon(aRow).c_str());
      }
    } else {
      ModelAPI_Object* aObj = (ModelAPI_Object*)theIndex.internalPointer();
      switch (theRole) {
      case Qt::DisplayRole:
        {
          if (aObj->groupName() == ModelAPI_ResultParameter::group()) {
            ModelAPI_ResultParameter* aParam = dynamic_cast<ModelAPI_ResultParameter*>(aObj);
            AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
            QString aVal = QString::number(aValueAttribute->value());
            QString aTitle = QString(aObj->data()->name().c_str());
            return aTitle + " = " + aVal;
          }
          QString aSuffix;
          if (aObj->groupName() == myXMLReader.subType()) {
            ResultPartPtr aPartRes = getPartResult(aObj);
            if (aPartRes.get()) {
              if (aPartRes->partDoc().get() == NULL)
                aSuffix = " (Not loaded)";
            }
          }
          return aObj->data()->name().c_str() + aSuffix;
        }
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
    int aNbFolders = foldersCount();
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
    return aRootDoc->size(aType);
  } else {
    // It is an object which could have children
    ModelAPI_Document* aDoc = getSubDocument(theParent.internalPointer());
    if (aDoc) { 
      // a folder of sub-document
      QIntList aMissedIdx = missedFolderIndexes(aDoc);
      int aRow = theParent.row();
      while (aMissedIdx.contains(aRow)) 
        aRow++;
      std::string aType = myXMLReader.subFolderType(aRow);
      return aDoc->size(aType);
    } else {
      ModelAPI_Object* aObj = (ModelAPI_Object*)theParent.internalPointer();
      // Check for Part feature
      ResultPartPtr aPartRes = getPartResult(aObj);
      if (aPartRes.get()) {
        DocumentPtr aSubDoc = aPartRes->partDoc();
        if (!aSubDoc.get())
          return 0;

        int aNbSubFolders = foldersCount(aSubDoc.get());
        int aNbSubItems = 0;
        std::string aSubType = myXMLReader.subType();
        if (!aSubType.empty())
          aNbSubItems = aSubDoc->size(aSubType);
        return aNbSubItems + aNbSubFolders;
      } else {
        // Check for composite object
        ModelAPI_CompositeFeature* aCompFeature = dynamic_cast<ModelAPI_CompositeFeature*>(aObj);
        if (aCompFeature) 
          return aCompFeature->numberOfSubs(true);
        ModelAPI_ResultCompSolid* aCompRes = dynamic_cast<ModelAPI_ResultCompSolid*>(aObj);
        if (aCompRes) 
          return aCompRes->numberOfSubs(true);
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
  int aNbFolders = foldersCount();

  QModelIndex aIndex;

  if (!theParent.isValid()) {
    if (theRow < aNbFolders) // Return first level folder index
      return createIndex(theRow, theColumn, -1);
    else { // return object under root index
      std::string aType = myXMLReader.rootType();
      int aObjId = theRow - aNbFolders;
      if (aObjId < aRootDoc->size(aType)) {
        ObjectPtr aObj = aRootDoc->object(aType, aObjId);
        aIndex = objectIndex(aObj);
      }
    }
  } else {
    int aId = theParent.internalId();
    int aParentPos = theParent.row();
    if (aId == -1) { // return object index inside of first level of folders
      std::string aType = myXMLReader.rootFolderType(aParentPos);
      if (theRow < aRootDoc->size(aType)) {
        ObjectPtr aObj = aRootDoc->object(aType, theRow);
        aIndex = objectIndex(aObj);
      }
    } else {
      // It is an object which could have children
      ModelAPI_Document* aDoc = getSubDocument(theParent.internalPointer());
      if (aDoc) { 
        // It is a folder of sub-document
        int aParentRow = aParentPos;
        QIntList aMissedIdx = missedFolderIndexes(aDoc);
        while (aMissedIdx.contains(aParentRow))
          aParentRow++;
        std::string aType = myXMLReader.subFolderType(aParentRow);
        if (theRow < aDoc->size(aType)) {
          ObjectPtr aObj = aDoc->object(aType, theRow);
          aIndex = objectIndex(aObj);
        }
      } else {
        ModelAPI_Object* aParentObj = (ModelAPI_Object*)theParent.internalPointer();

        // Check for Part feature
        ResultPartPtr aPartRes = getPartResult(aParentObj);
        if (aPartRes.get()) {
          DocumentPtr aSubDoc = aPartRes->partDoc();
          int aNbSubFolders = foldersCount(aSubDoc.get());
          if (theRow < aNbSubFolders) { // Create a Folder of sub-document
            aIndex = createIndex(theRow, theColumn, aSubDoc.get());
          } else {
            // this is an object under sub document root
            std::string aType = myXMLReader.subType();
            ObjectPtr aObj = aSubDoc->object(aType, theRow - aNbSubFolders);
            aIndex = objectIndex(aObj);
          }
        } else {
          // Check for composite object
          ModelAPI_CompositeFeature* aCompFeature = dynamic_cast<ModelAPI_CompositeFeature*>(aParentObj);
          if (aCompFeature) {
            aIndex = objectIndex(aCompFeature->subFeature(theRow));
          } else {
            ModelAPI_ResultCompSolid* aCompRes = dynamic_cast<ModelAPI_ResultCompSolid*>(aParentObj);
            if (aCompRes) 
              aIndex = objectIndex(aCompRes->subResult(theRow));
          }
        }
      }
    }
  }
  if (theColumn != 0)
    return createIndex(aIndex.row(), theColumn, aIndex.internalPointer());
  return aIndex;
}

//******************************************************
static QModelIndex MYLastDeleted;
QModelIndex XGUI_DataModel::parent(const QModelIndex& theIndex) const
{
  // To avoid additional request about index which was already deleted
  if (theIndex == MYLastDeleted)
    return QModelIndex();

  int aId = theIndex.internalId();
  if (aId != -1) { // The object is not a root folder
    ModelAPI_Document* aDoc = getSubDocument(theIndex.internalPointer());
    if (aDoc) { 
      // It is a folder of sub-document
      return findDocumentRootIndex(aDoc);
    }
    ObjectPtr aObj = object(theIndex);
    if (!aObj.get()) {
      // To avoid additional request about index which was already deleted
      // If deleted it causes a crash on delete object from Part
      MYLastDeleted = theIndex;
      return QModelIndex();
    }
    // Check is it object a sub-object of a complex object
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aFeature.get()) {
      CompositeFeaturePtr aCompFea = ModelAPI_Tools::compositeOwner(aFeature);
      if (aCompFea.get()) {
        return objectIndex(aCompFea);
      }
    }
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aResult.get()) {
      ResultCompSolidPtr aCompRes = ModelAPI_Tools::compSolidOwner(aResult);
      if (aCompRes.get()) {
        return objectIndex(aCompRes);
      }
    }
    // Use as ordinary object
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
  return rowCount(theParent) > 0;
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
  qint64 aIt = theIndex.internalId();
  ModelAPI_Object* aObj = 0;
  ModelAPI_Document* aDoc = 0;
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aActiveDoc = aSession->activeDocument();

  Qt::ItemFlags aNullFlag;
  Qt::ItemFlags aDefaultFlag = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  Qt::ItemFlags aEditingFlag = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;


  if (aIt == -1) {
    // Folders under root
    DocumentPtr aRootDoc = aSession->moduleDocument();
    if (aRootDoc != aActiveDoc)
      return aDefaultFlag;
  } else {
    aDoc = getSubDocument(theIndex.internalPointer());
    if (!aDoc)
      aObj = (ModelAPI_Object*) theIndex.internalPointer();
  }

  if (aObj) {
    // An object
    if (aObj->isDisabled()) 
      return theIndex.column() == 1? Qt::ItemIsSelectable : aNullFlag;
    
    bool isCompositeSub = false;
    // An object which is sub-object of a composite object can not be accessible in column 1
    if (theIndex.column() == 1) {
      ObjectPtr aObjPtr = aObj->data()->owner();
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObjPtr);
      if (aFeature.get()) {
        CompositeFeaturePtr aCompFea = ModelAPI_Tools::compositeOwner(aFeature);
        if (aCompFea.get()) 
          isCompositeSub = true;
      } else {
        ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObjPtr);
        if (aResult.get()) {
          ResultCompSolidPtr aCompRes = ModelAPI_Tools::compSolidOwner(aResult);
          if (aCompRes.get()) 
            isCompositeSub = true;
        }
      }
    }
    if (isCompositeSub)
      return Qt::ItemIsSelectable;

    if (aObj->document() != aActiveDoc) {
      // The object could be a root of sub-tree
      ResultPartPtr aPartRes = getPartResult(aObj);
      if (aPartRes.get()) {
        if (aPartRes->partDoc() == aActiveDoc)
          return aEditingFlag;
      }
      return aDefaultFlag;
    }
  } else if (aDoc) {
    // A folder under sub-document
    if (aActiveDoc.get() != aDoc)
      return aNullFlag;
  }
  return aEditingFlag;
}

//******************************************************
QModelIndex XGUI_DataModel::findDocumentRootIndex(const ModelAPI_Document* theDoc) const
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
        if (myXMLReader.rootType() == ModelAPI_Feature::group()) {
          aRow += foldersCount();
        }
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
          aRow += foldersCount();
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

//******************************************************
int XGUI_DataModel::foldersCount(ModelAPI_Document* theDoc) const
{
  int aNb = 0;
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if ((theDoc == 0) || (theDoc == aRootDoc.get())) {
    for (int i = 0; i < myXMLReader.rootFoldersNumber(); i++) {
      if (myXMLReader.rootShowEmpty(i))
        aNb++;
      else {
        if (aRootDoc->size(myXMLReader.rootFolderType(i)) > 0)
          aNb++;
      }
    }
  } else {
    for (int i = 0; i < myXMLReader.subFoldersNumber(); i++) {
      if (myXMLReader.subShowEmpty(i))
        aNb++;
      else {
        if (theDoc->size(myXMLReader.subFolderType(i)) > 0)
          aNb++;
      }
    }
  }
  return aNb;
}


//******************************************************
QIntList XGUI_DataModel::missedFolderIndexes(ModelAPI_Document* theDoc) const
{
  QIntList aList;
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aSession->moduleDocument();
  if ((theDoc == 0) || (theDoc == aRootDoc.get())) {
    for (int i = 0; i < myXMLReader.rootFoldersNumber(); i++) {
      if (!myXMLReader.rootShowEmpty(i)) {
        if (aRootDoc->size(myXMLReader.rootFolderType(i)) == 0)
          aList.append(i);
      }
    }
  } else {
    for (int i = 0; i < myXMLReader.subFoldersNumber(); i++) {
      if (!myXMLReader.subShowEmpty(i)) {
        if (theDoc->size(myXMLReader.subFolderType(i)) == 0)
          aList.append(i);
      }
    }
  }
  return aList;
}


//******************************************************
QStringList XGUI_DataModel::listOfShowNotEmptyFolders(bool fromRoot) const
{
  QStringList aResult;
  if (fromRoot) {
    for (int i = 0; i < myXMLReader.rootFoldersNumber(); i++) {
      if (!myXMLReader.rootShowEmpty(i))
        aResult << myXMLReader.rootFolderType(i).c_str();
    }
  } else {
    for (int i = 0; i < myXMLReader.subFoldersNumber(); i++) {
      if (!myXMLReader.subShowEmpty(i))
        aResult << myXMLReader.subFolderType(i).c_str();
    }
  }
  return aResult;
}

//******************************************************
QModelIndex XGUI_DataModel::lastHistoryIndex() const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aCurDoc = aSession->activeDocument();
  FeaturePtr aFeature = aCurDoc->currentFeature(true);
  if (aFeature.get()) {
    QModelIndex aInd = objectIndex(aFeature);
    return createIndex(aInd.row(), 1, aInd.internalPointer());
  } else {
    if (aCurDoc == aSession->moduleDocument())
      return createIndex(foldersCount() - 1, 1, -1);
    else 
      return createIndex(foldersCount(aCurDoc.get()) - 1, 1, aCurDoc.get());
  }
}

//******************************************************
int XGUI_DataModel::folderId(std::string theType, ModelAPI_Document* theDoc)
{
  SessionPtr aSession = ModelAPI_Session::get();
  ModelAPI_Document* aDoc = theDoc;
  if (aDoc == 0)
    aDoc = aSession->moduleDocument().get();

  bool aUseSubDoc = (aDoc != aSession->moduleDocument().get());

  int aRes = -1;
  if (aUseSubDoc) {
    int aId = myXMLReader.subFolderId(theType);
    aRes = aId;
    for (int i = 0; i < aId; i++) {
      if (!myXMLReader.subShowEmpty(i)) {
        if (aDoc->size(myXMLReader.subFolderType(i)) == 0)
          aRes--;
      }
    }
  } else {
    int aId = myXMLReader.rootFolderId(theType);
    aRes = aId;
    for (int i = 0; i < aId; i++) {
      if (!myXMLReader.rootShowEmpty(i)) {
        if (aDoc->size(myXMLReader.rootFolderType(i)) == 0)
          aRes--;
      }
    }
  }
  return aRes;
}

//******************************************************
void XGUI_DataModel::rebuildBranch(int theRow, int theCount, const QModelIndex& theParent)
{
  if (theCount > 0) {
    removeRows(theRow, theCount, theParent);
    insertRows(theRow, theCount, theParent);
  }
}
