// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_ErrorMgr.h
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#ifndef XGUI_ErrorMgr_H
#define XGUI_ErrorMgr_H

#include "XGUI.h"

#include <ModuleBase_IErrorMgr.h>
#include <ModelAPI_Feature.h>

class XGUI_Workshop;
class ModuleBase_IWorkshop;
class QAction;
class QDialog;
class QLabel;

/**
 * \class XGUI_ErrorMgr
 * \ingroup GUI
 * \brief Class of Errors manager object
 */
class XGUI_EXPORT XGUI_ErrorMgr : public ModuleBase_IErrorMgr
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent object
  /// \param theWorkshop a workshop object
  XGUI_ErrorMgr(QObject* theParent, ModuleBase_IWorkshop* theWorkshop);
  /// Virtual destructor
  virtual ~XGUI_ErrorMgr();

  /// Update actions for the given feature
  /// \param theFeature a feature
  void updateActions(const FeaturePtr& theFeature);

  /// Update enable state of AcceptAll action if the feature uses it
  /// \param theFeature a feature
  void updateAcceptAllAction(const FeaturePtr& theFeature);

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
  /// It updates the action state according to the given parameter
  /// \param theAction an action to be changed
  /// \param theFeature an feature that corresponds to the action
  void updateActionState(QAction* theAction, const FeaturePtr& theFeature);

  /// Returns the feature error message
  /// \param theFeature a feature
  /// \return the error message
  //QString getFeatureError(const FeaturePtr& theFeature) const;

  /// Returns casted workshop
  XGUI_Workshop* workshop() const;

private:
  ModuleBase_IWorkshop* myWorkshop;
  QDialog* myErrorDialog; /// contains the error message
  QLabel* myErrorLabel; /// contains an error information
};

#endif // XGUI_ErrorMgr_H