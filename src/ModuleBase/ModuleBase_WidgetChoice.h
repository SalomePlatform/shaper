// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_WidgetChoice_H
#define ModuleBase_WidgetChoice_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QWidget;
class ModuleBase_ChoiceCtrl;

/**
* \ingroup GUI
* Implementation of model widget for choice widget definition (combo box)
* It can be defined as following:
* \code
*   <choice id="bool_type" 
*     label="Type" 
*     tooltip="Type of boolean operation"
*     string_list="Cut Fuse Common Smash"
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
  virtual bool canAcceptFocus() const { return false; };

  virtual bool focusTo();

  /// Returns list of widget controls
  /// \return a controls list
  virtual QList<QWidget*> getControls() const;

  /// Returns text value for the property panel title
  /// \param theIndex a button index
  /// \return the title value
  QString getPropertyPanelTitle(int theIndex);

  /// The slot is called when user press Ok or OkPlus buttons in the parent property panel
  virtual void onFeatureAccepted();

signals:
  /// Segnal about selected item
  /// \param theWidget selected widget
  /// \param theIndex selected index
  void itemSelected(ModuleBase_ModelWidget* theWidget, int theIndex);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  virtual bool restoreValueCustom();

private slots:
  /// Slot called on combo box index change
  void onCurrentIndexChanged(int theIndex);

private:
  ModuleBase_ChoiceCtrl* myChoiceCtrl;

  // XML definition of titles
  QStringList myButtonTitles;
  std::string myStringListAttribute;

  bool myIsFirst;
  int myDefValue;
  bool myHasValue;
};

#endif
