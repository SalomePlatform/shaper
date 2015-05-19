// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "PartSet_DocumentDataModel.h"
#include "PartSet_PartDataModel.h"
#include "PartSet_Module.h"
//#include "XGUI_Tools.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>

#include <Events_Loop.h>

#include <Config_FeatureMessage.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ActionInfo.h>

#include <PartSetPlugin_Part.h>

#include <QIcon>
#include <QString>
#include <QBrush>
#include <QTreeView>

#include <set>

#define ACTIVE_COLOR QColor(0,72,140)
#define PASSIVE_COLOR Qt::black

QMap<QString, QString> PartSet_DocumentDataModel::myIcons;


PartSet_DocumentDataModel::PartSet_DocumentDataModel(QObject* theParent)
    : ModuleBase_IDocumentDataModel(theParent),
      myActivePartModel(0)
{
  // Create a top part of data tree model
  myModel = new PartSet_TopDataModel(this);
  myModel->setItemsColor(ACTIVE_COLOR);

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->registerListener(this, Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
}

PartSet_DocumentDataModel::~PartSet_DocumentDataModel()
{
  clearModelIndexes();
  clearSubModels();
}

void PartSet_DocumentDataModel::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();


  // Created object event *******************
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aObjects = aUpdMsg->objects();

    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
      ObjectPtr aObject = (*aIt);
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      if (aFeature && (!aFeature->isInHistory()))
        continue;

      DocumentPtr aDoc = aObject->document();
      if (aDoc == aRootDoc) {  // If root objects
        if (aObject->groupName() == ModelAPI_ResultPart::group()) {  // Update only Parts group
            // Add a new part
          int aStart = aRootDoc->size(ModelAPI_ResultPart::group());
          if (aStart > 0) {
            FeaturePtr aPartFeature = ModelAPI_Feature::feature(aObject);
            PartSet_PartDataModel* aModel = new PartSet_PartDataModel(this);
            int anId = aRootDoc->index(aPartFeature);
            aModel->setPart(aPartFeature);
            myPartModels.append(aModel);
            insertRow(aStart, partFolderNode(0));
          }
        } else {  // Update top groups (other except parts
          QModelIndex aIndex = myModel->findParent(aObject);
          int aStart = myModel->rowCount(aIndex) - 1;
          if (aStart < 0)
            aStart = 0;
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          insertRow(aStart, aIndex);
        }
      } else {  // if sub-objects of first level nodes
        PartSet_PartModel* aPartModel = 0;
        foreach (PartSet_PartModel* aPart, myPartModels) {
          if (aPart->hasDocument(aDoc)) {
            aPartModel = aPart;
            break;
          }
        }
        if (aPartModel) {
          QModelIndex aIndex = aPartModel->findParent(aObject);
          int aStart = aPartModel->rowCount(aIndex);  // check this index
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          insertRow(aStart, aIndex);
        } else
          reset();
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
        if (aGroup == ModelAPI_ResultPart::group()) {  // Update only Parts group
          PartSet_PartModel* aDelPartModel = 0;
          foreach (PartSet_PartModel* aPartModel, myPartModels) {
            if (aPartModel->position() == -1) {
              aDelPartModel = aPartModel;
              break;
            }
          }
          if (aDelPartModel) {
            deactivatePart();
            int aStart = myPartModels.size() - 1;
            removeSubModel(aDelPartModel);
            removeRow(aStart, partFolderNode(0));
          }
        } else {  // Update top groups (other except parts
          QModelIndex aIndex = myModel->findGroup(aGroup);
          int aStart = myModel->rowCount(aIndex);
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          removeRow(aStart, aIndex);
        }
      } else {
        PartSet_PartModel* aPartModel = 0;
        foreach (PartSet_PartModel* aPart, myPartModels) {
          if (aPart->hasDocument(aDoc)) {
            aPartModel = aPart;
            break;
          }
        }
        if (aPartModel) {
          QModelIndex aIndex = aPartModel->findGroup(aGroup);
          int aStart = aPartModel->rowCount(aIndex);
          aIndex = createIndex(aIndex.row(), aIndex.column(), (void*) getModelIndex(aIndex));
          removeRow(aStart, aIndex);
        }
      }
    }
    // Deleted object event ***********************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    //std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg = std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    //ObjectPtr aFeature = aUpdMsg->feature();
    //DocumentPtr aDoc = aFeature->document();

    // TODO: Identify the necessary index by the modified feature
    QModelIndex aIndex;
    emit dataChanged(aIndex, aIndex);

    // Reset whole tree **************************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(Config_FeatureMessage::GUI_EVENT())) {
    std::shared_ptr<Config_FeatureMessage> aFeatureMsg =
       std::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (!aFeatureMsg->isInternal()) {
      ActionInfo aFeatureInfo;
      aFeatureInfo.initFrom(aFeatureMsg);
      // Remember features icons
      myIcons[QString::fromStdString(aFeatureMsg->id())] = aFeatureInfo.iconFile;
    }
  } else {
    rebuildDataTree();
  }
}

