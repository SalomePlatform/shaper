// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "PartSet_TreeNodes.h"

#include <ModuleBase_IconFactory.h>
#include <ModuleBase_IWorkshop.h>

#include <PartSetPlugin_Part.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Folder.h>
#include <ModelAPI_AttributeReference.h>

#include <QBrush>


#define ACTIVE_COLOR QColor(Qt::black)
#define SELECTABLE_COLOR QColor(100, 100, 100)
#define DISABLED_COLOR QColor(200, 200, 200)

Qt::ItemFlags aNullFlag;
Qt::ItemFlags aDefaultFlag = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
Qt::ItemFlags aEditingFlag = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;


ResultPartPtr getPartResult(const ObjectPtr& theObj)
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature) {
    ResultPtr aRes = aFeature->firstResult();
    if (aRes.get() && (aRes->groupName() == ModelAPI_ResultPart::group())) {
      ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aRes);
      // Use only original parts, not a placement results
      if (aPartRes == aPartRes->original())
        return aPartRes;
    }
  }
  return ResultPartPtr();
}

bool isCurrentFeature(const ObjectPtr& theObj)
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aCurDoc = aSession->activeDocument();
  FeaturePtr aFeature = aCurDoc->currentFeature(true);
  return aFeature == theObj;
}

//////////////////////////////////////////////////////////////////////////////////
QVariant PartSet_TreeNode::data(int theColumn, int theRole) const
{
  if ((theColumn == 1) && (theRole == Qt::ForegroundRole)) {
    Qt::ItemFlags aFlags = flags(theColumn);
    if (aFlags == Qt::ItemFlags())
      return QBrush(DISABLED_COLOR);
    if (!aFlags.testFlag(Qt::ItemIsEditable))
      return QBrush(SELECTABLE_COLOR);
    return ACTIVE_COLOR;
  }
  return ModuleBase_ITreeNode::data(theColumn, theRole);
}


//////////////////////////////////////////////////////////////////////////////////
QVariant PartSet_ObjectNode::data(int theColumn, int theRole) const
{
  switch (theRole) {
  case Qt::DisplayRole:
    if (theColumn == 1) {
      if (myObject->groupName() == ModelAPI_ResultParameter::group()) {
        ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(myObject);
        AttributeDoublePtr aValueAttribute =
          aParam->data()->real(ModelAPI_ResultParameter::VALUE());
        QString aVal = QString::number(aValueAttribute->value());
        QString aTitle = QString(myObject->data()->name().c_str());
        return aTitle + " = " + aVal;
      }
      return myObject->data()->name().c_str();
    }
    break;
  case Qt::DecorationRole:
    switch (theColumn) {
    case 0:
      switch (visibilityState()) {
      case NoneState:
        return QIcon();
      case Visible:
        return QIcon(":pictures/eyeopen.png");
      case SemiVisible:
        return QIcon(":pictures/eyemiclosed.png");
      case Hidden:
        return QIcon(":pictures/eyeclosed.png");
      }
    case 1:
      if (myObject->groupName() == ModelAPI_Folder::group())
        return QIcon(":pictures/features_folder.png");
      else
        return ModuleBase_IconFactory::get()->getIcon(myObject);
    case 2:
      if (isCurrentFeature(myObject))
        return QIcon(":pictures/arrow.png");
    }
  }
  return PartSet_TreeNode::data(theColumn, theRole);
}

Qt::ItemFlags PartSet_ObjectNode::flags(int theColumn) const
{
  if (myObject->isDisabled()) {
    return (theColumn == 2) ? Qt::ItemIsSelectable : aNullFlag;
  } else {
    DocumentPtr aDoc = myObject->document();
    SessionPtr aSession = ModelAPI_Session::get();
    if (aSession->activeDocument() == aDoc)
      return aEditingFlag;
  }
  return aDefaultFlag;
}

