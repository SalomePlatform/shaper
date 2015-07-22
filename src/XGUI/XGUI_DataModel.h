// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_DataModel.h
// Created:     21 Jul 2015
// Author:      Vitaly SMETANNIKOV


#ifndef XGUI_DataModel_H
#define XGUI_DataModel_H

#include "XGUI.h"
#include <ModelAPI_Object.h>
#include <Config_DataModelReader.h>
#include <QAbstractItemModel>
#include <Events_Listener.h>


/**\class XGUI_DataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It uses XML file for definition of data tree.
 */
class XGUI_EXPORT XGUI_DataModel : public QAbstractItemModel, public Events_Listener
{
Q_OBJECT
public:
  XGUI_DataModel(QObject* theParent);

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  //! Returns an object by the given Model index.
  //! Returns 0 if the given index is not index of an object
  virtual ObjectPtr object(const QModelIndex& theIndex) const;

  //! Returns index of the object
  //! \param theObject object to find
  virtual QModelIndex objectIndex(const ObjectPtr theObject) const;

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
  virtual bool insertRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  /// Removes count rows starting with the given row under parent parent from the model.
  /// \param theRow a start row
  /// \param theCount a nember of rows to remove
  /// \param theParent a parent model index
  virtual bool removeRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());


private:
  Config_DataModelReader myXMLReader;
};

#endif