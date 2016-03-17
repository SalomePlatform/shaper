// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetChoice.h
// Created:     03 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetChoice_H
#define ModuleBase_WidgetChoice_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QWidget;
class QLabel;
class QComboBox;
class QButtonGroup;

/**
* \ingroup GUI
* Implementation of model widget for choice widget definition (combo box)
* It can be defined as following:
* \code
*   <choice id="bool_type" 
*     label="Type" 
*     tooltip="Type of boolean operation"
*     string_list="Cut Fuse Common"
*   />
* \endcode
* Aditionally can be used: 
* A key "widget_type". It can have values "combobox" or "radiobuttons".
* By default it uses "combobox".
* A key "buttons_dir" which is applicable only for "radiobuttons" mode.
* It defines direction of radiobuttons layout. it can be "vertical" or "horizontal"
* Default value is "vertical"
*/
class MODULEBASE_EXPORT ModuleBase_WidgetChoice : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetChoice(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetChoice();

  /// Defines if it is supported to set the value in this widget
  /// It returns false because this is an info widget
  virtual bool canSetValue() const { return false; };

  virtual bool focusTo();

  /// Returns list of widget controls
  /// \return a controls list
  virtual QList<QWidget*> getControls() const;

  /// Returns text value for the property panel title
  /// \param theIndex a button index
  /// \return the title value
  QString getPropertyPanelTitle(int theIndex);

signals:
  void itemSelected(ModuleBase_ModelWidget* theWidget, int theIndex);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

private slots:
  /// Slot called on combo box index change
  void onCurrentIndexChanged(int theIndex);

private:
  /// The label
  QLabel* myLabel;

  /// The control
  QComboBox* myCombo;
  QButtonGroup* myButtons;

  // XML definition of titles
  QStringList myButtonTitles;
};

#endif