PartSet_ObjectNode::VisibilityState PartSet_ObjectNode::visibilityState() const
{
  Qt::ItemFlags aFlags = flags(1);
  if (aFlags == Qt::ItemFlags())
    return NoneState;

  if (myObject->groupName() == ModelAPI_ResultParameter::group())
    return NoneState;
  ResultPtr aResObj = std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aResObj.get()) {
    ModuleBase_IWorkshop* aWork = workshop();
    ResultCompSolidPtr aCompRes = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aResObj);
    if (aCompRes.get()) {
      VisibilityState aState = aCompRes->numberOfSubs(true) == 0 ?
        (aWork->isVisible(aCompRes) ? Visible : Hidden) : NoneState;
      for (int i = 0; i < aCompRes->numberOfSubs(true); i++) {
        ResultPtr aSubRes = aCompRes->subResult(i, true);
        VisibilityState aS = aWork->isVisible(aSubRes) ? Visible : Hidden;
        if (aState == NoneState)
          aState = aS;
        else if (aState != aS) {
          aState = SemiVisible;
          break;
        }
      }
      return aState;
    } else {
      if (aWork->isVisible(aResObj))
        return Visible;
      else
        return Hidden;
    }
  }
  return NoneState;
}


//////////////////////////////////////////////////////////////////////////////////
PartSet_FolderNode::PartSet_FolderNode(ModuleBase_ITreeNode* theParent,
  FolderType theType)
  : PartSet_TreeNode(theParent), myType(theType)
{
}

QString PartSet_FolderNode::name() const
{
  switch (myType) {
  case ParametersFolder:
    return QObject::tr("Parameters");
  case ConstructionFolder:
    return QObject::tr("Constructions");
  case PartsFolder:
    return QObject::tr("Parts");
  case ResultsFolder:
    return QObject::tr("Results");
  case FieldsFolder:
    return QObject::tr("Fields");
  case GroupsFolder:
    return QObject::tr("Groups");
  }
  return "NoName";
}


QVariant PartSet_FolderNode::data(int theColumn, int theRole) const
{
  static QIcon aParamsIco(":pictures/params_folder.png");
  static QIcon aConstrIco(":pictures/constr_folder.png");

  if (theColumn == 1) {
    switch (theRole) {
    case Qt::DisplayRole:
      return name() + QString(" (%1)").arg(childrenCount());
    case Qt::DecorationRole:
      switch (myType) {
      case ParametersFolder:
        return aParamsIco;
      case ConstructionFolder:
        return aConstrIco;
      case PartsFolder:
        return aConstrIco;
      case ResultsFolder:
        return aConstrIco;
      case FieldsFolder:
        return aConstrIco;
      case GroupsFolder:
        return aConstrIco;
      }
    }
  }
  return PartSet_TreeNode::data(theColumn, theRole);
}

Qt::ItemFlags PartSet_FolderNode::flags(int theColumn) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aActiveDoc = aSession->activeDocument();
  if (theColumn == 1) {
    if (document() == aActiveDoc)
      return aEditingFlag;
  }
  return aDefaultFlag;
}

void PartSet_FolderNode::update()
{
  DocumentPtr aDoc = document();
  if (!aDoc.get())
    return;

  // Remove extra sub-nodes
  QTreeNodesList aDelList;
  int aIndex;
  int aId = -1;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    aId++;
    aIndex = aDoc->index(aNode->object(), true);
    if ((aIndex == -1) || (aId != aIndex))
      aDelList.append(aNode);
  }
  foreach(ModuleBase_ITreeNode* aNode, aDelList) {
    myChildren.removeAll(aNode);
    delete aNode;
  }

  // Add new nodes
  std::string aGroup = groupName();
  int aSize = aDoc->size(aGroup, true);
  for (int i = 0; i < aSize; i++) {
    ObjectPtr aObj = aDoc->object(aGroup, i, true);
    if (i < myChildren.size()) {
      if (myChildren.at(i)->object() != aObj) {
        PartSet_ObjectNode* aNode = new PartSet_ObjectNode(aObj, this);
        myChildren.insert(i, aNode);
      }
    } else {
      PartSet_ObjectNode* aNode = new PartSet_ObjectNode(aObj, this);
      myChildren.append(aNode);
    }
  }
}