void PartSet_DocumentDataModel::rebuildDataTree()
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();

  beginResetModel();
  clearModelIndexes();

  // Delete extra models
  ObjectPtr aObj;
  FeaturePtr aFeature;
  QList<PartSet_PartModel*> aDelList;
  foreach (PartSet_PartModel* aPartModel, myPartModels) {
    if (aPartModel->position() == -1) 
      aDelList.append(aPartModel);
  }
  foreach (PartSet_PartModel* aPartModel, aDelList) {
    removeSubModel(aPartModel);
  }
  // Add non existing models
  int aHistNb = aRootDoc->size(ModelAPI_Feature::group());
  for (int i = 0; i < aHistNb; i++) {
    aObj = aRootDoc->object(ModelAPI_Feature::group(), i);
    aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aFeature->getKind() == PartSetPlugin_Part::ID()) {
      if (!findPartModel(aFeature)) {
        PartSet_PartDataModel* aModel = new PartSet_PartDataModel(this);
        aModel->setPart(aFeature);
        myPartModels.append(aModel);
      }
    }
  }
  endResetModel();
}

QVariant PartSet_DocumentDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (!theIndex.isValid())
    return QVariant();

  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  QModelIndex aParent = theIndex.parent();
  if ((theIndex.column() == 1) ) {
    if ((theIndex.internalId() >= PartsFolder) && (theIndex.internalId() <= PartResult)) {
      if (ModelAPI_Session::get()->activeDocument() == aRootDoc) {
        if (!aParent.isValid()) {
          switch (theRole) {
          case Qt::DecorationRole:
            if (theIndex.row() == lastHistoryRow())
              return QIcon(":pictures/arrow.png");
          }
        }
      }
    } else {
      QModelIndex* aIndex = toSourceModelIndex(theIndex);
      const QAbstractItemModel* aModel = aIndex->model();
      if (isPartSubModel(aModel)) {
        return aModel->data(*aIndex, theRole);
      }
    }
    return QVariant();
  }

  switch (theIndex.internalId()) {
    case PartsFolder:
      switch (theRole) {
        case Qt::DisplayRole:
          return tr("Parts") + QString(" (%1)").arg(rowCount(theIndex));
        case Qt::DecorationRole:
          return QIcon(":pictures/constr_folder.png");
        case Qt::ToolTipRole:
          return tr("Parts folder");
        case Qt::ForegroundRole:
          if (myActivePartIndex.isValid())
              return QBrush(PASSIVE_COLOR);
            else
              return QBrush(ACTIVE_COLOR);
        default:
          return QVariant();
      }
      break;
    case HistoryNode:
      {
        int aOffset = historyOffset();
        ObjectPtr aObj = aRootDoc->object(ModelAPI_Feature::group(), theIndex.row() - aOffset);
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
        if (!aFeature)
          return QVariant();
        switch (theRole) {
          case Qt::DisplayRole:
            if (aFeature)
              return aFeature->data()->name().c_str();
            else
              return QVariant();
          case Qt::DecorationRole:
            return featureIcon(aFeature);
          case Qt::ToolTipRole:
            return tr("Feature object");
          case Qt::ForegroundRole:
            if (theIndex.row() > lastHistoryRow())
              return QBrush(Qt::lightGray);
            else {
              if (myActivePartIndex.isValid())
                return QBrush(PASSIVE_COLOR);
              else
                return QBrush(ACTIVE_COLOR);
            }
          default:
            return QVariant();
        }
      }
      break;
    case PartResult:
      {
        ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), theIndex.row());
        if (aObject) {
          switch (theRole) {
            case Qt::DisplayRole:
              return std::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
            case Qt::DecorationRole:
              return QIcon(":pictures/part_ico.png");
            case Qt::ForegroundRole:
              {
                if (theIndex == myActivePartIndex)
                  return QBrush(ACTIVE_COLOR);
                else
                  return QBrush(PASSIVE_COLOR);
              }
            default:
              return QVariant();
          }
        }
      }
      break;
  }
  if (aParent.internalId() == HistoryNode) {
    int aId = aParent.row() - historyOffset();
    QModelIndex* aIndex = toSourceModelIndex(theIndex);
    return findPartModel(aId)->data(*aIndex, theRole);
  }
  return toSourceModelIndex(theIndex)->data(theRole);
}

