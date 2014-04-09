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
  std::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  myDocument = aMgr->currentDocument();

  Event_Loop::loop()->registerListener(this, Event_Loop::eventByName(EVENT_FEATURE_UPDATED));

  myModel = new XGUI_TopDataModel(this);
  myModel->setDocument(myDocument);
}


XGUI_DocumentDataModel::~XGUI_DocumentDataModel()
{
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

  return 0;
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
    aIndex = createIndex(theRow, theColumn, aIndex.internalId());
  }
  return aIndex;
}


QModelIndex XGUI_DocumentDataModel::parent(const QModelIndex& theIndex) const
{
  return QModelIndex();
}


bool XGUI_DocumentDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;
  return false;
}


QModelIndex XGUI_DocumentDataModel::toSourceModel(const QModelIndex& theProxy) const
{
  int aRow = theProxy.row();
  if (!theProxy.parent().isValid()) {
    if (aRow < myModel->rowCount()) {
      return myModel->index(aRow, 0);
    } else {
      int aOffs = aRow - myModel->rowCount();
      return myPartModels.at(aOffs)->index(aOffs, 0);
    }
  } 
  return QModelIndex();
}


