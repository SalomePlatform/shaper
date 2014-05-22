#include "XGUI_PartDataModel.h"
#include "XGUI_Workshop.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>

#include <QIcon>
#include <QBrush>


//FeaturePtr featureObj(const FeaturePtr& theFeature)
//{
//  ObjectPtr aObject = boost::dynamic_pointer_cast<ModelAPI_Object>(theFeature);
//  if (aObject)
//    return aObject->featureRef();
//  return theFeature;
//}


XGUI_TopDataModel::XGUI_TopDataModel(const DocumentPtr& theDocument, QObject* theParent)
  : XGUI_FeaturesModel(theDocument, theParent)
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
    switch (theIndex.internalId()) {
    case ParamsFolder:
      return tr("Parameters") + QString(" (%1)").arg(rowCount(theIndex));
    case ParamObject:
      {
        FeaturePtr aFeature = myDocument->feature(PARAMETERS_GROUP, theIndex.row(), true);
        if (aFeature)
          return aFeature->data()->getName().c_str();
      } 
    case ConstructFolder:
        return tr("Constructions") + QString(" (%1)").arg(rowCount(theIndex));
    case ConstructObject:
      {
        FeaturePtr aFeature = myDocument->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
        if (aFeature)
          return aFeature->data()->getName().c_str();
      }
    }
    break;

  case Qt::DecorationRole:
    // return an Icon
    switch (theIndex.internalId()) {
    case ParamsFolder:
      return QIcon(":pictures/params_folder.png");
    case ConstructFolder:
      return QIcon(":pictures/constr_folder.png");
    case ConstructObject:
      {
        FeaturePtr aFeature = myDocument->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
        if (aFeature)
          return QIcon(XGUI_Workshop::featureIcon(aFeature->getKind()));
      }
    }
    break;

  case Qt::ToolTipRole:
    // return Tooltip
    break;
  case Qt::ForegroundRole:
    return QBrush(myItemsColor);
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
    return myDocument->size(PARAMETERS_GROUP);

  if (theParent.internalId() == ConstructFolder)
    return myDocument->size(CONSTRUCTIONS_GROUP);

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
      return createIndex(theRow, theColumn, (qint32) ParamsFolder);
    case 1:
      return createIndex(theRow, theColumn, (qint32) ConstructFolder);
    }
  } else {
    if (theParent.internalId() == ParamsFolder)
      return createIndex(theRow, theColumn, (qint32) ParamObject);

    if (theParent.internalId() == ConstructFolder)
      return createIndex(theRow, theColumn, (qint32) ConstructObject);
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
    return createIndex(0, 0, (qint32) ParamsFolder);
  case ConstructObject:
    return createIndex(1, 0, (qint32) ConstructFolder);
  }
  return QModelIndex();
}

bool XGUI_TopDataModel::hasChildren(const QModelIndex& theParent) const
{
  return rowCount(theParent) > 0;
}

FeaturePtr XGUI_TopDataModel::feature(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
  case ParamsFolder:
  case ConstructFolder:
    return FeaturePtr();
  case ParamObject:
    return myDocument->feature(PARAMETERS_GROUP, theIndex.row(), true);
  case ConstructObject:
    return myDocument->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
  }
  return FeaturePtr();
}


QModelIndex XGUI_TopDataModel::findParent(const FeaturePtr& theFeature) const
{
  return findGroup(theFeature->getGroup().c_str());
}

QModelIndex XGUI_TopDataModel::findGroup(const std::string& theGroup) const
{
  if (theGroup.compare(PARAMETERS_GROUP) == 0)
    return createIndex(0, 0, (qint32) ParamsFolder);
  if (theGroup.compare(CONSTRUCTIONS_GROUP) == 0)
    return createIndex(1, 0, (qint32) ConstructFolder);
  return QModelIndex();
}