QVariant PartSet_DocumentDataModel::headerData(int theSection, Qt::Orientation theOrient,
                                            int theRole) const
{
  return QVariant();
}

int PartSet_DocumentDataModel::rowCount(const QModelIndex& theParent) const
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  if (!theParent.isValid()) {
    // Size of external models
    int aVal = historyOffset();
    // Plus history size
    aVal += aRootDoc->size(ModelAPI_Feature::group());
    return aVal;
  }
  if (theParent.internalId() == PartsFolder) {
    return aRootDoc->size(ModelAPI_ResultPart::group());
    //int aSize = myPartModels.size();
    //return myPartModels.size();
  }
  if (theParent.internalId() == HistoryNode) {
    int aId = theParent.row() - historyOffset();
    PartSet_PartModel* aModel = findPartModel(aId);
    if (aModel)
      return aModel->rowCount(QModelIndex());
    return 0;
  }
  if (theParent.internalId() == PartResult)
    return 0;
 
  QModelIndex* aParent = toSourceModelIndex(theParent);
  const QAbstractItemModel* aModel = aParent->model();
  if (!isSubModel(aModel))
    return 0;

  /*if (isPartSubModel(aModel)) {
   if (aModel != myActivePart)
   return 0;
   }*/
  return aModel->rowCount(*aParent);
}

int PartSet_DocumentDataModel::columnCount(const QModelIndex& theParent) const
{
  return 2;
}

QModelIndex PartSet_DocumentDataModel::index(int theRow, int theColumn,
                                          const QModelIndex& theParent) const
{
  QModelIndex aIndex;
  if (!theParent.isValid()) {
    int aOffs = myModel->rowCount();
    if (theRow < aOffs) {
      aIndex = myModel->index(theRow, theColumn, theParent);
      aIndex = createIndex(theRow, theColumn, (void*) getModelIndex(aIndex));
    } else {
      if (theRow == aOffs)  // Create Parts node
        aIndex = partFolderNode(theColumn);
      else {
        // create history node
        aIndex = createIndex(theRow, theColumn, HistoryNode);
      }
    }
  } else {
    if (theParent.internalId() == PartsFolder) {
       aIndex = createIndex(theRow, theColumn, PartResult);
    } else { 
      if (theParent.internalId() == HistoryNode) {
        int aId = theParent.row() - historyOffset();
        aIndex = findPartModel(aId)->index(theRow, theColumn, QModelIndex());
      } else {
        QModelIndex* aParent = (QModelIndex*) theParent.internalPointer();
        aIndex = aParent->model()->index(theRow, theColumn, (*aParent));
      }
      aIndex = createIndex(theRow, theColumn, (void*) getModelIndex(aIndex));
    }
  }
  return aIndex;
}

