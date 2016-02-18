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
class ModuleBase_ModelWidget;

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

  /// Returns true if the apply is enabled for the current feature
  bool isApplyEnabled() const;

protected slots:
  /// Reimplemented from ModuleBase_ErrorMgr::onWidgetChanged().
  virtual void onWidgetChanged();

private:
  /// It disables the action if the error message is not empty
  /// The message is set to the header tool tip.
  /// \param theAction an action to be changed
  /// \param theError an error state
  void updateActionState(QAction* theAction, const QString& theError);

  /// It updates the tool tip of the widget controls according to the widget error
  /// \param theWidget a widget
  /// \param theError an error state
  void updateToolTip(ModuleBase_ModelWidget* theWidget, const QString& theError);

  /// Returns casted workshop
  XGUI_Workshop* workshop() const;

  /// Returns an active widget of the current operation
  /// \return the widget or zero
  ModuleBase_ModelWidget* activeWidget() const;

private:
  ModuleBase_IWorkshop* myWorkshop; /// workshop
  QDialog* myErrorDialog; /// contains the error message
  QLabel* myErrorLabel; /// contains an error information
  QString myAcceptToolTip; /// cached tool tip value for enabled Accept action
  QString myAcceptAllToolTip; /// cached tool tip value for enabled AcceptAll action
};

#endif // XGUI_ErrorMgr_H