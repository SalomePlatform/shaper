// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

class Config_DataModelReader;
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

  /// Returns an index which is root of the given document
  /// \param theDoc a document
  QModelIndex documentRootIndex(DocumentPtr theDoc, int theColumn = 1) const;

  /// Returns last history object index
  //virtual QModelIndex lastHistoryIndex() const;

  /// Initialises XML data model reader. It must be initialised before DataModel using.
  //void setXMLReader(Config_DataModelReader* theReader) { myXMLReader = theReader; }

  /// Do not processing anymore events of model loop
  //bool blockEventsProcessing(const bool theState);

  /// Returns true if the data model item has Hidden visual state
  /// \param theIndex a tree model item
  /// \return boolean value
  bool hasHiddenState(const QModelIndex& theIndex);

  /// Returns true if the given index exists in data tree
  /// \param theIndex an index to check
  bool hasIndex(const QModelIndex& theIndex) const;

  DocumentPtr document(const QModelIndex& theIndex) const;

  QModelIndex getIndex(ModuleBase_ITreeNode* theNode, int thCol) const;

signals:
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

  /// Find a root index which contains objects of the given document
  /// \param theDoc the document object
  //QModelIndex findDocumentRootIndex(const ModelAPI_Document* theDoc, int aColumn = 1) const;

  /// Returns number of folders in document.
  /// Considered folders which has to be shown only if they are not empty.
  /// \param theDoc document which has to be checked. If 0 then Root document will be considered
  //int foldersCount(ModelAPI_Document* theDoc = 0) const;

  /// Retrurns indexes of folders which can not be shown because they are empty
  /// \param theDoc document which has to be checked. If 0 then Root document will be considered
  //QIntList missedFolderIndexes(ModelAPI_Document* theDoc = 0) const;

  /// Returns Id (row) of a folder taking into consideration
  /// folders which can not be shown non empty
  /// \param theType Type of the folder
  /// \param theDoc a document which contains this folder
  //int folderId(std::string theType, ModelAPI_Document* theDoc = 0) const;

  /// Removes a row from branch of tree
  /// \param theStart - start row to update indexes
  /// \param theSize - number of indexes in the folder
  /// \param theParent - index of parent folder
  //void rebuildBranch(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  /// Returns list of folders types which can not be shown empty
  /// \param fromRoot - root document flag
  //QStringList listOfShowNotEmptyFolders(bool fromRoot = true) const;

  //int getNumberOfFolderItems(const ModelAPI_Folder* theFolder) const;
  //ObjectPtr getObjectInFolder(const ModelAPI_Folder* theFolder, int theId) const;

  //VisibilityState getVisibilityState(const QModelIndex& theIndex) const;

  //void addShownFolder(DocumentPtr theDoc, QString theFolder)
  //{
  //  if (!myShownFolders.contains(theDoc)) {
  //    myShownFolders[theDoc] = QStringList();
  //  }
  //  myShownFolders[theDoc].append(theFolder);
  //}

  //void removeShownFolder(DocumentPtr theDoc, QString theFolder)
  //{
  //  if (myShownFolders.contains(theDoc)) {
  //    myShownFolders[theDoc].removeAll(theFolder);
  //    if (myShownFolders[theDoc].isEmpty())
  //      myShownFolders.remove(theDoc);
  //  }
  //}

  //bool hasShownFolder(DocumentPtr theDoc, QString theFolder) const
  //{
  //  if (myShownFolders.contains(theDoc))
  //    return myShownFolders[theDoc].contains(theFolder);
  //  return false;
  //}

  //Config_DataModelReader* myXMLReader;

  XGUI_Workshop* myWorkshop;
  QMap<DocumentPtr, QStringList> myShownFolders;
  //bool myIsEventsProcessingBlocked;

  ModuleBase_ITreeNode* myRoot;
};

#endif