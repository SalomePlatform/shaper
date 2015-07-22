// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_ErrorMgr.h
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#ifndef XGUI_ErrorMgr_H
#define XGUI_ErrorMgr_H

#include "XGUI.h"

#include <ModuleBase_IErrorMgr.h>

class XGUI_EXPORT XGUI_ErrorMgr : public ModuleBase_IErrorMgr
{
  Q_OBJECT
public:
  XGUI_ErrorMgr(QObject* theParent = 0);
  /// Virtual destructor
  virtual ~XGUI_ErrorMgr();

public slots:
  /// Reimplemented from ModuleBase_ErrorMgr::onValidationStateChanged().
  virtual void onValidationStateChanged();

protected slots:
  /// Reimplemented from ModuleBase_ErrorMgr::onWidgetChanged().
  virtual void onWidgetChanged();
};

#endif // XGUI_ErrorMgr_H