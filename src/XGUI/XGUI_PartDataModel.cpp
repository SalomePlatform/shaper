#include "XGUI_PartDataModel.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Iterator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDocRef.h>

#include <QIcon>

XGUI_TopDataModel::XGUI_TopDataModel(QObject* theParent)
  : QAbstractItemModel(theParent)
{
}
  
XGUI_TopDataModel::~XGUI_TopDataModel()
{
}


QVariant XGUI_TopDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  switch (theRole) {
  case Qt::DisplayRole:
    // return a name
    if (theIndex.model() == this) {
      if (theIndex.internalId() == ParamsFolder)
        return tr("Parameters");

      if (theIndex.internalId() == ParamObject) {
        std::shared_ptr<ModelAPI_Feature> aFeature = myDocument->feature(PARAMETERS_GROUP, theIndex.row());
        return aFeature->data()->getName().c_str();
      } 
      if (theIndex.internalId() == ConstructFolder)
        return tr("Constructions");

      if (theIndex.internalId() == ConstructObject) {
        std::shared_ptr<ModelAPI_Feature> aFeature = myDocument->feature(CONSTRUCTIONS_GROUP, theIndex.row());
        return aFeature->data()->getName().c_str();
      }
    } 
    break;

  case Qt::DecorationRole:
    // return an Icon
    if (theIndex.model() == this) {
      if (theIndex.internalId() == ParamsFolder)
        return QIcon(":pictures/params_folder.png");
      else if (theIndex.internalId() == ConstructFolder)
        return QIcon(":pictures/constr_folder.png");
    }
    break;

  case Qt::ToolTipRole:
    // return Tooltip
    break;
  }
  return QVariant();
}

QVariant XGUI_TopDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

int XGUI_TopDataModel::rowCount(const QModelIndex& theParent) const
{
  if (!theParent.isValid()) 
    return 2;

  if (theParent.internalId() == ParamsFolder)
    return myDocument->featuresIterator(PARAMETERS_GROUP)->numIterationsLeft();

  if (theParent.internalId() == ConstructFolder)
    return myDocument->featuresIterator(CONSTRUCTIONS_GROUP)->numIterationsLeft();

  return 0;
}

int XGUI_TopDataModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

QModelIndex XGUI_TopDataModel::index(int theRow, int theColumn, const QModelIndex& theParent) const
{
  if (!theParent.isValid()) {
    switch (theRow) {
    case 0:
      return createIndex(theRow, theColumn, (quintptr) ParamsFolder);
    case 1:
      return createIndex(theRow, theColumn, (quintptr) ConstructFolder);
    }
  } else {
    if (theParent.internalId() == ParamsFolder)
      return createIndex(theRow, theColumn, (quintptr) ParamObject);

    if (theParent.internalId() == ConstructFolder)
      return createIndex(theRow, theColumn, (quintptr) ConstructObject);
  }
  return QModelIndex();
}

QModelIndex XGUI_TopDataModel::parent(const QModelIndex& theIndex) const
{
  int aId = (int)theIndex.internalId();
  switch (aId) {
  case ParamsFolder:
  case ConstructFolder:
    return QModelIndex();
  case ParamObject:
    return createIndex(0, 0, (quintptr) ParamsFolder);
  case ConstructObject:
    return createIndex(1, 0, (quintptr) ConstructFolder);
  }
  return QModelIndex();
}

bool XGUI_TopDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return true;

  int aId = (int)theParent.internalId();
  switch (aId) {
  case ParamsFolder:
    return myDocument->featuresIterator(PARAMETERS_GROUP)->more();
  case ConstructFolder:
    return myDocument->featuresIterator(CONSTRUCTIONS_GROUP)->more();
  case ParamObject:
  case ConstructObject:
    return false;
  } 
  return false;
}


//******************************************************************
//******************************************************************
//******************************************************************
XGUI_PartDataModel::XGUI_PartDataModel(QObject* theParent)
  : QAbstractItemModel(theParent)
{
}


XGUI_PartDataModel::~XGUI_PartDataModel()
{
}

QVariant XGUI_PartDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  switch (theRole) {
  case Qt::DisplayRole:
    // return a name
    if (theIndex.internalId() == MyRoot) {
      std::shared_ptr<ModelAPI_Feature> aFeature = myDocument->feature(PARTS_GROUP, myId);
      return aFeature->data()->getName().c_str();
    }
    if (theIndex.internalId() == ParamsFolder)
      return tr("Parameters");
    if (theIndex.internalId() == ConstructFolder)
      return tr("Constructions");
    break;
  case Qt::DecorationRole:
    // return an Icon
    if (theIndex.internalId() == MyRoot) 
      return QIcon(":pictures/part_ico.png");
    if (theIndex.internalId() == ParamsFolder)
      return QIcon(":pictures/params_folder.png");
    if (theIndex.internalId() == ConstructFolder)
      return QIcon(":pictures/constr_folder.png");
   break;
  case Qt::ToolTipRole:
    // return Tooltip
    break;
  }
  return QVariant();
}

QVariant XGUI_PartDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

int XGUI_PartDataModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid()) 
    if (myDocument->feature(PARTS_GROUP, myId))
      return 1;
    else 
      return 0;
  if (parent.internalId() == MyRoot)
      return 2;
  return 0;
}

int XGUI_PartDataModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

QModelIndex XGUI_PartDataModel::index(int theRow, int theColumn, const QModelIndex &theParent) const
{
  if (!theParent.isValid())
    return createIndex(theRow, 0, (quintptr) MyRoot);

  int aId = (int)theParent.internalId();
  switch (aId) {
  case MyRoot:
    switch (theRow) {
    case 0:
      return createIndex(0, 0, (quintptr) ParamsFolder);
    case 1:
      return createIndex(1, 0, (quintptr) ConstructFolder);
    }
  case ParamsFolder:
    return createIndex(theRow, 0, (quintptr) ParamObject);
  case ConstructFolder:
    return createIndex(theRow, 0, (quintptr) ConstructObject);
  }
  return QModelIndex();
}

QModelIndex XGUI_PartDataModel::parent(const QModelIndex& theIndex) const
{
  int aId = (int)theIndex.internalId();
  switch (aId) {
  case MyRoot:
    return QModelIndex();
  case ParamsFolder:
  case ConstructFolder:
    return createIndex(0, 0, (quintptr) MyRoot);
  case ParamObject:
    return createIndex(0, 0, (quintptr) ParamsFolder);
  case ConstructObject:
    return createIndex(1, 0, (quintptr) ConstructFolder);
  }
  return QModelIndex();
}

bool XGUI_PartDataModel::hasChildren(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return myDocument->feature(PARTS_GROUP, myId);

  int aId = (int)theParent.internalId();
  switch (aId) {
  case MyRoot:
    return true;
  case ParamsFolder:
    return false; // TODO
  case ConstructFolder:
    return false; // TODO
  case ParamObject:
  case ConstructObject:
    return false;
  }
  return false;
}
