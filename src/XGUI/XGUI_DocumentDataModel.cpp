#include "XGUI_DocumentDataModel.h"
#include "XGUI_PartDataModel.h"
#include "XGUI_Workshop.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <Model_Events.h>

#include <Events_Loop.h>

#include <Config_FeatureMessage.h>

#include <QIcon>
#include <QString>


XGUI_DocumentDataModel::XGUI_DocumentDataModel(QObject* theParent)
  : QAbstractItemModel(theParent), myActivePart(0)
{
  // Find Document object
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  myDocument = aMgr->currentDocument();

  // Register in event loop
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_CREATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_DELETED));

  // Create a top part of data tree model
  myModel = new XGUI_TopDataModel(myDocument, this);
}


XGUI_DocumentDataModel::~XGUI_DocumentDataModel()
{
  clearModelIndexes();
}


void XGUI_DocumentDataModel::processEvent(const Events_Message* theMessage)
{
  // Created object event *******************
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_CREATED)) {
    const Model_FeatureUpdatedMessage* aUpdMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Feature> aFeature = aUpdMsg->feature();
    boost::shared_ptr<ModelAPI_Document> aDoc = aFeature->document();

    if (aDoc == myDocument) {  // If root objects
      if (aFeature->getGroup().compare(PARTS_GROUP) == 0) { // Update only Parts group
        // Add a new part
        int aStart = myPartModels.size() + 1;
        XGUI_PartDataModel* aModel = new XGUI_PartDataModel(myDocument, this);
        aModel->setPartId(myPartModels.count());
        myPartModels.append(aModel);
        insertRow(aStart, partFolderNode());
      } else { // Update top groups (other except parts
        QModelIndex aIndex = myModel->findParent(aFeature);
        int aStart = myModel->rowCount(aIndex) - 1;
        aIndex = createIndex(aIndex.row(), aIndex.column(), (void*)getModelIndex(aIndex));
        insertRow(aStart, aIndex);
      }
    } else { // if sub-objects of first level nodes
      XGUI_PartModel* aPartModel = 0;
      QList<XGUI_PartModel*>::const_iterator aIt;
      for (aIt = myPartModels.constBegin(); aIt != myPartModels.constEnd(); ++aIt) {
        if ((*aIt)->hasDocument(aDoc)) {
          aPartModel = (*aIt);
          break;
        }
      }
      if (aPartModel) {
        QModelIndex aIndex = aPartModel->findParent(aFeature);
        int aStart = aPartModel->rowCount(aIndex) - 1;
        aIndex = createIndex(aIndex.row(), aIndex.column(), (void*)getModelIndex(aIndex));
        insertRow(aStart, aIndex);
      }
    }

  // Deleted object event ***********************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_DELETED)) {
    const Model_FeatureDeletedMessage* aUpdMsg = dynamic_cast<const Model_FeatureDeletedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Document> aDoc = aUpdMsg->document();

    if (aDoc == myDocument) {  // If root objects
      if (aUpdMsg->group().compare(PARTS_GROUP) == 0) { // Updsate only Parts group
        int aStart = myPartModels.size();
        removeSubModel(myPartModels.size() - 1);
        removeRow(aStart - 1, partFolderNode());
      } else { // Update top groups (other except parts
        QModelIndex aIndex = myModel->findGroup(aUpdMsg->group());
        int aStart = myModel->rowCount(aIndex);
        aIndex = createIndex(aIndex.row(), aIndex.column(), (void*)getModelIndex(aIndex));
        removeRow(aStart - 1, aIndex);
      }
    } else {
      XGUI_PartModel* aPartModel = 0;
      QList<XGUI_PartModel*>::const_iterator aIt;
      for (aIt = myPartModels.constBegin(); aIt != myPartModels.constEnd(); ++aIt) {
        if ((*aIt)->hasDocument(aDoc)) {
          aPartModel = (*aIt);
          break;
        }
      }
      if (aPartModel) {
        QModelIndex aIndex = aPartModel->findGroup(aUpdMsg->group());
        int aStart = aPartModel->rowCount(aIndex);
        aIndex = createIndex(aIndex.row(), aIndex.column(), (void*)getModelIndex(aIndex));
        removeRow(aStart - 1, aIndex);
      }
    }

  // Deleted object event ***********************
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_UPDATED)) {
    //const Model_FeatureUpdatedMessage* aUpdMsg = dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);
    //boost::shared_ptr<ModelAPI_Feature> aFeature = aUpdMsg->feature();
    //boost::shared_ptr<ModelAPI_Document> aDoc = aFeature->document();
    
    // TODO: Identify the necessary index by the modified feature
    QModelIndex aIndex;
    emit dataChanged(aIndex, aIndex);

  // Reset whole tree **************************
  } else {  
    beginResetModel();
    int aNbParts = myDocument->size(PARTS_GROUP);
    if (myPartModels.size() != aNbParts) { // resize internal models
      while (myPartModels.size() > aNbParts) {
        delete myPartModels.last();
        myPartModels.removeLast();
      }
      while (myPartModels.size() < aNbParts) {
        myPartModels.append(new XGUI_PartDataModel(myDocument, this));
      }
      for (int i = 0; i < myPartModels.size(); i++)
        myPartModels.at(i)->setPartId(i);
    }
    clearModelIndexes();
    endResetModel();
  }
}

