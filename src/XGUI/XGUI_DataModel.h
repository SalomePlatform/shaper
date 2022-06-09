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

#ifndef XGUI_DataModel_H
#define XGUI_DataModel_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Document.h>
#include <Events_Listener.h>

#include <QAbstractItemModel>

class XGUI_Workshop;
class ModuleBase_ITreeNode;

/**\class XGUI_DataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It uses XML file for definition of data tree.
 * Some tips about organization of the model:
 * - Non Valid Index - root index
 * - An index with internal Id == -1 is a folder of root document
 * - An index which contains internal pointer as ModelAPI_Object its the object
 * - An index which contains internal pointer as ModelAPI_Document is 
 *   a folder which belongs to this document
 */
class XGUI_EXPORT XGUI_DataModel : public QAbstractItemModel, public Events_Listener
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent object
  XGUI_DataModel(QObject* theParent);

  /// Destructor
  virtual ~XGUI_DataModel();


  void setRoot(ModuleBase_ITreeNode* theRoot) { myRoot = theRoot; }

  ModuleBase_ITreeNode* root() const { return myRoot; }

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  //! Returns an object by the given Model index.
  //! Returns 0 if the given index is not index of an object
  virtual ObjectPtr object(const QModelIndex& theIndex) const;

  //! Returns index of the object
  //! \param theObject object to find
  virtual QModelIndex objectIndex(const ObjectPtr theObject, int theColumn = 1) const;

  //! Clear internal data
  virtual void clear();

  //! Rebuild data tree
  virtual void rebuildDataTree();

  /// Returns the data stored under the given role for the item referred to by the index.
  /// \param theIndex a model index
  /// \param theRole a data role (see Qt::ItemDataRole)
  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;

  /// Returns the data for the given role and section in the header with the specified orientation.
  /// \param theSection a section
  /// \param theOrient an orientation
  /// \param theRole a data role (see Qt::ItemDataRole)
  virtual QVariant headerData(int theSection, Qt::Orientation theOrient, int theRole =
                                  Qt::DisplayRole) const;

  /// Returns the number of rows under the given parent. When the parent is valid it means that
  /// rowCount is returning the number of children of parent.
  /// \param theParent a parent model index
  virtual int rowCount(const QModelIndex& theParent = QModelIndex()) const;

  /// Returns the number of columns for the children of the given parent.
  /// It has a one column
  /// \param theParent a parent model index
  virtual int columnCount(const QModelIndex& theParent = QModelIndex()) const;

  /// Returns the index of the item in the model specified by the given row,
  /// column and parent index.
  /// \param theRow a row
  /// \param theColumn a column
  /// \param theParent a parent model index
  virtual QModelIndex index(int theRow, int theColumn, const QModelIndex &theParent =
                                QModelIndex()) const;

  /// Returns the parent of the model item with the given index.
  /// If the item has no parent, an invalid QModelIndex is returned.
  /// \param theIndex a model index
  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  /// Returns true if parent has any children; otherwise returns false.
  /// \param theParent a parent model index
  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

  /// Inserts count rows into the model before the given row.
  /// Items in the new row will be children of the item represented by the parent model index.
  /// \param theRow a start row
  /// \param theCount a nember of rows to insert
  /// \param theParent a parent model index
  virtual bool insertRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  /// Removes count rows starting with the given row under parent parent from the model.
  /// \param theRow a start row
  /// \param theCount a nember of rows to remove
  /// \param theParent a parent model index
  virtual bool removeRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  /// Returns the item flags for the given index.
  /// \param theIndex a model index
  virtual Qt::ItemFlags flags(const QModelIndex& theIndex) const;

  /// Returns true if a model can accept a drop of the data (used for drag and drop functionality).
  virtual bool canDropMimeData(const QMimeData *theData, Qt::DropAction theAction,
                               int theRow, int theColumn, const QModelIndex &theParent) const override;

  /// Converts the dragged items information into mime data format (to be encoded in dropMimeData)
  virtual QMimeData* mimeData(const QModelIndexList &indexes) const override;

  /// Performs a drag and drop of Part feature operation when it is droped.
  virtual bool dropMimeData(const QMimeData *theData, Qt::DropAction theAction,
                            int theRow, int theColumn, const QModelIndex &theParent) override;

  /// Returns an index which is root of the given document
  /// \param theDoc a document
  QModelIndex documentRootIndex(DocumentPtr theDoc, int theColumn = 1) const;

  /// Returns true if the data model item has Hidden visual state
  /// \param theIndex a tree model item
  /// \return boolean value
  bool hasHiddenState(const QModelIndex& theIndex);

  /// Returns true if the given index exists in data tree
  /// \param theIndex an index to check
  bool hasIndex(const QModelIndex& theIndex) const;

  /// Returns true if the given node is a parto of the data tree
  /// \param theNode a node to check
  bool hasNode(ModuleBase_ITreeNode* theNode) const;

  /// Returns document of the given index
  /// \param theIndex an index to check
  DocumentPtr document(const QModelIndex& theIndex) const;

  /// Returns index for the node and column
  /// \param theNode a node
  /// \param thCol a column
  QModelIndex getIndex(ModuleBase_ITreeNode* theNode, int thCol) const;

  /// Allows to support drag and drop of some model items
  virtual Qt::DropActions supportedDropActions() const override
  {
    return Qt::MoveAction;
  }

signals:
  /// Signal send before tree rebuild
  void beforeTreeRebuild();

  /// Signal about tree had been rebuilt
  void treeRebuilt();

private:
  enum VisibilityState {
    NoneState,
    Visible,
    SemiVisible,
    Hidden };


  QModelIndex getParentIndex(ModuleBase_ITreeNode* theNode, int thCol) const;

  void updateSubTree(ModuleBase_ITreeNode* theParent);

  XGUI_Workshop* myWorkshop;
  QMap<DocumentPtr, QStringList> myShownFolders;

  ModuleBase_ITreeNode* myRoot;
};

#endif