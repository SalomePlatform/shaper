// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "PartSet_PartDataModel.h"
#include "PartSet_Module.h"
#include "PartSet_DocumentDataModel.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_AttributeDouble.h>

#include <QIcon>
#include <QBrush>


PartSet_TopDataModel::PartSet_TopDataModel(QObject* theParent)
    : PartSet_FeaturesModel(theParent)
{
}

PartSet_TopDataModel::~PartSet_TopDataModel()
{
}

QVariant PartSet_TopDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (theIndex.column() == 1)
    return QVariant();

  switch (theRole) {
    case Qt::DisplayRole:
      // return a name
      switch (theIndex.internalId()) {
        case ParamsFolder:
          return tr("Parameters") + QString(" (%1)").arg(rowCount(theIndex));
        case ParamObject: {
          DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
          ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultParameter::group(), theIndex.row());
          if (aObject) {
            ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aObject);
            AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
            QString aVal = QString::number(aValueAttribute->value());
            QString aTitle = QString(aObject->data()->name().c_str());
            return aTitle + " = " + aVal;
          }
        }
          break;
        case ConstructFolder:
          return tr("Constructions") + QString(" (%1)").arg(rowCount(theIndex));
        case ConstructObject: {
          DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
          ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultConstruction::group(),
                                               theIndex.row());
          if (aObject)
            return aObject->data()->name().c_str();
        }
          break;
        //case GroupsFolder:
        //  return tr("Groups") + QString(" (%1)").arg(rowCount(theIndex));
        //case GroupObject: {
        //  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
        //  ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultGroup::group(),
        //                                       theIndex.row());
        //  if (aObject)
        //    return aObject->data()->name().c_str();
        //}
        //  break;
      }
      break;

    case Qt::DecorationRole:
      {
      // return an Icon
      switch (theIndex.internalId()) {
        case ParamsFolder:
          return QIcon(":pictures/params_folder.png");
        case ConstructFolder:
          return QIcon(":pictures/constr_folder.png");
        case ConstructObject:
          return QIcon(":pictures/constr_object.png");
        //case GroupsFolder:
        //  return QIcon(":pictures/constr_folder.png");
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

QVariant PartSet_TopDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

int PartSet_TopDataModel::rowCount(const QModelIndex& theParent) const
{
  if (!theParent.isValid())
    return 2;  // In case of groups using it has to be +1

  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  if (theParent.internalId() == ParamsFolder)
    return aRootDoc->size(ModelAPI_ResultParameter::group());

  if (theParent.internalId() == ConstructFolder)
    return aRootDoc->size(ModelAPI_ResultConstruction::group());

  //if (theParent.internalId() == GroupsFolder)
  //  return aRootDoc->size(ModelAPI_ResultGroup::group());

  return 0;
}

int PartSet_TopDataModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

QModelIndex PartSet_TopDataModel::index(int theRow, int theColumn, const QModelIndex& theParent) const
{
  if (!theParent.isValid()) {
    switch (theRow) {
      case 0:
        return createIndex(theRow, theColumn, (qint32) ParamsFolder);
      case 1:
        return createIndex(theRow, theColumn, (qint32) ConstructFolder);
      //case 2:
      //  return createIndex(theRow, theColumn, (qint32) GroupsFolder);
    }
  } else {
    if (theParent.internalId() == ParamsFolder)
      return createIndex(theRow, theColumn, (qint32) ParamObject);

    if (theParent.internalId() == ConstructFolder)
      return createIndex(theRow, theColumn, (qint32) ConstructObject);

    //if (theParent.internalId() == GroupsFolder)
    //  return createIndex(theRow, theColumn, (qint32) GroupObject);
  }
  return QModelIndex();
}

QModelIndex PartSet_TopDataModel::parent(const QModelIndex& theIndex) const
{
  int aId = (int) theIndex.internalId();
  switch (aId) {
    case ParamsFolder:
    case ConstructFolder:
    //case GroupsFolder:
      return QModelIndex();
    case ParamObject:
      return createIndex(0, 0, (qint32) ParamsFolder);
    case ConstructObject:
      return createIndex(1, 0, (qint32) ConstructFolder);
    //case GroupObject:
    //  return createIndex(2, 0, (qint32) GroupsFolder);
  }
  return QModelIndex();
}

bool PartSet_TopDataModel::hasChildren(const QModelIndex& theParent) const
{
  return rowCount(theParent) > 0;
}

ObjectPtr PartSet_TopDataModel::object(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
    case ParamsFolder:
    case ConstructFolder:
      return ObjectPtr();
    case ParamObject: {
      DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
      return aRootDoc->object(ModelAPI_ResultParameter::group(), theIndex.row());
    }
    case ConstructObject: {
      DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
      return aRootDoc->object(ModelAPI_ResultConstruction::group(), theIndex.row());
    }
    //case GroupObject: {
    //  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    //  return aRootDoc->object(ModelAPI_ResultGroup::group(), theIndex.row());
    //}
  }
  return ObjectPtr();
}

QModelIndex PartSet_TopDataModel::findParent(const ObjectPtr& theObject) const
{
  return findGroup(theObject->groupName().c_str());
}

QModelIndex PartSet_TopDataModel::findGroup(const std::string& theGroup) const
{
  if (theGroup == ModelAPI_ResultParameter::group())
    return createIndex(0, 0, (qint32) ParamsFolder);
  if (theGroup == ModelAPI_ResultConstruction::group())
    return createIndex(1, 0, (qint32) ConstructFolder);
  //if (theGroup == ModelAPI_ResultGroup::group())
  //  return createIndex(2, 0, (qint32) ConstructFolder);
  return QModelIndex();
}

QModelIndex PartSet_TopDataModel::objectIndex(const ObjectPtr& theObject) const
{
  QModelIndex aIndex;
  if (theObject) {
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
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
      if (aGroup == ModelAPI_ResultParameter::group())
        return createIndex(aRow, 0, (qint32) ParamObject);
      if (aGroup == ModelAPI_ResultConstruction::group())
        return createIndex(aRow, 0, (qint32) ConstructObject);
      //if (aGroup == ModelAPI_ResultGroup::group())
      //  return createIndex(aRow, 0, (qint32) GroupObject);
    }
  }
  return aIndex;
}

