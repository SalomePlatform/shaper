// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.h
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_WidgetSketchCreator_H
#define PartSet_WidgetSketchCreator_H

#include "PartSet.h"

#include <ModuleBase_ModelWidget.h>

class QLabel;
class QLineEdit;
class PartSet_Module;
class ModuleBase_Operation;

class PARTSET_EXPORT PartSet_WidgetSketchCreator : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetSketchCreator(QWidget* theParent, PartSet_Module* theModule, 
    const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~PartSet_WidgetSketchCreator();

  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// The methiod called when widget is activated
  virtual void activateCustom();

private slots:
  void onStarted();

  void onResumed(ModuleBase_Operation* theOp);

private:

  PartSet_Module* myModule;

  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

};

#endif