QModelIndex PartSet_DocumentDataModel::parent(const QModelIndex& theIndex) const
{
  if ((theIndex.internalId() == PartsFolder) || (theIndex.internalId() == HistoryNode))
    return QModelIndex();

  if (theIndex.internalId() == PartResult)
    return partFolderNode(0);

  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  const QAbstractItemModel* aModel = aIndex->model();
  if (!isSubModel(aModel))
    return QModelIndex();

  QModelIndex aIndex1 = aModel->parent(*aIndex);
  const PartSet_PartModel* aPartModel = dynamic_cast<const PartSet_PartModel*>(aModel);
  if (aPartModel && (!aIndex1.isValid())) {
    int aId = aPartModel->position();
    int aRow = aId + historyOffset();
    return createIndex(aRow, 0, (qint32) HistoryNode);
  }

  if (aIndex1.isValid())
    return createIndex(aIndex1.row(), 0, (void*) getModelIndex(aIndex1));
  return aIndex1;
}

bool PartSet_DocumentDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;
  return rowCount(theParent) > 0;
}

QModelIndex* PartSet_DocumentDataModel::toSourceModelIndex(const QModelIndex& theProxy) const
{
  QModelIndex* aIndexPtr = static_cast<QModelIndex*>(theProxy.internalPointer());
  return aIndexPtr;
}

QModelIndex* PartSet_DocumentDataModel::findModelIndex(const QModelIndex& theIndex) const
{
  QList<QModelIndex*>::const_iterator aIt;
  for (aIt = myIndexes.constBegin(); aIt != myIndexes.constEnd(); ++aIt) {
    QModelIndex* aIndex = (*aIt);
    if ((*aIndex) == theIndex)
      return aIndex;
  }
  return 0;
}

QModelIndex* PartSet_DocumentDataModel::getModelIndex(const QModelIndex& theIndex) const
{
  QModelIndex* aIndexPtr = findModelIndex(theIndex);
  if (!aIndexPtr) {
    aIndexPtr = new QModelIndex(theIndex);
    PartSet_DocumentDataModel* that = (PartSet_DocumentDataModel*) this;
    that->myIndexes.append(aIndexPtr);
  }
  return aIndexPtr;
}

void PartSet_DocumentDataModel::clearModelIndexes()
{
  foreach (QModelIndex* aIndex, myIndexes) 
    delete aIndex;
  myIndexes.clear();
}

void PartSet_DocumentDataModel::clearSubModels()
{
  foreach (PartSet_PartModel* aPart, myPartModels) 
    delete aPart;
  myPartModels.clear();
}

ObjectPtr PartSet_DocumentDataModel::object(const QModelIndex& theIndex) const
{
  if (theIndex.internalId() == PartsFolder)
    return ObjectPtr();
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  if (theIndex.internalId() == HistoryNode) {
    int aOffset = historyOffset();
    return aRootDoc->object(ModelAPI_Feature::group(), theIndex.row() - aOffset);
  }
  if (theIndex.internalId() == PartResult) {
    return aRootDoc->object(ModelAPI_ResultPart::group(), theIndex.row());
  }
  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  if (!isSubModel(aIndex->model()))
    return ObjectPtr();

  const PartSet_FeaturesModel* aModel = dynamic_cast<const PartSet_FeaturesModel*>(aIndex->model());
  return aModel->object(*aIndex);
}

