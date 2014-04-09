#include "XGUI_DocumentDataModel.h"
#include "XGUI_PartDataModel.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Iterator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <Model_Document.h>

#include <Event_Loop.h>


#include <QIcon>


XGUI_DocumentDataModel::XGUI_DocumentDataModel(QObject* theParent)
  : QAbstractItemModel(theParent)
{
  // Find Document object
  std::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  myDocument = aMgr->currentDocument();

  // Register in event loop
  Event_Loop::loop()->registerListener(this, Event_Loop::eventByName(EVENT_FEATURE_UPDATED));

  // Create a top part of data tree model
  myModel = new XGUI_TopDataModel(this);
  myModel->setDocument(myDocument);
}


XGUI_DocumentDataModel::~XGUI_DocumentDataModel()
{
  clearModelIndexes();
}


void XGUI_DocumentDataModel::processEvent(const Event_Message* theMessage)
{
  beginResetModel();
  int aNbParts = myDocument->featuresIterator(PARTS_GROUP)->numIterationsLeft();
  if (myPartModels.size() != aNbParts) { // resize internal models
    while (myPartModels.size() > aNbParts) {
      delete myPartModels.last();
      myPartModels.removeLast();
    }
    while (myPartModels.size() < aNbParts) {
      myPartModels.append(new XGUI_PartDataModel(this));
    }
    for (int i = 0; i < myPartModels.size(); i++)
      myPartModels.at(i)->setDocument(myDocument, i);
  }
  clearModelIndexes();
  endResetModel();
}

QVariant XGUI_DocumentDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (!theIndex.isValid())
    return QVariant();
  return toSourceModel(theIndex).data(theRole);
}


QVariant XGUI_DocumentDataModel::headerData(int theSection, Qt::Orientation theOrient, int theRole) const
{
  return QVariant();
}

int XGUI_DocumentDataModel::rowCount(const QModelIndex& theParent) const
{
  if (!theParent.isValid()) 
    return myModel->rowCount(theParent) + myPartModels.size();

  QModelIndex aParent = toSourceModel(theParent);
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
    else
      aIndex = myPartModels.at(theRow - aOffs)->index(theRow - aOffs, theColumn, theParent);

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
  QModelIndex aParent = toSourceModel(theIndex);
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


QModelIndex XGUI_DocumentDataModel::toSourceModel(const QModelIndex& theProxy) const
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