QModelIndex XGUI_TopDataModel::featureIndex(const FeaturePtr& theFeature) const
{
  QModelIndex aIndex;
  if (theFeature) {
    std::string aGroup = theFeature->getGroup();
    int aNb = myDocument->size(aGroup);
    int aRow = -1;
    for (int i = 0; i < aNb; i++) {
      if (myDocument->feature(aGroup, i, true) == theFeature) {
        aRow = i;
        break;
      }
    }
    if (aRow != -1) {
      if (aGroup.compare(PARAMETERS_GROUP) == 0)
        return createIndex(aRow, 0, (qint32) ParamObject);
      if (aGroup.compare(CONSTRUCTIONS_GROUP) == 0)
        return createIndex(aRow, 0, (qint32) ConstructObject);
    }
  }
  return aIndex;
}



//******************************************************************
//******************************************************************
//******************************************************************
XGUI_PartDataModel::XGUI_PartDataModel(const DocumentPtr& theDocument, QObject* theParent)
  : XGUI_PartModel(theDocument, theParent)
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
    switch (theIndex.internalId()) {
    case MyRoot:
      {
        FeaturePtr aFeature = myDocument->feature(PARTS_GROUP, myId, true);
        if (aFeature)
          return aFeature->data()->getName().c_str();
      }
    case ParamsFolder:
      return tr("Parameters") + QString(" (%1)").arg(rowCount(theIndex));
    case ConstructFolder:
      return tr("Constructions") + QString(" (%1)").arg(rowCount(theIndex));
    case BodiesFolder:
      return tr("Bodies") + QString(" (%1)").arg(rowCount(theIndex));
    case ParamObject:
      {
        FeaturePtr aFeature = featureDocument()->feature(PARAMETERS_GROUP, theIndex.row(), true);
        if (aFeature)
          return aFeature->data()->getName().c_str();
      }
    case ConstructObject:
      {
        FeaturePtr aFeature = featureDocument()->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
        if (aFeature)
          return aFeature->data()->getName().c_str();
      }
    case HistoryObject:
      {
        FeaturePtr aFeature = featureDocument()->feature(FEATURES_GROUP, theIndex.row() - 3, true);
        if (aFeature)
          return aFeature->data()->getName().c_str();
      }
    }
    break;
  case Qt::DecorationRole:
    // return an Icon
    switch (theIndex.internalId()) {
    case MyRoot:
      return QIcon(":pictures/part_ico.png");
    case ParamsFolder:
      return QIcon(":pictures/params_folder.png");
    case ConstructFolder:
    case BodiesFolder:
      return QIcon(":pictures/constr_folder.png");
    case ConstructObject:
      {
        FeaturePtr aFeature = featureDocument()->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
        if (aFeature)
          return QIcon(XGUI_Workshop::featureIcon(aFeature->getKind()));
      }
    case HistoryObject:
      {
        FeaturePtr aFeature = featureDocument()->feature(FEATURES_GROUP, theIndex.row() - 3, true);
        if (aFeature)
          return QIcon(XGUI_Workshop::featureIcon(aFeature->getKind()));
      }
    }
   break;
  case Qt::ToolTipRole:
    // return Tooltip
    break;
  case Qt::ForegroundRole:
    return QBrush(myItemsColor);
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
    if (myDocument->feature(PARTS_GROUP, myId, true))
      return 1;
    else 
      return 0;
  switch (parent.internalId()) {
  case MyRoot:
    return 3 + featureDocument()->size(FEATURES_GROUP);
  case ParamsFolder:
    return featureDocument()->size(PARAMETERS_GROUP);
  case ConstructFolder:
    return featureDocument()->size(CONSTRUCTIONS_GROUP);
  case BodiesFolder:
    return 0;
  }
  return 0;
}

int XGUI_PartDataModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