bool PartSet_DocumentDataModel::insertRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginInsertRows(theParent, theRow, theRow + theCount - 1);
  //endInsertRows();

  // Update history
  QModelIndex aRoot;
  int aRow = rowCount(aRoot);
  beginInsertRows(aRoot, aRow, aRow);
  endInsertRows();

  return true;
}

bool PartSet_DocumentDataModel::removeRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginRemoveRows(theParent, theRow, theRow + theCount - 1);
  endRemoveRows();
  return true;
}

void PartSet_DocumentDataModel::removeSubModel(int theModelId)
{
  PartSet_PartModel* aModel = myPartModels.at(theModelId);
  removeSubModel(aModel);
}

void PartSet_DocumentDataModel::removeSubModel(PartSet_PartModel* theModel)
{
  QIntList aToRemove;
  for (int i = 0; i < myIndexes.size(); i++) {
    if (myIndexes.at(i)->model() == theModel)
      aToRemove.append(i);
  }
  int aId;
  while (aToRemove.size() > 0) {
    aId = aToRemove.last();
    delete myIndexes.at(aId);
    myIndexes.removeAt(aId);
    aToRemove.removeLast();
  }
  delete theModel;
  myPartModels.removeAll(theModel);
}


bool PartSet_DocumentDataModel::isSubModel(const QAbstractItemModel* theModel) const
{
  if (theModel == myModel)
    return true;
  return isPartSubModel(theModel);
}

bool PartSet_DocumentDataModel::isPartSubModel(const QAbstractItemModel* theModel) const
{
  return myPartModels.contains((PartSet_PartModel*) theModel);
}

QModelIndex PartSet_DocumentDataModel::partFolderNode(int theColumn) const
{
  int aPos = myModel->rowCount(QModelIndex());
  return createIndex(aPos, theColumn, PartsFolder);
}

int PartSet_DocumentDataModel::historyOffset() const
{
  // Nb of rows of top model + Parts folder
  return myModel->rowCount(QModelIndex()) + 1;
}

bool PartSet_DocumentDataModel::activatePart(const QModelIndex& theIndex)
{
  if ((theIndex.internalId() == PartsFolder) || (theIndex.internalId() == HistoryNode))
    return false;

  if (theIndex.isValid() && (theIndex.internalId() == PartResult)) {
    myActivePartIndex = theIndex;
    myModel->setItemsColor(PASSIVE_COLOR);
    if (myActivePartModel) 
      myActivePartModel->setItemsColor(PASSIVE_COLOR);
    
    // Find activated part feature by its ID
    ResultPartPtr aPartRes = activePart();
    FeaturePtr aFeature = ModelAPI_Feature::feature(aPartRes);
    if (aFeature.get()) {
      myActivePartModel = findPartModel(aFeature);
      myActivePartModel->setItemsColor(ACTIVE_COLOR);
    }
  } 
  return true;
}

ResultPartPtr PartSet_DocumentDataModel::activePart() const
{
  if (myActivePartIndex.isValid()) {
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    ObjectPtr aObj = aRootDoc->object(ModelAPI_ResultPart::group(), myActivePartIndex.row());
    return std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
  }
  return ResultPartPtr();
}

QModelIndex PartSet_DocumentDataModel::activePartTree() const
{
  if (myActivePartModel) {
    return createIndex(myActivePartModel->position() + historyOffset(), 0, HistoryNode);
  }
  return QModelIndex();
}

void PartSet_DocumentDataModel::deactivatePart()
{
  if (myActivePartIndex.isValid()) {
    if (myActivePartModel) 
      myActivePartModel->setItemsColor(PASSIVE_COLOR);
    myActivePartModel = 0;
    myActivePartIndex = QModelIndex();
    myModel->setItemsColor(ACTIVE_COLOR);
  }
}

