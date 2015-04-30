// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ModuleBase_IDocumentDataModel.h
// Created:     28 Apr 2015
// Author:      Vitaly SMETANNIKOV


#ifndef ModuleBase_IDocumentDataModel_H
#define ModuleBase_IDocumentDataModel_H

#include "ModuleBase.h"
#include <QAbstractItemModel>
#include <ModelAPI_Object.h>

class MODULEBASE_EXPORT ModuleBase_IDocumentDataModel : public QAbstractItemModel
{
Q_OBJECT
public:
  ModuleBase_IDocumentDataModel(QObject* theParent): QAbstractItemModel(theParent) {}

  //! Returns an object by the given Model index.
  //! Returns 0 if the given index is not index of an object
  virtual ObjectPtr object(const QModelIndex& theIndex) const = 0;

  //! Returns index of the object
  //! \param theObject object to find
  virtual QModelIndex objectIndex(const ObjectPtr theObject) const = 0;

  //! Clear internal data
  virtual void clear() {}

  //! Rebuild data tree
  virtual void rebuildDataTree() {}
};

#endif