std::string PartSet_FolderNode::groupName() const
{
  switch (myType) {
  case ParametersFolder:
    return ModelAPI_ResultParameter::group();
  case ConstructionFolder:
    return ModelAPI_ResultConstruction::group();
  case PartsFolder:
    return ModelAPI_ResultPart::group();
  case ResultsFolder:
    return ModelAPI_ResultBody::group();
  case FieldsFolder:
    return ModelAPI_ResultField::group();
  case GroupsFolder:
    return ModelAPI_ResultGroup::group();
  }
  return "";
}

QTreeNodesList PartSet_FolderNode::objectCreated(const QObjectPtrList& theObjects)
{
  QTreeNodesList aResult;
  std::string aName = groupName();
  DocumentPtr aDoc = document();
  int aIdx = -1;
  foreach(ObjectPtr aObj, theObjects) {
    if ((aObj->document() == aDoc) && (aObj->groupName() == aName)) {
      aIdx = aDoc->index(aObj, true);
      if (aIdx != -1) {
        bool aHasObject = (aIdx < myChildren.size()) && (myChildren.at(aIdx)->object() == aObj);
        if (!aHasObject) {
          PartSet_ObjectNode* aNode = new PartSet_ObjectNode(aObj, this);
          aResult.append(aNode);
          if (aIdx < myChildren.size())
            myChildren.insert(aIdx, aNode);
          else
            myChildren.append(aNode);
        }
      }
    }
  }
  return aResult;
}

QTreeNodesList PartSet_FolderNode::objectsDeleted(const DocumentPtr& theDoc,
  const QString& theGroup)
{
  DocumentPtr aDoc = document();
  QTreeNodesList aResult;
  if ((theGroup.toStdString() == groupName()) && (theDoc == aDoc)) {
    QTreeNodesList aDelList;
    int aIndex;
    int aId = -1;
    foreach(ModuleBase_ITreeNode* aNode, myChildren) {
      aId++;
      aIndex = aDoc->index(aNode->object(), true);
      if ((aIndex == -1) || (aId != aIndex))
        aDelList.append(aNode);
    }
    if (aDelList.size() > 0) {
      foreach(ModuleBase_ITreeNode* aNode, aDelList) {
        myChildren.removeAll(aNode);
        delete aNode;
      }
      aResult.append(this);
    }
  }
  return aResult;
}

//////////////////////////////////////////////////////////////////////////////////
QTreeNodesList PartSet_FeatureFolderNode::objectCreated(const QObjectPtrList& theObjects)
{
  QTreeNodesList aResult;
  // Process all folders
  ModuleBase_ITreeNode* aFoder = 0;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    if ((aNode->type() == PartSet_FolderNode::typeId()) ||
      (aNode->type() == PartSet_PartRootNode::typeId())) { // aFolder node
      QTreeNodesList aList = aNode->objectCreated(theObjects);
      if (aList.size() > 0)
        aResult.append(aList);
    }
  }
  // Process the root sub-objects
  DocumentPtr aDoc = document();
  int aIdx = -1;
  int aNb = numberOfFolders();
  foreach(ObjectPtr aObj, theObjects) {
    if (aDoc == aObj->document()) {
      if ((aObj->groupName() == ModelAPI_Feature::group()) ||
        (aObj->groupName() == ModelAPI_Folder::group())){
        ModuleBase_ITreeNode* aNode = createNode(aObj);
        aIdx = aDoc->index(aObj, true);
        if (aIdx != -1) {
          aIdx += aNb;
          bool aHasObject = (aIdx < myChildren.size()) && (myChildren.at(aIdx)->object() == aObj);
          if (!aHasObject) {
            if (aIdx < myChildren.size())
              myChildren.insert(aIdx, aNode);
            else
              myChildren.append(aNode);
            aResult.append(aNode);
          }
        }
      }
    }
  }
  // Update sub-folders
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    if ((aNode->type() == PartSet_ObjectFolderNode::typeId()) ||
      (aNode->type() == PartSet_PartRootNode::typeId()))
      aResult.append(aNode->objectCreated(theObjects));
  }
  return aResult;
}