QVariant XGUI_DocumentDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (!theIndex.isValid())
    return QVariant();
  switch (theIndex.internalId()) {
  case PartsFolder:
    switch (theRole) {
    case Qt::DisplayRole:
      return tr("Parts") + QString(" (%1)").arg(rowCount(theIndex));
    case Qt::DecorationRole:
      return QIcon(":pictures/constr_folder.png");
    case Qt::ToolTipRole:
      return tr("Parts folder");
    default:
      return QVariant();
    }
    break;
  case HistoryNode:
    {
      int aOffset = historyOffset();
      FeaturePtr aFeature = myDocument->feature(FEATURES_GROUP, theIndex.row() - aOffset);
      if (!aFeature)
        return QVariant();
      switch (theRole) {
      case Qt::DisplayRole:
        if (aFeature)
          return aFeature->data()->getName().c_str();
        else 
          return QVariant();
      case Qt::DecorationRole:
        return QIcon(XGUI_Workshop::featureIcon(aFeature->getKind()));
      case Qt::ToolTipRole:
        return tr("Feature object");
      default:
        return QVariant();
      }
    }
    break;
  }
  QModelIndex aParent = theIndex.parent();
  if (aParent.isValid() && (aParent.internalId() == PartsFolder)) {
    return myPartModels.at(theIndex.row())->data(QModelIndex(), theRole);
  }
  return toSourceModelIndex(theIndex)->data(theRole);
}


QVariant XGUI_DocumentDataModel::headerData(int theSection, Qt::Orientation theOrient, int theRole) const
{
  return QVariant();
}

int XGUI_DocumentDataModel::rowCount(const QModelIndex& theParent) const
{
  if (!theParent.isValid()) {
    // Size of external models
    int aVal = historyOffset();
    // Plus history size
    aVal += myDocument->size(FEATURES_GROUP);
    return aVal;
  }
  if (theParent.internalId() == PartsFolder) {
    return myPartModels.size();
  }
  if (theParent.internalId() == HistoryNode) {
    return 0;
  }
  QModelIndex* aParent = toSourceModelIndex(theParent);
  const QAbstractItemModel* aModel = aParent->model();
  if (!isSubModel(aModel)) 
    return 0;

  if (isPartSubModel(aModel)) {
    if (aModel != myActivePart)
      return 0;
  }
  return aModel->rowCount(*aParent);
}

int XGUI_DocumentDataModel::columnCount(const QModelIndex& theParent) const
{
  return 1;
}

QModelIndex XGUI_DocumentDataModel::index(int theRow, int theColumn, const QModelIndex& theParent) const
{
  QModelIndex aIndex;
  if (!theParent.isValid()) {
    int aOffs = myModel->rowCount();
    if (theRow < aOffs) {
      aIndex = myModel->index(theRow, theColumn, theParent);
      aIndex = createIndex(theRow, theColumn, (void*)getModelIndex(aIndex));
    } else {
      if (theRow == aOffs)  // Create Parts node
        aIndex = partFolderNode();
      else // create history node
        aIndex = createIndex(theRow, theColumn, HistoryNode);
    }
  } else {
    if (theParent.internalId() == PartsFolder) {
      aIndex = myPartModels.at(theRow)->index(0, theColumn, QModelIndex());
    } else {
      QModelIndex* aParent = (QModelIndex*)theParent.internalPointer();
      aIndex = aParent->model()->index(theRow, theColumn, (*aParent));
    }
    aIndex = createIndex(theRow, theColumn, (void*)getModelIndex(aIndex));
  }
  return aIndex;
}


QModelIndex XGUI_DocumentDataModel::parent(const QModelIndex& theIndex) const
{
  if ((theIndex.internalId() == PartsFolder) || (theIndex.internalId() == HistoryNode))
    return QModelIndex();

  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  const QAbstractItemModel* aModel = aIndex->model();
  if (!isSubModel(aModel)) 
    return QModelIndex();

  if (isPartSubModel(aModel)) {
    if (!aModel->parent(*aIndex).isValid()) {
      return partFolderNode();
    }
  }

  QModelIndex aIndex1 = aModel->parent(*aIndex);
  if (aIndex1.isValid())
    return createIndex(aIndex1.row(), aIndex1.column(), (void*)getModelIndex(aIndex1));
  return aIndex1;
}


