// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_LockApplyMgr.h
// Created:     25 Jun 2015
// Author:      Natalia Ermolaeva

#ifndef PartSet_LockApplyMgr_H
#define PartSet_LockApplyMgr_H

#include "PartSet.h"

#include <QObject>

class ModuleBase_IWorkshop;
class XGUI_OperationMgr;

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetShapeSelector in order to provide 
* working with sketch specific objects.
*/
class PARTSET_EXPORT PartSet_LockApplyMgr : public QObject
{
  Q_OBJECT

public:
  /// Constructor
  /// \param theExternal the external state
  /// \param theDefaultValue the default value for the external object using
  PartSet_LockApplyMgr(QObject* theParent,
                       ModuleBase_IWorkshop* theWorkshop);

  virtual ~PartSet_LockApplyMgr() {}

  void activate();
  void deactivate();
  void valuesChanged();

protected slots:
  // Set lock validating in the operation manager. Set apply is disabled
  void onLockValidating();
  // Set unlock validating in the operation manager. Call method to update the apply state.
  void onUnlockValidating();

private:
  XGUI_OperationMgr* operationMgr() const;

private:
  ModuleBase_IWorkshop* myWorkshop; // the current application workshop
};

#endif