QTreeNodesList PartSet_FeatureFolderNode::objectsDeleted(const DocumentPtr& theDoc,
  const QString& theGroup)
{
  QTreeNodesList aResult;

  // Process sub-folders
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    if (aNode->childrenCount() > 0) { // aFolder node
      QTreeNodesList aList = aNode->objectsDeleted(theDoc, theGroup);
      if (aList.size() > 0)
        aResult.append(aList);
    }
  }

  // Process root
  DocumentPtr aDoc = document();
  int aNb = numberOfFolders();
  bool isGroup = ((theGroup.toStdString() == ModelAPI_Feature::group()) ||
    (theGroup.toStdString() == ModelAPI_Folder::group()));
  if ((theDoc == aDoc) && isGroup) {
    QTreeNodesList aDelList;
    int aIndex;
    int aId = -1;
    foreach(ModuleBase_ITreeNode* aNode, myChildren) {
      aId++;
      if (aNode->object().get()) {
        aIndex = aDoc->index(aNode->object(), true);
        if ((aIndex == -1) || (aId != (aIndex + aNb)))
          aDelList.append(aNode);
      }
    }
    if (aDelList.size() > 0) {
      foreach(ModuleBase_ITreeNode* aNode, aDelList) {
        myChildren.removeAll(aNode);
        delete aNode;
      }
      aResult.append(this);
    }
  }
  return aResult;
}

ModuleBase_ITreeNode* PartSet_FeatureFolderNode::findParent(const DocumentPtr& theDoc,
  QString theGroup)
{
  ModuleBase_ITreeNode* aResult = 0;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    aResult = aNode->findParent(theDoc, theGroup);
    if (aResult) {
      return aResult;
    }
  }
  bool isGroup = ((theGroup.toStdString() == ModelAPI_Feature::group()) ||
    (theGroup.toStdString() == ModelAPI_Folder::group()));
  if ((theDoc == document()) && isGroup)
    return this;
  return 0;
}


//////////////////////////////////////////////////////////////////////////////////
PartSet_RootNode::PartSet_RootNode() : PartSet_FeatureFolderNode(0), myWorkshop(0)
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDoc = aSession->moduleDocument();

  myParamsFolder = new PartSet_FolderNode(this, PartSet_FolderNode::ParametersFolder);
  myConstrFolder = new PartSet_FolderNode(this, PartSet_FolderNode::ConstructionFolder);
  myPartsFolder = new PartSet_FolderNode(this, PartSet_FolderNode::PartsFolder);

  myChildren.append(myParamsFolder);
  myChildren.append(myConstrFolder);
  myChildren.append(myPartsFolder);

  update();
}


void PartSet_RootNode::update()
{
  myParamsFolder->update();
  myConstrFolder->update();
  myPartsFolder->update();

  // Update features content
  DocumentPtr aDoc = document();
  int aNb = numberOfFolders();

  // Remove extra sub-nodes
  QTreeNodesList aDelList;
  int aIndex;
  int aId = -1;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    aId++;
    if (aNode->object().get()) {
      aIndex = aDoc->index(aNode->object(), true);
      if ((aIndex == -1) || (aId != (aIndex + aNb)))
        aDelList.append(aNode);
    }
  }
  foreach(ModuleBase_ITreeNode* aNode, aDelList) {
    myChildren.removeAll(aNode);
    delete aNode;
  }

  // Add new nodes
  std::string aGroup = ModelAPI_Feature::group();
  int aSize = aDoc->size(aGroup, true);
  FeaturePtr aFeature;
  for (int i = 0; i < aSize; i++) {
    ObjectPtr aObj = aDoc->object(aGroup, i, true);
    aId = i + aNb; // Take into account existing folders
    if (aId < myChildren.size()) {
      if (myChildren.at(aId)->object() != aObj) {
        ModuleBase_ITreeNode* aNode = createNode(aObj);
        myChildren.insert(aId, aNode);
      }
    } else {
      ModuleBase_ITreeNode* aNode = createNode(aObj);
      myChildren.append(aNode);
    }
  }
  // Update sub-folders
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    if ((aNode->type() == PartSet_ObjectFolderNode::typeId()) ||
      (aNode->type() == PartSet_PartRootNode::typeId()))
      aNode->update();
  }
}

DocumentPtr PartSet_RootNode::document() const
{
  return ModelAPI_Session::get()->moduleDocument();
}