//******************************************************************
//******************************************************************
//******************************************************************
PartSet_PartDataModel::PartSet_PartDataModel(QObject* theParent)
    : PartSet_PartModel(theParent)
{
}

PartSet_PartDataModel::~PartSet_PartDataModel()
{
}

QVariant PartSet_PartDataModel::data(const QModelIndex& theIndex, int theRole) const
{
  DocumentPtr aPartDoc = partDocument();
  if (theIndex.column() == 1) {
    DocumentPtr aActiveDoc = ModelAPI_Session::get()->activeDocument();
    QModelIndex aParent = theIndex.parent();
    if (aActiveDoc == aPartDoc) {
      if (!aParent.isValid()) {
        switch (theRole) {
        case Qt::DecorationRole:
          if (theIndex.row() == lastHistoryRow())
            return QIcon(":pictures/arrow.png");
        }
      }
    }
    return QVariant();
  }

  switch (theRole) {
    case Qt::DisplayRole:
      // return a name
      switch (theIndex.internalId()) {
        //case MyRoot: {
        //  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
        //  ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), myId);
        //  if (aObject)
        //    return std::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
        //}
        case ParamsFolder:
          return tr("Parameters") + QString(" (%1)").arg(rowCount(theIndex));
        case ConstructFolder:
          return tr("Constructions") + QString(" (%1)").arg(rowCount(theIndex));
        case BodiesFolder:
          return tr("Bodies") + QString(" (%1)").arg(rowCount(theIndex));
        case GroupsFolder:
          return tr("Groups") + QString(" (%1)").arg(rowCount(theIndex));
        case ParamObject: {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_ResultParameter::group(), theIndex.row());
          if (aObject) {
            ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aObject);
            AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
            QString aVal = QString::number(aValueAttribute->value());
            QString aTitle = QString(aObject->data()->name().c_str());
            return aTitle + " = " + aVal;
          }
        }
          break;
        case ConstructObject: {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_ResultConstruction::group(), theIndex.row());
          if (aObject)
            return std::dynamic_pointer_cast<ModelAPI_Object>(aObject)->data()->name().c_str();
        }
          break;
        case BodiesObject: {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_ResultBody::group(), theIndex.row());
          if (aObject)
            return aObject->data()->name().c_str();
        }
          break;
        case GroupObject: {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_ResultGroup::group(), theIndex.row());
          if (aObject)
            return aObject->data()->name().c_str();
        }
        case HistoryObject: {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_Feature::group(), theIndex.row() - getRowsNumber());
          if (aObject)
            return aObject->data()->name().c_str();
        }
      }
      break;
    case Qt::DecorationRole:
      // return an Icon
      switch (theIndex.internalId()) {
        //case MyRoot:
        //  return QIcon(":pictures/part_ico.png");
        case ParamsFolder:
          return QIcon(":pictures/params_folder.png");
        case ConstructFolder:
        case BodiesFolder:
          return QIcon(":pictures/constr_folder.png");
        case GroupsFolder:
          return QIcon(":pictures/constr_folder.png");
        case ConstructObject:
        case GroupObject:
        case BodiesObject: {
          std::string aGroup = theIndex.internalId() == ConstructObject ?
            ModelAPI_ResultConstruction::group() : ModelAPI_ResultBody::group();
          ObjectPtr anObject = aPartDoc->object(aGroup, theIndex.row());
          if (anObject && anObject->data() && 
              anObject->data()->execState() == ModelAPI_StateMustBeUpdated) {
            return QIcon(":pictures/constr_object_modified.png");
          }
          return QIcon(":pictures/constr_object.png");
        }
        case HistoryObject: {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_Feature::group(), theIndex.row() - getRowsNumber());
          FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
          if (aFeature)
            return PartSet_DocumentDataModel::featureIcon(aFeature);
        }
      }
      break;
    case Qt::ToolTipRole:
      // return Tooltip
      break;
    case Qt::ForegroundRole:
      if (theIndex.internalId() == HistoryObject) {
        if (theIndex.row() > lastHistoryRow())
          return QBrush(Qt::lightGray);
      }
      return QBrush(myItemsColor);
  }
  return QVariant();
}

