#include "XGUI_DocumentDataModel.h"
#include "XGUI_PartDataModel.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Iterator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <Model_Events.h>

#include <Event_Loop.h>


#include <QIcon>
#include <QString>


XGUI_DocumentDataModel::XGUI_DocumentDataModel(QObject* theParent)
  : QAbstractItemModel(theParent)
{
  // Find Document object
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  myDocument = aMgr->currentDocument();

  // Register in event loop
  Event_Loop::loop()->registerListener(this, Event_Loop::eventByName(EVENT_FEATURE_CREATED));
  Event_Loop::loop()->registerListener(this, Event_Loop::eventByName(EVENT_FEATURE_UPDATED));
  Event_Loop::loop()->registerListener(this, Event_Loop::eventByName(EVENT_FEATURE_DELETED));

  // Create a top part of data tree model
  myModel = new XGUI_TopDataModel(myDocument, this);
}


XGUI_DocumentDataModel::~XGUI_DocumentDataModel()
{
  clearModelIndexes();
}


void XGUI_DocumentDataModel::processEvent(const Event_Message* theMessage)
{
  // Created object event *******************
  if (QString(theMessage->eventID().eventText()) == EVENT_FEATURE_CREATED) {
    const ModelAPI_FeatureUpdatedMessage* aUpdMsg = dynamic_cast<const ModelAPI_FeatureUpdatedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Document> aDoc = aUpdMsg->document();
    boost::shared_ptr<ModelAPI_Feature> aFeature = aUpdMsg->feature();

    if (aDoc == myDocument) {  // If root objects
      if (aFeature->getGroup().compare(PARTS_GROUP) == 0) { // Updsate only Parts group
        // Add a new part
        int aStart = myModel->rowCount(QModelIndex()) + myPartModels.size() + 1;
        XGUI_PartDataModel* aModel = new XGUI_PartDataModel(myDocument, this);
        aModel->setPartId(myPartModels.count());
        myPartModels.append(aModel);
        insertRows(QModelIndex(), aStart, aStart);
      } else { // Update top groups (other except parts
        QModelIndex aIndex = myModel->findParent(aFeature);
        int aStart = myModel->rowCount(aIndex) - 1;
        aIndex = createIndex(aIndex.row(), aIndex.column(), (void*)getModelIndex(aIndex));
        insertRows(aIndex, aStart, aStart);
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
        insertRows(aIndex, aStart, aStart);
      }
    }

  // Deteted object event ***********************
  } else if (QString(theMessage->eventID().eventText()) == EVENT_FEATURE_DELETED) {
    const ModelAPI_FeatureDeletedMessage* aUpdMsg = dynamic_cast<const ModelAPI_FeatureDeletedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Document> aDoc = aUpdMsg->document();

    if (aDoc == myDocument) {  // If root objects
      if (aUpdMsg->group().compare(PARTS_GROUP) == 0) { // Updsate only Parts group
        int aStart = myModel->rowCount(QModelIndex()) + myPartModels.size() - 1;
        beginRemoveRows(QModelIndex(), aStart, aStart);
        removeSubModel(myPartModels.size() - 1);
        endRemoveRows();
      } else { // Update top groups (other except parts
        QModelIndex aIndex = myModel->findGroup(aUpdMsg->group());
        int aStart = myModel->rowCount(aIndex);
        aIndex = createIndex(aIndex.row(), aIndex.column(), (void*)getModelIndex(aIndex));
        beginRemoveRows(aIndex, aStart, aStart);
        endRemoveRows();
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
        beginRemoveRows(aIndex, aStart, aStart);
        endRemoveRows();
      }
    }

  // Reset whole tree **************************
  } else {  
    beginResetModel();
    int aNbParts = myDocument->featuresIterator(PARTS_GROUP)->numIterationsLeft();
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
  return toSourceModelIndex(theIndex).data(theRole);
}


QVariant XGUI_DocumentDataModel::headerData(int theSection, Qt::Orientation theOrient, int theRole) const
{
  return QVariant();
}

int XGUI_DocumentDataModel::rowCount(const QModelIndex& theParent) const
{
  if (!theParent.isValid()) {
    int aVal = myModel->rowCount(theParent) + myPartModels.size();
    return myModel->rowCount(theParent) + myPartModels.size();
  }
  QModelIndex aParent = toSourceModelIndex(theParent);
  if (!hasSubModel(aParent.model())) 
    return 0;

  return aParent.model()->rowCount(aParent);
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
    if (theRow < aOffs) 
      aIndex = myModel->index(theRow, theColumn, theParent);
    else  {
      if (myPartModels.size() > 0) {
        int aPos = theRow - aOffs;
        if (aPos >= myPartModels.size())
          aPos = 0;
        aIndex = myPartModels.at(aPos)->index(aPos, theColumn, theParent);
      } else 
        return aIndex;
    }
    aIndex = createIndex(theRow, theColumn, (void*)getModelIndex(aIndex));
  } else {
    QModelIndex* aParent = (QModelIndex*)theParent.internalPointer();
    aIndex = aParent->model()->index(theRow, theColumn, (*aParent));

    aIndex = createIndex(theRow, theColumn, (void*)getModelIndex(aIndex));
  }
  return aIndex;
}


QModelIndex XGUI_DocumentDataModel::parent(const QModelIndex& theIndex) const
{
  QModelIndex aParent = toSourceModelIndex(theIndex);
  if (!hasSubModel(aParent.model())) 
    return QModelIndex();

  aParent = aParent.model()->parent(aParent);
  if (aParent.isValid())
    return createIndex(aParent.row(), aParent.column(), (void*)getModelIndex(aParent));
  return aParent;
}


bool XGUI_DocumentDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;
  return rowCount(theParent) > 0;
}


QModelIndex XGUI_DocumentDataModel::toSourceModelIndex(const QModelIndex& theProxy) const
{
  QModelIndex* aIndexPtr = static_cast<QModelIndex*>(theProxy.internalPointer());
  return (*aIndexPtr);
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
  QModelIndex aIndex = toSourceModelIndex(theIndex);
  if (!hasSubModel(aIndex.model())) 
    return FeaturePtr();

  const XGUI_FeaturesModel* aModel = dynamic_cast<const XGUI_FeaturesModel*>(aIndex.model());
  return aModel->feature(aIndex);
}

void XGUI_DocumentDataModel::insertRows(const QModelIndex& theParent, int theStart, int theEnd)
{
  beginInsertRows(theParent, theStart, theEnd);
  endInsertRows();
  if (theStart == 0) // Update parent if this is a first child in order to update node decoration
    emit dataChanged(theParent, theParent);
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

bool XGUI_DocumentDataModel::hasSubModel(const QAbstractItemModel* theModel) const
{
  if (theModel == myModel)
    return true;
  QList<XGUI_PartModel*>::const_iterator aIt;
  for (aIt = myPartModels.constBegin(); aIt != myPartModels.constEnd(); ++aIt) 
    if ((*aIt) == theModel)
      return true;
  return false;
}