ModuleBase_ITreeNode* PartSet_RootNode::createNode(const ObjectPtr& theObj)
{
  if (theObj->groupName() == ModelAPI_Folder::group())
    return new PartSet_ObjectFolderNode(theObj, this);

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature->getKind() == PartSetPlugin_Part::ID())
    return new PartSet_PartRootNode(theObj, this);

  return new PartSet_ObjectNode(theObj, this);
}

//////////////////////////////////////////////////////////////////////////////////
PartSet_PartRootNode::PartSet_PartRootNode(const ObjectPtr& theObj, ModuleBase_ITreeNode* theParent)
  : PartSet_FeatureFolderNode(theParent), myObject(theObj)
{
  myParamsFolder = new PartSet_FolderNode(this, PartSet_FolderNode::ParametersFolder);
  myConstrFolder = new PartSet_FolderNode(this, PartSet_FolderNode::ConstructionFolder);
  myResultsFolder = new PartSet_FolderNode(this, PartSet_FolderNode::ResultsFolder);
  myFieldsFolder = new PartSet_FolderNode(this, PartSet_FolderNode::FieldsFolder);
  myGroupsFolder = new PartSet_FolderNode(this, PartSet_FolderNode::GroupsFolder);

  myChildren.append(myParamsFolder);
  myChildren.append(myConstrFolder);
  myChildren.append(myResultsFolder);

  update();
}

void PartSet_PartRootNode::deleteChildren()
{
  if (!myFieldsFolder->childrenCount()) {
    delete myFieldsFolder;
  }
  if (!myGroupsFolder->childrenCount()) {
    delete myGroupsFolder;
  }
  PartSet_FeatureFolderNode::deleteChildren();
}


void PartSet_PartRootNode::update()
{
  DocumentPtr aDoc = document();
  if (!aDoc.get())
    return;

  myParamsFolder->update();
  myConstrFolder->update();
  myResultsFolder->update();
  myFieldsFolder->update();
  myGroupsFolder->update();

  bool aHasFields = myFieldsFolder->childrenCount() > 0;
  bool aHasGroups = myGroupsFolder->childrenCount() > 0;
  if (aHasFields && (!myChildren.contains(myFieldsFolder))) {
    myChildren.insert(3, myFieldsFolder);
  }
  if (aHasGroups && (!myChildren.contains(myGroupsFolder))) {
    myChildren.insert(aHasFields ? 4 : 3, myGroupsFolder);
  }

  // Update features content
  int aRows = numberOfFolders();

  // Remove extra sub-nodes
  QTreeNodesList aDelList;
  int aIndex = -1;
  int aId = -1;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    aId++;
    if (aNode->object().get()) {
      aIndex = aDoc->index(aNode->object(), true);
      if ((aIndex == -1) || (aId != (aIndex + aRows)))
        aDelList.append(aNode);
    }
  }
  foreach(ModuleBase_ITreeNode* aNode, aDelList) {
    myChildren.removeAll(aNode);
    delete aNode;
  }

  std::string aGroup = ModelAPI_Feature::group();
  int aSize = aDoc->size(aGroup, true);
  FeaturePtr aFeature;
  for (int i = 0; i < aSize; i++) {
    ObjectPtr aObj = aDoc->object(aGroup, i, true);
    aId = i + aRows; // Take into account existing folders
    if (aId < myChildren.size()) {
      if (myChildren.at(aId)->object() != aObj) {
        ModuleBase_ITreeNode* aNode = createNode(aObj);
        myChildren.insert(aId, aNode);
      }
    } else {
      ModuleBase_ITreeNode* aNode = createNode(aObj);
      myChildren.append(aNode);
    }
  }
  // Update sub-folders
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    if (aNode->type() == PartSet_ObjectFolderNode::typeId())
      aNode->update();
  }
}

DocumentPtr PartSet_PartRootNode::document() const
{
  ResultPartPtr aPartRes = getPartResult(myObject);
  if (aPartRes.get())
    return aPartRes->partDoc();
  return DocumentPtr();
}

