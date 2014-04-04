#include "XGUI_DocumentDataModel.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Iterator.h>
#include <ModelAPI_Document.h>




XGUI_DocumentDataModel::XGUI_DocumentDataModel(QObject* theParent)
  : QAbstractItemModel(theParent),
  myParamsFolder(0),
  myConstructFolder(0)
{
  //std::shared_ptr<ModelAPI_Feature> myRoot = aMgr->createFeature("Point");
  std::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  myDocument = aMgr->currentDocument();
}


XGUI_DocumentDataModel::~XGUI_DocumentDataModel()
{
}


QVariant XGUI_DocumentDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  switch (theRole) {
  case Qt::DisplayRole:
    // return a name
    if (theIndex.internalId() == quintptr(&myParamsFolder))
      return "Parameters";
    else if (theIndex.internalId() == quintptr(&myConstructFolder))
      return "Constructions";
    else if (theIndex.internalId() == 0) {
      return "Part";
    }
    break;
  case Qt::DecorationRole:
    // return an Icon
    break;
  case Qt::ToolTipRole:
    // return Tooltip
    break;
  }
  return QVariant();
}


QVariant XGUI_DocumentDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

int XGUI_DocumentDataModel::rowCount(const QModelIndex &parent) const
{
  std::shared_ptr<ModelAPI_Iterator> aIt = myDocument->featuresIterator(PARTS_GROUP);
  return aIt->numIterationsLeft() + 2;
}

int XGUI_DocumentDataModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

QModelIndex XGUI_DocumentDataModel::index(int theRow, int theColumn, const QModelIndex& theParent) const
{
  switch (theRow) {
  case 0:
    return createIndex(theRow, theColumn, (quintptr) &myParamsFolder);
  case 1:
    return createIndex(theRow, theColumn, (quintptr) &myConstructFolder);
  default:
    {
      std::shared_ptr<ModelAPI_Iterator> aIt = myDocument->featuresIterator(PARTS_GROUP);
      if (aIt->numIterationsLeft() <= (theRow - 1)) {
        return createIndex(theRow, theColumn, (quintptr) 0);
      }
    }
  }
  return QModelIndex();
}


QModelIndex XGUI_DocumentDataModel::parent(const QModelIndex &index) const
{
  return QModelIndex();
}

bool XGUI_DocumentDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;

  if (theParent.internalId() == quintptr(&myParamsFolder)) 
    return myDocument->featuresIterator(PARAMETERS_GROUP)->more();
  if (theParent.internalId() == quintptr(&myConstructFolder))
    return myDocument->featuresIterator(CONSTRUCTIONS_GROUP)->more();
  if (theParent.internalId() == 0)
    return false;
  return false;
}