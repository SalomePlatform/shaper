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

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to do nothing if the plane of the sketch has been already set.
  /// \param theValues the wrapped selection values
  /// \param thePosition an index in the list of values, the values should be get from the index
  virtual bool setSelection(const QList<ModuleBase_ViewerPrs>& theValues, int& thePosition);

  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid);

  /// Fills the attribute with the value of the selected owner
  /// \param theOwner a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// The methiod called when widget is activated
  virtual void activateCustom();

private slots:
  void onStarted();

private:

  PartSet_Module* myModule;

  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

};

#endif