QVariant PartSet_PartRootNode::data(int theColumn, int theRole) const
{
  switch (theColumn) {
  case 1:
    switch (theRole) {
    case Qt::DisplayRole:
      return QString(myObject->data()->name().c_str());
    case Qt::DecorationRole:
      return ModuleBase_IconFactory::get()->getIcon(myObject);
    }
  case 2:
    if (theRole == Qt::DecorationRole)
      if (isCurrentFeature(myObject))
        return QIcon(":pictures/arrow.png");
  }
  return PartSet_TreeNode::data(theColumn, theRole);
}

Qt::ItemFlags PartSet_PartRootNode::flags(int theColumn) const
{
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aActiveDoc = aSession->activeDocument();
  if ((aActiveDoc == document()) || (myObject->document() == aActiveDoc))
    return aEditingFlag;
  return aDefaultFlag;
}

ModuleBase_ITreeNode* PartSet_PartRootNode::createNode(const ObjectPtr& theObj)
{
  if (theObj->groupName() == ModelAPI_Folder::group())
    return new PartSet_ObjectFolderNode(theObj, this);
  return new PartSet_ObjectNode(theObj, this);
}

int PartSet_PartRootNode::numberOfFolders() const
{
  int aNb = 3;
  if (myFieldsFolder->childrenCount() > 0)
    aNb++;
  if (myGroupsFolder->childrenCount() > 0)
    aNb++;
  return aNb;
}

QTreeNodesList PartSet_PartRootNode::objectCreated(const QObjectPtrList& theObjects)
{
  QTreeNodesList aResult = PartSet_FeatureFolderNode::objectCreated(theObjects);
  if (!myFieldsFolder->childrenCount()) {
    QTreeNodesList aList = myFieldsFolder->objectCreated(theObjects);
    if (aList.size()) {
      myChildren.insert(3, myFieldsFolder);
      aResult.append(myFieldsFolder);
      aResult.append(aList);
    }
  }
  if (!myGroupsFolder->childrenCount()) {
    QTreeNodesList aList = myGroupsFolder->objectCreated(theObjects);
    if (aList.size()) {
      myChildren.insert(myFieldsFolder->childrenCount()? 4 : 3, myGroupsFolder);
      aResult.append(myGroupsFolder);
      aResult.append(aList);
    }
  }
  return aResult;
}

QTreeNodesList PartSet_PartRootNode::objectsDeleted(const DocumentPtr& theDoc,
  const QString& theGroup)
{
  QTreeNodesList aResult;
  if (myFieldsFolder->childrenCount()) {
    QTreeNodesList aList = myFieldsFolder->objectsDeleted(theDoc, theGroup);
    if (aList.size()) {
      aResult.append(aList);
      if (!myFieldsFolder->childrenCount())
        myChildren.removeAll(myFieldsFolder);
    }
  }
  if (myGroupsFolder->childrenCount()) {
    QTreeNodesList aList = myGroupsFolder->objectsDeleted(theDoc, theGroup);
    if (aList.size()) {
      aResult.append(aList);
      if (!myGroupsFolder->childrenCount())
        myChildren.removeAll(myGroupsFolder);
    }
  }
  aResult.append(PartSet_FeatureFolderNode::objectsDeleted(theDoc, theGroup));
  return aResult;
}

//////////////////////////////////////////////////////////////////////////////////
void PartSet_ObjectFolderNode::update()
{
  int aFirst, aLast;
  getFirstAndLastIndex(aFirst, aLast);
  if ((aFirst == -1) || (aLast == -1)) {
    deleteChildren();
    return;
  }

  int aNbItems = aLast - aFirst + 1;
  if (!aNbItems) {
    deleteChildren();
    return;
  }

  DocumentPtr aDoc = myObject->document();
  // Delete obsolete nodes
  QTreeNodesList aDelList;
  int aId = -1;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    aId++;
    if ((aFirst + aId) < aDoc->size(ModelAPI_Feature::group(), true)) {
      if (aNode->object() != aDoc->object(ModelAPI_Feature::group(), aFirst + aId)) {
        aDelList.append(aNode);
      }
    } else {
      aDelList.append(aNode);
    }
  }
  foreach(ModuleBase_ITreeNode* aNode, aDelList) {
    myChildren.removeAll(aNode);
    delete aNode;
  }

  // Add new nodes
  ModuleBase_ITreeNode* aNode;
  for (int i = 0; i < aNbItems; i++) {
    ObjectPtr aObj = aDoc->object(ModelAPI_Feature::group(), aFirst + i);
    if (i < myChildren.size()) {
      if (aObj != myChildren.at(i)->object()) {
        aNode = new PartSet_ObjectNode(aObj, this);
        myChildren.insert(i, aNode);
      }
    } else {
      aNode = new PartSet_ObjectNode(aObj, this);
      myChildren.append(aNode);
    }
  }
}

