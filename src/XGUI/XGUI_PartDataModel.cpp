#include "XGUI_PartDataModel.h"
#include "XGUI_Workshop.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultParameters.h>

#include <QIcon>
#include <QBrush>


//ObjectPtr featureObj(const ObjectPtr& theFeature)
//{
//  ObjectPtr aObject = boost::dynamic_pointer_cast<ModelAPI_Object>(theFeature);
//  if (aObject)
//    return aObject->featureRef();
//  return theFeature;
//}


XGUI_TopDataModel::XGUI_TopDataModel(QObject* theParent)
  : XGUI_FeaturesModel(theParent)
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
        DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
        ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultParameters::group(), theIndex.row());
        if (aObject)
          return boost::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
      } 
    case ConstructFolder:
        return tr("Constructions") + QString(" (%1)").arg(rowCount(theIndex));
    case ConstructObject:
      {
        DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
        ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultConstruction::group(), theIndex.row());
        if (aObject)
          return boost::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
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
      return QIcon(":pictures/constr_object.png");
      //{
      //  DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
      //  ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultConstruction::group(), theIndex.row());
      //  if (aObject)
      //    return QIcon(XGUI_Workshop::featureIcon(aObject->getKind()));
      //}
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

  DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
  if (theParent.internalId() == ParamsFolder)
    return aRootDoc->size(ModelAPI_ResultParameters::group());

  if (theParent.internalId() == ConstructFolder)
    return aRootDoc->size(ModelAPI_ResultConstruction::group());

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

ObjectPtr XGUI_TopDataModel::object(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
  case ParamsFolder:
  case ConstructFolder:
    return ObjectPtr();
  case ParamObject:
    {
      DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
      return aRootDoc->object(ModelAPI_ResultParameters::group(), theIndex.row());
    }
  case ConstructObject:
    {
      DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
      return aRootDoc->object(ModelAPI_ResultConstruction::group(), theIndex.row());
    }
  }
  return ObjectPtr();
}


QModelIndex XGUI_TopDataModel::findParent(const ObjectPtr& theObject) const
{
  return findGroup(theObject->groupName().c_str());
}

QModelIndex XGUI_TopDataModel::findGroup(const std::string& theGroup) const
{
  if (theGroup.compare(ModelAPI_ResultParameters::group()) == 0)
    return createIndex(0, 0, (qint32) ParamsFolder);
  if (theGroup.compare(ModelAPI_ResultConstruction::group()) == 0)
    return createIndex(1, 0, (qint32) ConstructFolder);
  return QModelIndex();
}

QModelIndex XGUI_TopDataModel::objectIndex(const ObjectPtr& theObject) const
{
  QModelIndex aIndex;
  if (theObject) {
    DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
    std::string aGroup = theObject->groupName();
    int aNb = aRootDoc->size(aGroup);
    int aRow = -1;
    for (int i = 0; i < aNb; i++) {
      if (aRootDoc->object(aGroup, i) == theObject) {
        aRow = i;
        break;
      }
    }
    if (aRow != -1) {
      if (aGroup.compare(ModelAPI_ResultParameters::group()) == 0)
        return createIndex(aRow, 0, (qint32) ParamObject);
      if (aGroup.compare(ModelAPI_ResultConstruction::group()) == 0)
        return createIndex(aRow, 0, (qint32) ConstructObject);
    }
  }
  return aIndex;
}



