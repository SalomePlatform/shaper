// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_ErrorMgr.h
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#ifndef XGUI_ErrorMgr_H
#define XGUI_ErrorMgr_H

#include "XGUI.h"

#include <ModuleBase_IErrorMgr.h>
#include <ModelAPI_Feature.h>

class QAction;
class QDialog;
class QLabel;

class XGUI_EXPORT XGUI_ErrorMgr : public ModuleBase_IErrorMgr
{
  Q_OBJECT
public:
  XGUI_ErrorMgr(QObject* theParent = 0);
  /// Virtual destructor
  virtual ~XGUI_ErrorMgr();

  /// It updates the action state according to the given parameter
  /// \param theAction an action to be changed
  /// \param theFeature an feature that corresponds to the action
  /// \param theEnabled an enable state
  void updateActionState(QAction* theAction, const FeaturePtr& theFeature,
                         const bool theEnabled);

  /// Return true if the feature has no error. If there is an error and the action
  /// is not valid, the dialog with the error information is shown.
  /// \param theAction an action, which is checked on validity
  /// \param theFeature a feature that provides error information
  bool canProcessClick(QAction* theAction, const FeaturePtr& theFeature);

public slots:
  /// Reimplemented from ModuleBase_ErrorMgr::onValidationStateChanged().
  //virtual void onValidationStateChanged();

protected slots:
  /// Reimplemented from ModuleBase_ErrorMgr::onWidgetChanged().
  virtual void onWidgetChanged();

private:
  /// Returns the feature error message
  /// \param theFeature a feature
  /// \return the error message
  QString getFeatureError(const FeaturePtr& theFeature) const;

private:
  QDialog* myErrorDialog; /// contains the error message
  QLabel* myErrorLabel; /// contains an error information
};

#endif // XGUI_ErrorMgr_H