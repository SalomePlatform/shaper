// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_TableModel.h
// Author:    Natalia ERMOLAEVA
//
#ifndef MODULEBASE_TABLE_MODEL_H_
#define MODULEBASE_TABLE_MODEL_H_

#include "ModuleBase.h"

#include <QAbstractTableModel>
#include <QStringList>

/**
  * \ingroup GUI
  * Enhanced version of the Qt's int spin box.
  * It allows to store modified state
*/
class MODULEBASE_EXPORT ModuleBase_TableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
struct Value {
  Value(int theRowId, int theColumnId, double theValue)
    : myRow(theRowId), myColumn(theColumnId), myValue(theValue) {}
  int myRow;
  int myColumn;
  double myValue;
  };

public:
  explicit ModuleBase_TableModel(QObject* theParent = 0);
  virtual ~ModuleBase_TableModel() {};

  void setHeaders(const QStringList& theHeaders) { myColumnHeaders = theHeaders; }

  void addRow(const QStringList& theValue);
  void addRow();

  virtual QVariant headerData( int theSection, Qt::Orientation theOrientation,
                               int theRole = Qt::DisplayRole ) const;
  //--------------------------------------------------------------------------------------
  virtual QModelIndex index( int theRow, int theColumn,
                             const QModelIndex &theParent = QModelIndex() ) const;
  virtual QVariant data( const QModelIndex& theIndex, int theRole = Qt::DisplayRole ) const;
  virtual bool setData( const QModelIndex& theIndex, const QVariant& theValue,
                        int theRole = Qt::EditRole );

  //--------------------------------------------------------------------------------------
  virtual Qt::ItemFlags flags( const QModelIndex &theIndex ) const;

  virtual int rowCount( const QModelIndex &theParent = QModelIndex() ) const;
  virtual int columnCount( const QModelIndex &theParent = QModelIndex() ) const;

private:
  QStringList  myColumnHeaders;
  QList<Value> myValues;
};

#endif
