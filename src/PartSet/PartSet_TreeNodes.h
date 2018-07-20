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

#ifndef PartSet_TreeNodes_H
#define PartSet_TreeNodes_H

#include "PartSet.h"

#include <ModuleBase_ITreeNode.h>
#include <ModelAPI_Feature.h>


/**
* \ingroup Modules
* Implementation of base node for the module data tree
*/
class PartSet_TreeNode : public ModuleBase_ITreeNode
{
public:
  PartSet_TreeNode(ModuleBase_ITreeNode* theParent = 0) : ModuleBase_ITreeNode(theParent) {}

  /// Returns the node representation according to theRole.
  virtual QVariant data(int theColumn, int theRole) const;
};

/**
* \ingroup Modules
* Implementation of a node for object repesentation
*/
class PartSet_ObjectNode : public PartSet_TreeNode
{
public:
  PartSet_ObjectNode(const ObjectPtr& theObj, ModuleBase_ITreeNode* theParent = 0)
    : PartSet_TreeNode(theParent), myObject(theObj) {}

  static std::string typeId()
  {
    static std::string myType = "Object";
    return myType;
  }

  virtual std::string type() const { return typeId(); }

  /// Returns the node representation according to theRole.
  virtual QVariant data(int theColumn, int theRole) const;

  /// Returns properties flag of the item
  virtual Qt::ItemFlags flags(int theColumn) const;

  /// Returns object referenced by the node (can be null)
  virtual ObjectPtr object() const { return myObject; }

  /// Updates sub-nodes of the node
  virtual void update() {}

  virtual VisibilityState visibilityState() const;

protected:
  ObjectPtr myObject;
};

/**
* \ingroup Modules
* Implementation of aFolder node in data tree
*/
class PartSet_FolderNode : public PartSet_TreeNode
{
public:
  enum FolderType {
    ParametersFolder,
    ConstructionFolder,
    PartsFolder,
    ResultsFolder,
    FieldsFolder,
    GroupsFolder
  };

  PartSet_FolderNode(ModuleBase_ITreeNode* theParent, FolderType theType);

  static std::string typeId()
  {
    static std::string myType = "Folder";
    return myType;
  }

  virtual std::string type() const { return typeId(); }

  /// Returns the node representation according to theRole.
  virtual QVariant data(int theColumn, int theRole) const;

  /// Returns properties flag of the item
  virtual Qt::ItemFlags flags(int theColumn) const;

  /// Updates sub-nodes of the node
  virtual void update();

  /// Process creation of objects.
  /// \param theObjects a list of created objects
  /// \return a list of nodes which corresponds to the created objects
  virtual QTreeNodesList objectCreated(const QObjectPtrList& theObjects);

  /// Process deletion of objects.
  /// \param theDoc a document where objects were deleted
  /// \param theGroup a name of group where objects were deleted
  virtual QTreeNodesList objectsDeleted(const DocumentPtr& theDoc, const QString& theGroup);

  QString name() const;

  /// Returns a node which belongs to the given document and contains objects of the given group
  /// \param theDoc a document
  /// \param theGroup a name of objects group
  /// \return a parent node if it is found
  virtual ModuleBase_ITreeNode* findParent(const DocumentPtr& theDoc, QString theGroup)
  {
    if ((theDoc == document()) && (theGroup.toStdString() == groupName()))
      return this;
    return 0;
  }

private:
  std::string groupName() const;

  FolderType myType;
};

/////////////////////////////////////////////////////////////////////
/**
* \ingroup Modules
* A base class for root folders
*/
class PartSet_FeatureFolderNode : public PartSet_TreeNode
{
public:
  PartSet_FeatureFolderNode(ModuleBase_ITreeNode* theParent = 0) : PartSet_TreeNode(theParent) {}

  /// Process creation of objects.
  /// \param theObjects a list of created objects
  /// \return a list of nodes which corresponds to the created objects
  virtual QTreeNodesList objectCreated(const QObjectPtrList& theObjects);

  /// Process deletion of objects.
  /// \param theDoc a document where objects were deleted
  /// \param theGroup a name of group where objects were deleted
  virtual QTreeNodesList objectsDeleted(const DocumentPtr& theDoc, const QString& theGroup);