QTreeNodesList PartSet_ObjectFolderNode::objectCreated(const QObjectPtrList& theObjects)
{
  QTreeNodesList aResult;
  int aFirst, aLast;
  getFirstAndLastIndex(aFirst, aLast);
  if ((aFirst == -1) || (aLast == -1)) {
    return aResult;
  }
  int aNbItems = aLast - aFirst + 1;
  if (!aNbItems) {
    return aResult;
  }
  DocumentPtr aDoc = myObject->document();
  // Add new nodes
  ModuleBase_ITreeNode* aNode;
  for (int i = 0; i < aNbItems; i++) {
    ObjectPtr aObj = aDoc->object(ModelAPI_Feature::group(), aFirst + i);
    if (i < myChildren.size()) {
      if (aObj != myChildren.at(i)->object()) {
        aNode = new PartSet_ObjectNode(aObj, this);
        myChildren.insert(i, aNode);
        aResult.append(aNode);
      }
    } else {
      aNode = new PartSet_ObjectNode(aObj, this);
      myChildren.append(aNode);
      aResult.append(aNode);
    }
  }
  return aResult;
}

QTreeNodesList PartSet_ObjectFolderNode::objectsDeleted(const DocumentPtr& theDoc,
  const QString& theGroup)
{
  QTreeNodesList aResult;
  int aFirst, aLast;
  getFirstAndLastIndex(aFirst, aLast);
  if ((aFirst == -1) || (aLast == -1)) {
    return aResult;
  }
  int aNbItems = aLast - aFirst + 1;
  if (!aNbItems) {
    return aResult;
  }
  DocumentPtr aDoc = myObject->document();
  // Delete obsolete nodes
  QTreeNodesList aDelList;
  int aId = -1;
  foreach(ModuleBase_ITreeNode* aNode, myChildren) {
    aId++;
    if ((aFirst + aId) < aDoc->size(ModelAPI_Feature::group(), true)) {
      if (aNode->object() != aDoc->object(ModelAPI_Feature::group(), aFirst + aId)) {
        aDelList.append(aNode);
      }
    } else {
      aDelList.append(aNode);
    }
  }
  if (aDelList.size() > 0) {
    aResult.append(this);
    foreach(ModuleBase_ITreeNode* aNode, aDelList) {
      myChildren.removeAll(aNode);
      delete aNode;
    }
  }
  return aResult;
}

FeaturePtr PartSet_ObjectFolderNode::getFeature(const std::string& theId) const
{
  FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(myObject);
  AttributeReferencePtr aFeatAttr = aFolder->data()->reference(theId);
  if (aFeatAttr)
    return ModelAPI_Feature::feature(aFeatAttr->value());
  return FeaturePtr();
}

void PartSet_ObjectFolderNode::getFirstAndLastIndex(int& theFirst, int& theLast) const
{
  DocumentPtr aDoc = myObject->document();
  FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(myObject);

  FeaturePtr aFirstFeatureInFolder = getFeature(ModelAPI_Folder::FIRST_FEATURE_ID());
  if (!aFirstFeatureInFolder.get()) {
    theFirst = -1;
    return;
  }
  FeaturePtr aLastFeatureInFolder = getFeature(ModelAPI_Folder::LAST_FEATURE_ID());
  if (!aLastFeatureInFolder.get()) {
    theLast = -1;
    return;
  }

  theFirst = aDoc->index(aFirstFeatureInFolder);
  theLast = aDoc->index(aLastFeatureInFolder);
}
