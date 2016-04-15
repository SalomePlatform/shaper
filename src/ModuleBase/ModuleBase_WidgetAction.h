// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetAction.h
// Created:     15 Apr 2016
// Author:      Natalia Ermolaeva

#ifndef ModuleBase_WidgetAction_H
#define ModuleBase_WidgetAction_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class Config_WidgetAPI;
class QWidget;
class QToolButton;

/**
* \ingroup GUI
* Implementation of widget for feature action (tool button)
*/ 
class MODULEBASE_EXPORT ModuleBase_WidgetAction : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetAction(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetAction();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Do nothing
  /// \return True in success
  virtual bool storeValueCustom();

  /// Do nothing
  virtual bool restoreValueCustom();

protected slots:
  /// Listens the button click and call the customAction function of the current feature
  void onActionClicked();

private:
  QToolButton* myButton; ///< action button
  std::string myActionID; ///< action index
};

#endif