//******************************************************************
//******************************************************************
//******************************************************************
XGUI_PartDataModel::XGUI_PartDataModel(QObject* theParent)
  : XGUI_PartModel(theParent)
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
        DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
        ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), myId);
        if (aObject)
          return boost::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
      }
    case ParamsFolder:
      return tr("Parameters") + QString(" (%1)").arg(rowCount(theIndex));
    case ConstructFolder:
      return tr("Constructions") + QString(" (%1)").arg(rowCount(theIndex));
    case BodiesFolder:
      return tr("Bodies") + QString(" (%1)").arg(rowCount(theIndex));
    case ParamObject:
      {
        ObjectPtr aObject = partDocument()->object(ModelAPI_ResultParameters::group(), theIndex.row());
        if (aObject)
          return boost::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
      }
    case ConstructObject:
      {
        ObjectPtr aObject = partDocument()->object(ModelAPI_ResultConstruction::group(), theIndex.row());
        if (aObject)
          return boost::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
      }
    case HistoryObject:
      {
        ObjectPtr aObject = partDocument()->object(ModelAPI_Feature::group(), theIndex.row() - 3);
        if (aObject)
          return aObject->data()->name().c_str();
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
      return QIcon(":pictures/constr_object.png");
      //{
      //  ObjectPtr aObject = partDocument()->object(ModelAPI_ResultConstruction::group(), theIndex.row());
      //  if (aObject)
      //    return QIcon(XGUI_Workshop::featureIcon(aObject->getKind()));
      //}
    case HistoryObject:
      {
        ObjectPtr aObject = partDocument()->object(ModelAPI_Feature::group(), theIndex.row() - 3);
        FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
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
  if (!parent.isValid()) {
    DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
    if (aRootDoc->object(ModelAPI_ResultPart::group(), myId))
      return 1;
    else 
      return 0;
  }
  switch (parent.internalId()) {
  case MyRoot:
    return 3 + partDocument()->size(ModelAPI_Feature::group());
  case ParamsFolder:
    return partDocument()->size(ModelAPI_ResultParameters::group());
  case ConstructFolder:
    return partDocument()->size(ModelAPI_ResultConstruction::group());
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


DocumentPtr XGUI_PartDataModel::partDocument() const
{
  DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
  ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), myId);
  return aObject->document();
}
 
ObjectPtr XGUI_PartDataModel::object(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
  case MyRoot:
    {
      DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
      return aRootDoc->object(ModelAPI_ResultPart::group(), myId);
    }
  case ParamsFolder:
  case ConstructFolder:
  case BodiesFolder:
    return ObjectPtr();
  case ParamObject:
    return partDocument()->object(ModelAPI_ResultParameters::group(), theIndex.row());
  case ConstructObject:
    return partDocument()->object(ModelAPI_ResultConstruction::group(), theIndex.row());
  //case BodiesObject:
  //  return partDocument()->feature(ModelAPI_ResultConstruction::group(), theIndex.row());
  case HistoryObject:
    return partDocument()->object(ModelAPI_Feature::group(), theIndex.row() - 3); 
  }
  return ObjectPtr();
}

bool XGUI_PartDataModel::hasDocument(const DocumentPtr& theDoc) const
{
  return (partDocument() == theDoc);
}


QModelIndex XGUI_PartDataModel::findParent(const ObjectPtr& theObject) const
{
  return findGroup(theObject->groupName().c_str());
}

QModelIndex XGUI_PartDataModel::findGroup(const std::string& theGroup) const
{
  if (theGroup.compare(ModelAPI_ResultParameters::group()) == 0)
    return createIndex(0, 0, (qint32) ParamsFolder);
  if (theGroup.compare(ModelAPI_ResultConstruction::group()) == 0)
    return createIndex(1, 0, (qint32) ConstructFolder);
  return QModelIndex();
}

ResultPartPtr XGUI_PartDataModel::part() const
{
  DocumentPtr aRootDoc = ModelAPI_PluginManager::get()->rootDocument();
  ObjectPtr aObj = aRootDoc->object(ModelAPI_ResultPart::group(), myId);
  return boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
}

QModelIndex XGUI_PartDataModel::objectIndex(const ObjectPtr& theObject) const
{
  QModelIndex aIndex;
  if (theObject) {
    if (part() == theObject) 
      return aIndex;

    //std::string aGroup = theFeature->getGroup();
    DocumentPtr aDoc = theObject->document();
    int aNb = aDoc->size(ModelAPI_Feature::group());
    int aRow = -1;
    for (int i = 0; i < aNb; i++) {
      if (aDoc->object(ModelAPI_Feature::group(), i) == theObject) {
        aRow = i;
        break;
      }
    }
    if (aRow != -1) {
      return createIndex(aRow + 3, 0, (qint32) HistoryObject);
/*      if (aGroup.compare(ModelAPI_ResultParameters::group()) == 0)
        return createIndex(aRow, 0, (qint32) ParamObject);
      if (aGroup.compare(ModelAPI_ResultConstruction::group()) == 0)
        return createIndex(aRow, 0, (qint32) ConstructObject);*/
    }
  }
  return aIndex;
}
