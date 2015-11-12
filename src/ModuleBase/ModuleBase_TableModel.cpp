// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_TableModel.cxx
// Author:    Natalia ERMOLAEVA
//
#include "ModuleBase_TableModel.h"

ModuleBase_TableModel::ModuleBase_TableModel(QObject* theParent)
: QAbstractTableModel(theParent)
{
}

void ModuleBase_TableModel::addRow(const QStringList& theValue)
{
  if (theValue.size() != columnCount())
    return;

  int aRowId = rowCount();
  for (int i = 0, aCols = columnCount(); i < aCols; i++)
    myValues.append(Value(aRowId, i, theValue[i].toDouble()));

  emit layoutChanged();
}

void ModuleBase_TableModel::addRow()
{
  return;
  int aRowId = rowCount();
  for (int i = 0, aCols = columnCount(); i < aCols; i++)
    myValues.append(Value(aRowId, i, 0.0));

  emit layoutChanged();
}

QVariant ModuleBase_TableModel::headerData(int theSection, Qt::Orientation theOrientation,
                                           int theRole) const
{
  if (theSection < myColumnHeaders.size() && theRole == Qt::DisplayRole)
    return myColumnHeaders[theSection];

  return QAbstractTableModel::headerData(theSection, theOrientation, theRole);
}

QModelIndex ModuleBase_TableModel::index( int theRow, int theColumn,
                                         const QModelIndex &theParent) const
{
    if (!hasIndex(theRow, theColumn, theParent))
    return QModelIndex();

  return createIndex(theRow, theColumn);
}

QVariant ModuleBase_TableModel::data( const QModelIndex& theIndex, int theRole) const
{
  QVariant aValue;
  if (theRole == Qt::DisplayRole) {
    QList<Value>::const_iterator anIt = myValues.begin(), aLast = myValues.end();
    for (; anIt != aLast; anIt++) {
      Value aVal = *anIt;
      if (aVal.myRow == theIndex.row() && aVal.myColumn == theIndex.column()) {
        aValue = aVal.myValue;
        break;
      }
    }
  }
  else {
    aValue = QVariant();
  }

  return aValue;
}

bool ModuleBase_TableModel::setData( const QModelIndex& theIndex, const QVariant& theValue,
                                     int theRole)
{
  bool isDone = false;
  if (theRole == Qt::DisplayRole) {
    QList<Value>::const_iterator anIt = myValues.begin(), aLast = myValues.end();
    for (; anIt != aLast; anIt++) {
      Value aVal = *anIt;
      if (aVal.myRow == theIndex.row() && aVal.myColumn == theIndex.column()) {
        aVal.myValue = theValue.toDouble();
        break;
      }
    }
    isDone = true;
  }
  else
    isDone = true;
  return isDone;
}

int ModuleBase_TableModel::rowCount( const QModelIndex &theParent) const
{
  return myValues.size()/myColumnHeaders.size();
}

int ModuleBase_TableModel::columnCount( const QModelIndex &theParent) const
{
  return myColumnHeaders.size();
}

Qt::ItemFlags ModuleBase_TableModel::flags(const QModelIndex& theIndex) const
{
  if (!theIndex.isValid())
    return 0;
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

  return aFlags;
}
