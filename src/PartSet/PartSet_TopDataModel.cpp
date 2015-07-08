// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "PartSet_TopDataModel.h"
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
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

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