QVariant PartSet_PartDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

int PartSet_PartDataModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid()) {
    DocumentPtr aDoc = partDocument();
    if (aDoc.get()) {
      return getRowsNumber() + aDoc->size(ModelAPI_Feature::group());
    } else 
      return 0;
  }
  switch (parent.internalId()) {
    case ParamsFolder:
      return partDocument()->size(ModelAPI_ResultParameter::group());
    case ConstructFolder:
      return partDocument()->size(ModelAPI_ResultConstruction::group());
    case BodiesFolder:
      return partDocument()->size(ModelAPI_ResultBody::group());
    case GroupsFolder:
      return partDocument()->size(ModelAPI_ResultGroup::group());
  }
  return 0;
}

int PartSet_PartDataModel::columnCount(const QModelIndex &parent) const
{
  return 2;
}

QModelIndex PartSet_PartDataModel::index(int theRow, int theColumn, const QModelIndex &theParent) const
{
  if (!theParent.isValid()) {
    switch (theRow) {
      case 0:
        return createIndex(theRow, theColumn, (qint32) ParamsFolder);
      case 1:
        return createIndex(theRow, theColumn, (qint32) ConstructFolder);
      case 2:
        return createIndex(theRow, theColumn, (qint32) BodiesFolder);
      case 3:
        {
        int aSize = partDocument()->size(ModelAPI_ResultGroup::group());
        if (aSize > 0)
          return createIndex(theRow, theColumn, (qint32) GroupsFolder);
        else
          return createIndex(theRow, theColumn, (qint32) HistoryObject);
        }
      default:
        return createIndex(theRow, theColumn, (qint32) HistoryObject);
    }
  } else {
    int aId = (int) theParent.internalId();
    switch (aId) {
      case ParamsFolder:
        return createIndex(theRow, theColumn, (qint32) ParamObject);
      case ConstructFolder:
        return createIndex(theRow, theColumn, (qint32) ConstructObject);
      case BodiesFolder:
        return createIndex(theRow, theColumn, (qint32) BodiesObject);
      case GroupsFolder:
        return createIndex(theRow, theColumn, (qint32) GroupObject);
    }
  }
  return QModelIndex();
}

QModelIndex PartSet_PartDataModel::parent(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
    case ParamsFolder:
    case ConstructFolder:
    case BodiesFolder:
    case GroupsFolder:
    case HistoryObject:
      return QModelIndex();

    case ParamObject:
      return createIndex(0, 0, (qint32) ParamsFolder);
    case ConstructObject:
      return createIndex(1, 0, (qint32) ConstructFolder);
    case BodiesObject:
      return createIndex(2, 0, (qint32) BodiesFolder);
    case GroupObject:
      return createIndex(3, 0, (qint32) GroupsFolder);
  }
  return QModelIndex();
}

bool PartSet_PartDataModel::hasChildren(const QModelIndex& theParent) const
{
  return rowCount(theParent) > 0;
}

DocumentPtr PartSet_PartDataModel::partDocument() const
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  ObjectPtr aObject = aRootDoc->object(ModelAPI_Feature::group(), myId);
  FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
  ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFeature->firstResult()); 
  return aPart->partDoc();
}