Qt::ItemFlags PartSet_DocumentDataModel::flags(const QModelIndex& theIndex) const
{
  if ((theIndex.internalId() >= PartsFolder) && (theIndex.internalId() <= PartResult)) {
    Qt::ItemFlags aFlags = Qt::ItemIsSelectable;
    if (object(theIndex)) {
      aFlags |= Qt::ItemIsEditable;
    }
    // Disable items which are below of last history row
    // Do not disable second column
    if (theIndex.internalId() == HistoryNode) {
      if (theIndex.row() <= lastHistoryRow() || (theIndex.column() == 1))
        aFlags |= Qt::ItemIsEnabled;
    } else
      aFlags |= Qt::ItemIsEnabled;
    return aFlags;
  } else {
    QModelIndex* aIndex = toSourceModelIndex(theIndex);
    const QAbstractItemModel* aModel = aIndex->model();
    return aModel->flags(*aIndex);
  }
}

QModelIndex PartSet_DocumentDataModel::partIndex(const ResultPartPtr& theObject) const
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  int aNb = aRootDoc->size(ModelAPI_ResultPart::group());
  for (int aId = 0; aId < aNb; aId++) {
    if (theObject == aRootDoc->object(ModelAPI_ResultPart::group(), aId))
      return createIndex(aId, 0, PartResult);
  }
  return QModelIndex();
}

QModelIndex PartSet_DocumentDataModel::objectIndex(const ObjectPtr theObject) const
{
  // Check that this feature belongs to root document
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  DocumentPtr aDoc = theObject->document();
  if (aDoc == aRootDoc) {
    // This feature belongs to histrory or top model
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    if (aFeature) {
      int aId;
      int aNb = aRootDoc->size(ModelAPI_Feature::group());
      for (aId = 0; aId < aNb; aId++) {
        if (theObject == aRootDoc->object(ModelAPI_Feature::group(), aId))
          break;
      }
      if (aId < aNb)
        return index(aId + historyOffset(), 0, QModelIndex());
    } else {
      QModelIndex aIndex = myModel->objectIndex(theObject);
      return
          aIndex.isValid() ?
              createIndex(aIndex.row(), 0, (void*) getModelIndex(aIndex)) :
              QModelIndex();
    }
  } else {
    PartSet_PartModel* aPartModel = 0;
    foreach(PartSet_PartModel* aModel, myPartModels) {
      if (aModel->hasDocument(aDoc)) {
        aPartModel = aModel;
        break;
      }
    }
    if (aPartModel) {
      QModelIndex aIndex = aPartModel->objectIndex(theObject);
      return aIndex.isValid() ?
              createIndex(aIndex.row(), 0, (void*) getModelIndex(aIndex)) :
              QModelIndex();
    }
  }
  return QModelIndex();
}


void PartSet_DocumentDataModel::clear()
{
  clearModelIndexes();
  clearSubModels();
  //myActivePart = 0;
  myActivePartIndex = QModelIndex();
  myModel->setItemsColor(ACTIVE_COLOR);
}

int PartSet_DocumentDataModel::lastHistoryRow() const
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  FeaturePtr aFeature = aRootDoc->currentFeature(true);
  if (aFeature.get())
    return historyOffset() + aRootDoc->index(aFeature);
  else 
    return historyOffset() - 1;
}

void PartSet_DocumentDataModel::setLastHistoryItem(const QModelIndex& theIndex)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aMgr->moduleDocument();
  std::string aOpName = tr("History change").toStdString();
  if (theIndex.internalId() == HistoryNode) {
    ObjectPtr aObject = object(theIndex);
    aMgr->startOperation(aOpName);
    aRootDoc->setCurrentFeature(std::dynamic_pointer_cast<ModelAPI_Feature>(aObject), true);
    aMgr->finishOperation();
  } else {
    aMgr->startOperation(aOpName);
    aRootDoc->setCurrentFeature(FeaturePtr(), true);
    aMgr->finishOperation();
  }
}

