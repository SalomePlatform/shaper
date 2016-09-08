// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ModelDialogWidget.h
// Created:     01 June 2016
// Author:      Vitaly SMETANNIKOV

#ifndef MODULEBASE_MODELDIALOGWIDGET_H
#define MODULEBASE_MODELDIALOGWIDGET_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QDialogButtonBox;

/// \class ModuleBase_ModelDialogWidget
/// \ingroup GUI
/// \brief Widget for dialog.
class MODULEBASE_EXPORT ModuleBase_ModelDialogWidget : public ModuleBase_ModelWidget
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// a low-level API for reading xml definitions of widgets
  ModuleBase_ModelDialogWidget(QWidget* theParent, const Config_WidgetAPI* theData) :
                                ModuleBase_ModelWidget(theParent, theData) {}

  /// Set general buttons from dialog
  /// \param theButtons the dialog buttons
  void setDialogButtons(QDialogButtonBox* theButtons) { myOkCancelBtn = theButtons; }

protected:

  /// Contains dialog buttons to enable/disable Ok and Cancel buttons
  QDialogButtonBox* myOkCancelBtn;
};

#endif