ObjectPtr PartSet_PartDataModel::object(const QModelIndex& theIndex) const
{
  switch (theIndex.internalId()) {
    case ParamsFolder:
    case ConstructFolder:
    case BodiesFolder:
    case GroupsFolder:
      return ObjectPtr();

    case ParamObject:
      return partDocument()->object(ModelAPI_ResultParameter::group(), theIndex.row());
    case ConstructObject:
      return partDocument()->object(ModelAPI_ResultConstruction::group(), theIndex.row());
    case BodiesObject:
      return partDocument()->object(ModelAPI_ResultBody::group(), theIndex.row());
    case GroupObject:
      return partDocument()->object(ModelAPI_ResultGroup::group(), theIndex.row());
    case HistoryObject:
      return partDocument()->object(ModelAPI_Feature::group(), theIndex.row() - getRowsNumber());
  }
  return ObjectPtr();
}

bool PartSet_PartDataModel::hasDocument(const DocumentPtr& theDoc) const
{
  return (partDocument() == theDoc);
}

QModelIndex PartSet_PartDataModel::findParent(const ObjectPtr& theObject) const
{
  return findGroup(theObject->groupName().c_str());
}

QModelIndex PartSet_PartDataModel::findGroup(const std::string& theGroup) const
{
  if (theGroup == ModelAPI_ResultParameter::group())
    return createIndex(0, 0, (qint32) ParamsFolder);
  if (theGroup == ModelAPI_ResultConstruction::group())
    return createIndex(1, 0, (qint32) ConstructFolder);
  if (theGroup == ModelAPI_ResultBody::group())
    return createIndex(2, 0, (qint32) BodiesFolder);
  if (theGroup == ModelAPI_ResultGroup::group())
    return createIndex(3, 0, (qint32) GroupsFolder);
  return QModelIndex();
}

ResultPartPtr PartSet_PartDataModel::part() const
{
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  ObjectPtr aObj = aRootDoc->object(ModelAPI_ResultPart::group(), myId);
  return std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
}

QModelIndex PartSet_PartDataModel::objectIndex(const ObjectPtr& theObject) const
{
  QModelIndex aIndex;
  if (theObject) {
    if (part() == theObject)
      return aIndex;

    std::string aGroup = theObject->groupName();
    DocumentPtr aDoc = theObject->document();
    int aNb = aDoc->size(aGroup);
    int aRow = -1;
    for (int i = 0; i < aNb; i++) {
      if (aDoc->object(aGroup, i) == theObject) {
        aRow = i;
        break;
      }
    }
    if (aRow == -1)
      return aIndex;
    if (aGroup == ModelAPI_ResultParameter::group())
      return createIndex(aRow, 0, (qint32) ParamObject);
    else if (aGroup == ModelAPI_ResultConstruction::group())
      return createIndex(aRow, 0, (qint32) ConstructObject);
    else if (aGroup == ModelAPI_ResultBody::group())
      return createIndex(aRow, 0, (qint32) BodiesObject);
    else if (aGroup == ModelAPI_ResultGroup::group())
      return createIndex(aRow, 0, (qint32) GroupObject);
    else
      return createIndex(aRow + getRowsNumber(), 0, (qint32) HistoryObject);
  }
  return aIndex;
}


int PartSet_PartDataModel::getRowsNumber() const
{
  int aSize = partDocument()->size(ModelAPI_ResultGroup::group());
  if (aSize == 0) // If there are no groups then do not show group folder
    return 3;
  return 4;
}

int PartSet_PartDataModel::lastHistoryRow() const
{
  DocumentPtr aDoc = partDocument();
  FeaturePtr aFeature = aDoc->currentFeature();
  if (aFeature.get())
    return getRowsNumber() + aDoc->index(aFeature);
  else
    return getRowsNumber() - 1;
}

void PartSet_PartDataModel::setLastHistoryItem(const QModelIndex& theIndex)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = partDocument();
  std::string aOpName = tr("History change").toStdString();
  if (theIndex.internalId() == HistoryObject) {
    ObjectPtr aObject = object(theIndex);
    aMgr->startOperation(aOpName);
    aDoc->setCurrentFeature(std::dynamic_pointer_cast<ModelAPI_Feature>(aObject));
    aMgr->finishOperation();
  } else {
    aMgr->startOperation(aOpName);
    aDoc->setCurrentFeature(FeaturePtr());
    aMgr->finishOperation();
  }
}

QModelIndex PartSet_PartDataModel::lastHistoryItem() const
{
  return index(lastHistoryRow(), 1);
}

Qt::ItemFlags PartSet_PartDataModel::flags(const QModelIndex& theIndex) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsSelectable;
  if (object(theIndex)) {
    aFlags |= Qt::ItemIsEditable;
  }
  if (theIndex.internalId() == HistoryObject) {
    if (theIndex.row() <= lastHistoryRow() || (theIndex.column() == 1))
      aFlags |= Qt::ItemIsEnabled;
  } else
    aFlags |= Qt::ItemIsEnabled;
  return aFlags;
}