bool XGUI_DocumentDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;
  return rowCount(theParent) > 0;
}


QModelIndex* XGUI_DocumentDataModel::toSourceModelIndex(const QModelIndex& theProxy) const
{
  QModelIndex* aIndexPtr = static_cast<QModelIndex*>(theProxy.internalPointer());
  return aIndexPtr;
}


QModelIndex* XGUI_DocumentDataModel::findModelIndex(const QModelIndex& theIndex) const
{
  QList<QModelIndex*>::const_iterator aIt;
  for (aIt = myIndexes.constBegin(); aIt != myIndexes.constEnd(); ++aIt) {
    QModelIndex* aIndex = (*aIt);
    if ((*aIndex) == theIndex)
      return aIndex;
  }
  return 0;
}

QModelIndex* XGUI_DocumentDataModel::getModelIndex(const QModelIndex& theIndex) const
{
  QModelIndex* aIndexPtr = findModelIndex(theIndex);
  if (!aIndexPtr) {
    aIndexPtr = new QModelIndex(theIndex);
    XGUI_DocumentDataModel* that = (XGUI_DocumentDataModel*) this;
    that->myIndexes.append(aIndexPtr);
  }
  return aIndexPtr;
}

void XGUI_DocumentDataModel::clearModelIndexes()
{
  QList<QModelIndex*>::const_iterator aIt;
  for (aIt = myIndexes.constBegin(); aIt != myIndexes.constEnd(); ++aIt) 
    delete (*aIt);
  myIndexes.clear();
}

FeaturePtr XGUI_DocumentDataModel::feature(const QModelIndex& theIndex) const
{
  if (theIndex.internalId() == PartsFolder)
    return FeaturePtr();
  if (theIndex.internalId() == HistoryNode) {
      int aOffset = historyOffset();
      return myDocument->feature(FEATURES_GROUP, theIndex.row() - aOffset);
  }
  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  if (!isSubModel(aIndex->model())) 
    return FeaturePtr();

  const XGUI_FeaturesModel* aModel = dynamic_cast<const XGUI_FeaturesModel*>(aIndex->model());
  return aModel->feature(*aIndex);
}

bool XGUI_DocumentDataModel::insertRows(int theRow, int theCount, const QModelIndex& theParent)
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

bool XGUI_DocumentDataModel::removeRows(int theRow, int theCount, const QModelIndex& theParent)
{
  beginRemoveRows(theParent, theRow, theRow + theCount - 1);
  endRemoveRows();
  return true;
}


void XGUI_DocumentDataModel::removeSubModel(int theModelId)
{
  XGUI_PartModel* aModel = myPartModels.at(theModelId);
  QIntList aToRemove;
  for (int i = 0; i < myIndexes.size(); i++) {
    if (myIndexes.at(i)->model() == aModel)
      aToRemove.append(i);
  }
  int aId;
  while(aToRemove.size() > 0) {
    aId = aToRemove.last();
    delete myIndexes.at(aId);
    myIndexes.removeAt(aId);
    aToRemove.removeLast();
  }
  delete aModel;
  myPartModels.removeAt(theModelId);
}

bool XGUI_DocumentDataModel::isSubModel(const QAbstractItemModel* theModel) const
{
  if (theModel == myModel)
    return true;
  return isPartSubModel(theModel);
}

bool XGUI_DocumentDataModel::isPartSubModel(const QAbstractItemModel* theModel) const
{
  return myPartModels.contains((XGUI_PartModel*)theModel);
}

QModelIndex XGUI_DocumentDataModel::partFolderNode() const
{
  int aPos = myModel->rowCount(QModelIndex());
  return createIndex(aPos, columnCount() - 1, PartsFolder);
}

int XGUI_DocumentDataModel::historyOffset() const
{
  // Nb of rows of top model + Parts folder
  return myModel->rowCount(QModelIndex()) + 1;
}

bool XGUI_DocumentDataModel::activatedIndex(const QModelIndex& theIndex)
{
  if ((theIndex.internalId() == PartsFolder) || (theIndex.internalId() == HistoryNode))
    return false;

  QModelIndex* aIndex = toSourceModelIndex(theIndex);
  if (!aIndex)
    return false;

  const QAbstractItemModel* aModel = aIndex->model();

  if (isPartSubModel(aModel)) {
    // if this is root node (Part item index)
    if (!aIndex->parent().isValid()) {
      beginResetModel();
      myActivePart = (myActivePart == aModel)? 0 : (XGUI_PartModel*)aModel;
      endResetModel();
      return true;
    }
  }
  return false;
}

FeaturePtr XGUI_DocumentDataModel::activePart() const
{
  if (myActivePart) 
    return myActivePart->part();
  return FeaturePtr();
}