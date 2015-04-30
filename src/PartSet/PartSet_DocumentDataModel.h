// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef PartSet_DocumentDataModel_H
#define PartSet_DocumentDataModel_H

#include "PartSet.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Feature.h>
#include <ModuleBase_IDocumentDataModel.h>

#include <Events_Listener.h>
#include <QList>

class ModelAPI_Document;
class PartSet_PartModel;
class PartSet_TopDataModel;

/**\class PartSet_DocumentDataModel
 * \ingroup GUI
 * \brief This is a proxy data model for Object Browser (QTreeView).
 * It contains several sub-models for generation of each sub-part of data tree.
 */
class PARTSET_EXPORT PartSet_DocumentDataModel : public ModuleBase_IDocumentDataModel, public Events_Listener
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  PartSet_DocumentDataModel(QObject* theParent);
  virtual ~PartSet_DocumentDataModel();

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

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

  /// Returns the index of the item in the model specified by the given row, column and parent index.
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
  bool insertRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  /// Removes count rows starting with the given row under parent parent from the model.
  /// \param theRow a start row
  /// \param theCount a nember of rows to remove
  /// \param theParent a parent model index
  bool removeRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  /// Returns the item flags for the given index.
  /// \param theIndex a model index
  virtual Qt::ItemFlags flags(const QModelIndex& theIndex) const;

  //! Returns an object by the given Model index.
  //! Returns 0 if the given index is not index of an object
  virtual ObjectPtr object(const QModelIndex& theIndex) const;

  //! Returns index of the object
  //! \param theObject object to find
  virtual QModelIndex objectIndex(const ObjectPtr theObject) const;

  //! Returns QModelIndex which corresponds to the given part
  //! If the object is not found then index is not valid
  //! \param thePart a part for analysis
  QModelIndex partIndex(const ResultPartPtr& thePart) const;

  //! Activates a part data model if the index is a Part node index. 
  //! Returns true if active part changed.
  //! \param theIndex a model index
  bool activatePart(const QModelIndex& theIndex);

  //! Retrurns active part
  ResultPartPtr activePart() const;

  //! Retrurns QModelIndex of active part
  QModelIndex activePartIndex() const
  {
    return myActivePartIndex;
  }

  //! Deactivates a Part
  void deactivatePart();

  //! Rebuild data tree
  virtual void rebuildDataTree();

  //! Clear internal data
  virtual void clear();

  //! Set an Index which will be considered as a last history index
  //! \param theIndex a last index for history
  void setLastHistoryItem(const QModelIndex& theIndex);

  QModelIndex lastHistoryItem() const;

  //! Returns icon name according to feature
  static QIcon featureIcon(const FeaturePtr& theFeature);

 private:

  enum
  {
    PartsFolder,
    HistoryNode
  };

  //! Converts QModelIndex of this model to QModelIndex of a one of sub-models.
  QModelIndex* toSourceModelIndex(const QModelIndex& theProxy) const;

  //! Finds a pointer on QModelIndex which is equal to the given one
  QModelIndex* findModelIndex(const QModelIndex& theIndex) const;

  //! Returns pointer on QModelIndex which is equal to the given one.
  QModelIndex* getModelIndex(const QModelIndex& theIndex) const;

  //! Deletes all saved pointers on QModelIndex objects.
  void clearModelIndexes();

  //! Deletes all saved pointers on QModelIndex objects.
  void clearSubModels();

  //! Removes sub-model on removing a part object. Also it removes QModelIndex-es which refer to this model
  void removeSubModel(int theModelId);

  //! Returns true if the given model is a one of sub-models (of both types)
  bool isSubModel(const QAbstractItemModel* theModel) const;

  //! Returns true if the given model is a one of sub-models of Part type
  bool isPartSubModel(const QAbstractItemModel* theModel) const;

  //! Returns Parts Folder node
  //! \param theColumn an Id of column
  QModelIndex partFolderNode(int theColumn) const;

  int lastHistoryRow() const;

  int historyOffset() const;

  //! Data model of top part of data tree (not parts object)
  PartSet_TopDataModel* myModel;

  //! Data models for Parts data tree representation (one data model per a one part)
  QList<PartSet_PartModel*> myPartModels;

  //! Active part in part editing mode
  PartSet_PartModel* myActivePart;

  QModelIndex myActivePartIndex;

  //! List of saved QModelIndexes created by sub-models
  QList<QModelIndex*> myIndexes;

  int myHistoryBackOffset;

  static QMap<QString, QString> myIcons;
};

#endif