QModelIndex PartSet_DocumentDataModel::lastHistoryItem() const
{
  return index(lastHistoryRow(), 1);
}


QIcon PartSet_DocumentDataModel::featureIcon(const FeaturePtr& theFeature)
{
  QIcon anIcon;

  std::string aKind = theFeature->getKind();
  QString aId(aKind.c_str());
  if (!myIcons.contains(aId))
    return anIcon;

  QString anIconString = myIcons[aId];

  ModelAPI_ExecState aState = theFeature->data()->execState();
  switch(aState) {
    case ModelAPI_StateDone:
    case ModelAPI_StateNothing: {
      anIcon = QIcon(anIconString);
    }
    break;
    case ModelAPI_StateMustBeUpdated: {
      anIcon = ModuleBase_Tools::lighter(anIconString);
    }
    break;
    case ModelAPI_StateExecFailed: {
      anIcon = ModuleBase_Tools::composite(":icons/exec_state_failed.png", anIconString);
    }
    break;
    case ModelAPI_StateInvalidArgument: {
      anIcon = ModuleBase_Tools::composite(":icons/exec_state_invalid_parameters.png",
                                           anIconString);
    }
    break;
    default: break;  
  }
  return anIcon;  
}

void PartSet_DocumentDataModel::onMouseDoubleClick(const QModelIndex& theIndex)
{
  if (theIndex.column() != 1)
    return;
  QTreeView* aTreeView = dynamic_cast<QTreeView*>(sender());
  if ((theIndex.internalId() >= PartsFolder) && (theIndex.internalId() <= PartResult)) {
    if (myActivePartModel)
      // It means that the root document is not active
      return;
    QModelIndex aNewIndex;
    if (theIndex.internalId() == HistoryNode) 
      aNewIndex = theIndex;
    int aOldId = lastHistoryRow();
    setLastHistoryItem(theIndex);
    int aStartRow = std::min(aOldId, theIndex.row());
    int aEndRow = std::max(aOldId, theIndex.row());
    for (int i = aStartRow; i <= aEndRow; i++) {
      aTreeView->update(createIndex(i, 0, HistoryNode));
      aTreeView->update(createIndex(i, 1, HistoryNode));
    }
    
  } else {
    QModelIndex* aIndex = toSourceModelIndex(theIndex);
    const QAbstractItemModel* aModel = aIndex->model();
    if (isPartSubModel(aModel)) {
      PartSet_PartDataModel* aPartModel = (PartSet_PartDataModel*)aModel;
      QModelIndex aOldItem = aPartModel->lastHistoryItem();
      aPartModel->setLastHistoryItem(*aIndex);
      QModelIndex aOldIndex = createIndex(aOldItem.row(), aOldItem.column(), (void*) getModelIndex(aOldItem));
      int aStartRow = std::min(aOldItem.row(), aIndex->row());
      int aEndRow = std::max(aOldItem.row(), aIndex->row());
      for (int i = aStartRow; i <= aEndRow; i++) {
        QModelIndex aInd1 = aPartModel->index(i, 0);
        QModelIndex aInd2 = createIndex(i, 0, (void*) getModelIndex(aInd1));
        aTreeView->update(aInd2);
        aInd1 = aPartModel->index(i, 1);
        aInd2 = createIndex(i, 1, (void*) getModelIndex(aInd1));
        aTreeView->update(aInd2);
      }
    }
  }
} 


PartSet_PartModel* PartSet_DocumentDataModel::findPartModel(FeaturePtr thePart) const
{
  foreach (PartSet_PartModel* aModel, myPartModels) {
    if (aModel->part() == thePart)
      return aModel;
  }
  return 0;
}

PartSet_PartModel* PartSet_DocumentDataModel::findPartModel(int thePosition) const
{
  foreach (PartSet_PartModel* aModel, myPartModels) {
    if (aModel->position() == thePosition)
      return aModel;
  }
  return 0;
}