QModelIndex XGUI_PartDataModel::index(int theRow, int theColumn, const QModelIndex &theParent) const
{
  if (!theParent.isValid())
    return createIndex(theRow, 0, (qint32) MyRoot);

  int aId = (int)theParent.internalId();
  switch (aId) {
  case MyRoot:
    switch (theRow) {
    case 0:
      return createIndex(0, 0, (qint32) ParamsFolder);
    case 1:
      return createIndex(1, 0, (qint32) ConstructFolder);
    case 2:
      return createIndex(2, 0, (qint32) BodiesFolder);
    default:
      return createIndex(theRow, theColumn, (qint32) HistoryObject);
    }
  case ParamsFolder:
    return createIndex(theRow, 0, (qint32) ParamObject);
  case ConstructFolder:
    return createIndex(theRow, 0, (qint32) ConstructObject);
  case BodiesFolder:
    return createIndex(theRow, 0, (qint32) BodiesObject);
  }
  return QModelIndex();
}

QModelIndex XGUI_PartDataModel::parent(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
  case MyRoot:
    return QModelIndex();
  case ParamsFolder:
  case ConstructFolder:
  case BodiesFolder:
  case HistoryObject:
    return createIndex(0, 0, (qint32) MyRoot);
  case ParamObject:
    return createIndex(0, 0, (qint32) ParamsFolder);
  case ConstructObject:
    return createIndex(1, 0, (qint32) ConstructFolder);
  }
  return QModelIndex();
}

bool XGUI_PartDataModel::hasChildren(const QModelIndex& theParent) const
{
  return rowCount(theParent) > 0;
}


DocumentPtr XGUI_PartDataModel::featureDocument() const
{
  FeaturePtr aFeature = myDocument->feature(PARTS_GROUP, myId, true);
  return aFeature->data()->docRef("PartDocument")->value();
}
 
FeaturePtr XGUI_PartDataModel::feature(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
  case MyRoot:
    if (theIndex.row() < 3) {
      return myDocument->feature(PARTS_GROUP, myId, true);
    } else 
      return featureDocument()->feature(FEATURES_GROUP, theIndex.row() - 3, true);
  case ParamsFolder:
  case ConstructFolder:
  case BodiesFolder:
    return FeaturePtr();
  case ParamObject:
    return featureDocument()->feature(PARAMETERS_GROUP, theIndex.row(), true);
  case ConstructObject:
    return featureDocument()->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
  //case BodiesObject:
  //  return featureDocument()->feature(CONSTRUCTIONS_GROUP, theIndex.row(), true);
  }
  return FeaturePtr();
}

bool XGUI_PartDataModel::hasDocument(const DocumentPtr& theDoc) const
{
  return (featureDocument() == theDoc);
}


QModelIndex XGUI_PartDataModel::findParent(const FeaturePtr& theFeature) const
{
  return findGroup(theFeature->getGroup().c_str());
}

QModelIndex XGUI_PartDataModel::findGroup(const std::string& theGroup) const
{
  if (theGroup.compare(PARAMETERS_GROUP) == 0)
    return createIndex(0, 0, (qint32) ParamsFolder);
  if (theGroup.compare(CONSTRUCTIONS_GROUP) == 0)
    return createIndex(1, 0, (qint32) ConstructFolder);
  return QModelIndex();
}

FeaturePtr XGUI_PartDataModel::part() const
{
  return myDocument->feature(PARTS_GROUP, myId, true);
}

QModelIndex XGUI_PartDataModel::featureIndex(const FeaturePtr& theFeature) const
{
  QModelIndex aIndex;
  if (theFeature) {
    if (part() == theFeature) 
      return aIndex;

    std::string aGroup = theFeature->getGroup();
    int aNb = myDocument->size(aGroup);
    int aRow = -1;
    for (int i = 0; i < aNb; i++) {
      if (myDocument->feature(aGroup, i, true) == theFeature) {
        aRow = i;
        break;
      }
    }
    if (aRow != -1) {
      if (aGroup.compare(PARAMETERS_GROUP) == 0)
        return createIndex(aRow, 0, (qint32) ParamObject);
      if (aGroup.compare(CONSTRUCTIONS_GROUP) == 0)
        return createIndex(aRow, 0, (qint32) ConstructObject);
    }
  }
  return aIndex;
}
