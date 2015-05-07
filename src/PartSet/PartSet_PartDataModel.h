// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef PartSet_PartDataModel_H
#define PartSet_PartDataModel_H

#include "PartSet.h"
#include "PartSet_DataTreeModel.h"

/**\class PartSet_TopDataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It represents only upper part of data tree (non-parts tree items)
 */
class PARTSET_EXPORT PartSet_TopDataModel : public PartSet_FeaturesModel
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  PartSet_TopDataModel(QObject* theParent);
  virtual ~PartSet_TopDataModel();

  // Reimpl from QAbstractItemModel

  /// Returns the data stored under the given role for the item referred to by the index.
  /// \param theIndex a model index
  /// \param theRole a data role (see Qt::ItemDataRole)
  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;

  /// Returns the data for the given role and section in the header with the specified orientation.
  /// \param theSection a section
  /// \param theOrient an orientation
  /// \param theRole a data role (see Qt::ItemDataRole)
  virtual QVariant headerData(int theSection, Qt::Orientation theOrient,
                              int theRole = Qt::DisplayRole) const;

  /// Returns the number of rows under the given parent. When the parent is valid it means that 
  /// rowCount is returning the number of children of parent.
  /// \param theParent a parent model index
  virtual int rowCount(const QModelIndex &theParent = QModelIndex()) const;

  /// Returns the number of columns for the children of the given parent.
  /// It has a one column
  /// \param theParent a parent model index
  virtual int columnCount(const QModelIndex &theParent = QModelIndex()) const;


  /// Returns the index of the item in the model specified by the given row, column and parent index.
  /// \param theRow a row
  /// \param theColumn a column
  /// \param theParent a parent model index
  virtual QModelIndex index(int theRow, int theColumn, const QModelIndex& theParent =
                                QModelIndex()) const;

  /// Returns the parent of the model item with the given index. 
  /// If the item has no parent, an invalid QModelIndex is returned.
  /// \param theIndex a model index
  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  /// Returns true if parent has any children; otherwise returns false.
  /// \param theParent a parent model index
  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

  //! Returns object by the given Model index.
  //! Returns 0 if the given index is not index of a object
  virtual ObjectPtr object(const QModelIndex& theIndex) const;

  //! Returns QModelIndex which corresponds to the given object
  //! If the object is not found then index is not valid
  virtual QModelIndex objectIndex(const ObjectPtr& theObject) const;

  //! Returns parent index of the given object
  virtual QModelIndex findParent(const ObjectPtr& theObject) const;

  //! Returns index corresponded to the group
  virtual QModelIndex findGroup(const std::string& theGroup) const;

 private:
  //! Types of QModelIndexes
  enum DataIds
  {
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject
    //GroupsFolder,
    //GroupObject
  };

};

/**\class PartSet_PartDataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It represents data tree only of a one part
 */
class PartSet_PartDataModel : public PartSet_PartModel
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  PartSet_PartDataModel(QObject* theParent);
  virtual ~PartSet_PartDataModel();

  // Reimpl from QAbstractItemModel

  /// Returns the data stored under the given role for the item referred to by the index.
  /// \param theIndex a model index
  /// \param theRole a data role (see Qt::ItemDataRole)
  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;

  /// Returns the data for the given role and section in the header with the specified orientation.
  /// \param theSection a section
  /// \param theOrient an orientation
  /// \param theRole a data role (see Qt::ItemDataRole)
  virtual QVariant headerData(int theSection, Qt::Orientation theOrient,
                              int theRole = Qt::DisplayRole) const;

  /// Returns the number of rows under the given parent. When the parent is valid it means that 
  /// rowCount is returning the number of children of parent.
  /// \param theParent a parent model index
  virtual int rowCount(const QModelIndex &theParent = QModelIndex()) const;

  /// Returns the number of columns for the children of the given parent.
  /// It has a one column
  /// \param theParent a parent model index
  virtual int columnCount(const QModelIndex &theParent = QModelIndex()) const;

  /// Returns the index of the item in the model specified by the given row, column and parent index.
  /// \param theRow a row
  /// \param theColumn a column
  /// \param theParent a parent model index
  virtual QModelIndex index(int theRow, int theColumn, const QModelIndex& theParent =
                                QModelIndex()) const;

  /// Returns the parent of the model item with the given index. 
  /// If the item has no parent, an invalid QModelIndex is returned.
  /// \param theIndex a model index
  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  /// Returns true if parent has any children; otherwise returns false.
  /// \param theParent a parent model index
  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

  //! Returns object by the given Model index.
  //! Returns 0 if the given index is not index of a object
  virtual ObjectPtr object(const QModelIndex& theIndex) const;

  //! Returns QModelIndex which corresponds to the given object
  //! If the object is not found then index is not valid
  virtual QModelIndex objectIndex(const ObjectPtr& theObject) const;

  //! Returns true if the given document is a sub-document of this tree
  virtual bool hasDocument(const DocumentPtr& theDoc) const;

  //! Returns parent index of the given object
  virtual QModelIndex findParent(const ObjectPtr& theObject) const;

  //! Returns index corresponded to the group
  virtual QModelIndex findGroup(const std::string& theGroup) const;

  //! Return a Part object
  virtual ResultPartPtr part() const;

 private:

  //! Returns document of the current part
  DocumentPtr partDocument() const;

  //! Returns defult number of rows
  int getRowsNumber() const;

  int lastHistoryRow() const;

  //! Types of QModelIndexes
  enum DataIds
  {
    //MyRoot,
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject,
    BodiesFolder,
    BodiesObject,
    GroupsFolder,
    GroupObject,
    HistoryObject
  };

  int myHistoryBackOffset;
};

#endif
