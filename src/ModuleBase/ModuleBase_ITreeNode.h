// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModuleBase_ITreeNode_H
#define ModuleBase_ITreeNode_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Document.h>

#include <QList>
#include <QString>
#include <QIcon>
#include <QVariant>

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif

class ModuleBase_ITreeNode;
class ModuleBase_IWorkshop;

typedef QList<ModuleBase_ITreeNode*> QTreeNodesList;

class ModuleBase_ITreeNode
{
public:
  enum VisibilityState {
    NoneState,
    Visible,
    SemiVisible,
    Hidden
  };

  /// Default constructor
  ModuleBase_ITreeNode(ModuleBase_ITreeNode* theParent = 0) : myParent(theParent) {}

  virtual ~ModuleBase_ITreeNode() { deleteChildren(); }

  virtual std::string type() const = 0;

  /// Returns the node representation according to theRole.
  virtual QVariant data(int theColumn, int theRole) const { return QVariant(); }

  /// Returns properties flag of the item
  virtual Qt::ItemFlags flags(int theColumn) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }

  /// Returns parent node of the current node
  ModuleBase_ITreeNode* parent() const { return myParent; }

  /// Returns list of the node children
  QTreeNodesList children() const { return myChildren; }

  /// Returns a children node according to given row (index)
  ModuleBase_ITreeNode* subNode(int theRow) const
  {
    if ((theRow > -1) && (theRow < myChildren.length()))
      return myChildren.at(theRow);
    return 0;
  }

  /// Finds a node which contains the referenced object
  /// \param theObj an object to find
  /// \param allLevels if true then all sub-trees will be processed
  ModuleBase_ITreeNode* subNode(const ObjectPtr& theObj, bool allLevels = true) const
  {
    foreach(ModuleBase_ITreeNode* aNode, myChildren) {
      if (aNode->object() == theObj)
        return aNode;
      if (allLevels) {
        ModuleBase_ITreeNode* aSubNode = aNode->subNode(theObj, allLevels);
        if (aSubNode)
          return aSubNode;
      }
    }
    return 0;
  }

  /// Returns true if the given node is found within children
  /// \param theNode a node to find
  /// \param allLevels if true then all sub-trees will be processed
  bool hasSubNode(ModuleBase_ITreeNode* theNode, bool allLevels = true) const
  {
    foreach(ModuleBase_ITreeNode* aNode, myChildren) {
      if (aNode == theNode)
        return true;
      if (allLevels) {
        if (aNode->hasSubNode(theNode))
          return true;
      }
    }
    return false;
  }

  /// Returns number of children
  int childrenCount() const { return myChildren.length(); }

  int nodeRow(ModuleBase_ITreeNode* theNode) const { return myChildren.indexOf(theNode); }

  /// Returns object referenced by the node (can be null)
  virtual ObjectPtr object() const { return ObjectPtr(); }

  /// Updates all sub-nodes of the node (checks whole sub-tree)
  virtual void update() {}

  /// Process creation of objects.
  /// \param theObjects a list of created objects
  /// \return a list of nodes which corresponds to the created objects
  virtual QTreeNodesList objectCreated(const QObjectPtrList& theObjects) {
    return QTreeNodesList();
  }

  /// Process deletion of objects.
  /// \param theDoc a document where objects were deleted
  /// \param theGroup a name of group where objects were deleted
  /// \return a list of parents where nodes were deleted
  virtual QTreeNodesList objectsDeleted(const DocumentPtr& theDoc, const QString& theGroup)
  { return QTreeNodesList(); }

  /// Returns workshop object. Has to be reimplemented in a root node
  virtual ModuleBase_IWorkshop* workshop() const { return parent()->workshop(); }

  /// Returns document object of the sub-tree. Has to be reimplemented in sub-tree root object
  virtual DocumentPtr document() const { return parent()->document(); }

  /// Returns a node which belongs to the given document and contains objects of the given group
  /// \param theDoc a document
  /// \param theGroup a name of objects group
  /// \return a parent node if it is found
  virtual ModuleBase_ITreeNode* findParent(const DocumentPtr& theDoc, QString theGroup)
  { return 0; }

  /// Returns root node of a data tree of the given document
  /// \param theDoc a document
  /// \return a tree node which is a root of the document structure
  virtual ModuleBase_ITreeNode* findRoot(const DocumentPtr& theDoc)
  {
    if (document() == theDoc)
      return this;
    ModuleBase_ITreeNode* aRoot;
    foreach(ModuleBase_ITreeNode* aNode, myChildren) {
      aRoot = aNode->findRoot(theDoc);
      if (aRoot)
        return aRoot;
    }
    return 0;
  }

  /// Returns visibilitystate of the node in viewer 3d
  virtual VisibilityState visibilityState() const { return NoneState; }

protected:

  /// deletes all children nodes (called in destructor.)
  virtual void deleteChildren()
  {
    while (myChildren.size()) {
      ModuleBase_ITreeNode* aNode = myChildren.takeLast();
      delete aNode;
    }
  }

  void sortChildren() {
    if (myChildren.size() > 1) {
      int i = 0;
      ModuleBase_ITreeNode* aNode = 0;
      ObjectPtr aObject;
      int aIdx;
      int aCount = 0;
      int aSize = myChildren.size();
      int aMaxCount = aSize * aSize;
      int aShift = 0;
      while (i < aSize) {
        aCount++;
        // To avoid unlimited cycling
        if (aCount > aMaxCount)
          break;

        aNode = myChildren.at(i);
        aObject = aNode->object();
        if (aObject.get()) {
          aIdx = aObject->document()->index(aObject, true) + aShift;
          if (aIdx != i) {
            myChildren.removeAll(aNode);
            myChildren.insert(aIdx, aNode);
            i = 0;
            continue;
          }
        }
        else
          aShift++;
        i++;
      }
    }
  }

  ModuleBase_ITreeNode* myParent; //!< Parent of the node
  QTreeNodesList myChildren; //!< Children of the node
};

#endif