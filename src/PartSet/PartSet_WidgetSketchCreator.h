// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchCreator.h
// Created:     08 June 2015
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_WidgetSketchCreator_H
#define PartSet_WidgetSketchCreator_H

#include "PartSet.h"

#include <ModuleBase_WidgetSelector.h>

class QLabel;
class QLineEdit;
class PartSet_Module;
class ModuleBase_Operation;


/**
* \ingroup Modules
* A widget which allow creation of a sketch in another operation.
* It creates sketch on aqctivation of this widget. If sketch object is already created then
* it will transfer a focus to next widget. It is supposed that the widget will be placed as 
* a first widget in property panel
*/
class PARTSET_EXPORT PartSet_WidgetSketchCreator : public ModuleBase_WidgetSelector
{
Q_OBJECT

public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theModule a reference to a module object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetSketchCreator(QWidget* theParent, PartSet_Module* theModule,
                              const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetSketchCreator();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Visualization of the current control or others in PP
  /// \param theSelectionControl state whether the control should be shown/hidden
  void setVisibleSelectionControl(const bool theSelectionControl);

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList getShapeTypes() const;

private:
  /// Returns true if the selection mode is active. This is when composition feature has no
  ///  a sub-object and the attribute list is empty
  /// \return boolean value
  bool isSelectionMode() const;

private slots:
  void onStarted();

  void onResumed(ModuleBase_Operation* theOp);

private:
  std::string myAttributeListID;

  PartSet_Module* myModule;

  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

  /// List of accepting shapes types
  QStringList myShapeTypes;

  /// To check if we need to use body for composite feature or not
  bool myUseBody;

};

#endif