  /// Returns a node which belongs to the given document and contains objects of the given group
  /// \param theDoc a document
  /// \param theGroup a name of objects group
  /// \return a parent node if it is found
  virtual ModuleBase_ITreeNode* findParent(const DocumentPtr& theDoc, QString theGroup);

protected:
  virtual ModuleBase_ITreeNode* createNode(const ObjectPtr& theObj) = 0;

  virtual int numberOfFolders() const { return 0; }
};


/////////////////////////////////////////////////////////////////////
/**
* \ingroup Modules
* Implementation of Root node in data tree
*/
class PartSet_RootNode : public PartSet_FeatureFolderNode
{
public:
  PartSet_RootNode();

  static std::string typeId()
  {
    static std::string myType = "PartSetRoot";
    return myType;
  }

  virtual std::string type() const { return typeId(); }

  /// Updates sub-nodes of the node
  virtual void update();

  virtual ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  /// Returns document object of the sub-tree.
  virtual DocumentPtr document() const;

  void setWorkshop(ModuleBase_IWorkshop* theWork) { myWorkshop = theWork; }

protected:
  virtual ModuleBase_ITreeNode* createNode(const ObjectPtr& theObj);

  virtual int numberOfFolders() const { return 3; }

private:
  PartSet_FolderNode* myParamsFolder;
  PartSet_FolderNode* myConstrFolder;
  PartSet_FolderNode* myPartsFolder;

  ModuleBase_IWorkshop* myWorkshop;
};

/////////////////////////////////////////////////////////////////////
/**
* \ingroup Modules
* Implementation of Root node of a Part document in data tree
*/
class PartSet_PartRootNode : public PartSet_FeatureFolderNode
{
public:
  PartSet_PartRootNode(const ObjectPtr& theObj, ModuleBase_ITreeNode* theParent);

  static std::string typeId()
  {
    static std::string myType = "PartRoot";
    return myType;
  }

  virtual std::string type() const { return typeId(); }

  /// Returns object referenced by the node (can be null)
  virtual ObjectPtr object() const { return myObject; }

  /// Returns document object of the sub-tree.
  virtual DocumentPtr document() const;

  /// Updates sub-nodes of the node
  virtual void update();

  /// Returns the node representation according to theRole.
  virtual QVariant data(int theColumn, int theRole) const;

  /// Returns properties flag of the item
  virtual Qt::ItemFlags flags(int theColumn) const;

  /// Process creation of objects.
  /// \param theObjects a list of created objects
  /// \return a list of nodes which corresponds to the created objects
  virtual QTreeNodesList objectCreated(const QObjectPtrList& theObjects);

  /// Process deletion of objects.
  /// \param theDoc a document where objects were deleted
  /// \param theGroup a name of group where objects were deleted
  virtual QTreeNodesList objectsDeleted(const DocumentPtr& theDoc, const QString& theGroup);

protected:
  virtual ModuleBase_ITreeNode* createNode(const ObjectPtr& theObj);

  virtual int numberOfFolders() const;

  virtual void deleteChildren();

private:
  PartSet_FolderNode* myParamsFolder;
  PartSet_FolderNode* myConstrFolder;
  PartSet_FolderNode* myResultsFolder;
  PartSet_FolderNode* myFieldsFolder;
  PartSet_FolderNode* myGroupsFolder;

  ObjectPtr myObject;
};

/////////////////////////////////////////////////////////////////////
/**
* \ingroup Modules
* Implementation of a folder which corresponds to ModelAPI_Folder object
*/
class PartSet_ObjectFolderNode : public PartSet_ObjectNode
{
public:
  PartSet_ObjectFolderNode(const ObjectPtr& theObj, ModuleBase_ITreeNode* theParent)
    : PartSet_ObjectNode(theObj, theParent) {}

  static std::string typeId()
  {
    static std::string myType = "ObjectFolder";
    return myType;
  }

  virtual std::string type() const { return typeId(); }

  /// Updates sub-nodes of the node
  virtual void update();

  /// Process creation of objects.
  /// \param theObjects a list of created objects
  /// \return a list of nodes which corresponds to the created objects
  virtual QTreeNodesList objectCreated(const QObjectPtrList& theObjects);

  /// Process deletion of objects.
  /// \param theDoc a document where objects were deleted
  /// \param theGroup a name of group where objects were deleted
  virtual QTreeNodesList objectsDeleted(const DocumentPtr& theDoc, const QString& theGroup);

private:
  FeaturePtr getFeature(const std::string& theId) const;

  void getFirstAndLastIndex(int& theFirst, int& theLast) const;
};

#endif