// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModuleBase_ChoiceCtrl_H
#define ModuleBase_ChoiceCtrl_H

#include "ModuleBase.h"

#include <QWidget>
#include <QStringList>
#include <QList>

class QLabel;
class QComboBox;
class QGroupBox;
class QButtonGroup;

/**
* \ingroup GUI
* A Choice control. It provides a choice in several strings.
* It can be represented by several radiobuttons or by combo box.
* Radio buttons can be represented as by radiou buttons with text
* or by icons in toggle buttons.
*/
class MODULEBASE_EXPORT ModuleBase_ChoiceCtrl: public QWidget
{
Q_OBJECT
public:
  enum ControlType {
    RadioButtons,
    ComboBox
  };

  /**
  * Constructor
  * \param theParent a parent widget
  * \param theChoiceList a list of choice strings
  * \param theIconsList a list of icon names for radiou buttons
  * \param theType a type of choice representation
  * \param theButtonsDir direction of radio buttons placement
  */
  ModuleBase_ChoiceCtrl(QWidget* theParent,
                        const QStringList& theChoiceList,
                        const QStringList& theIconsList,
                        ControlType theType = RadioButtons,
                        Qt::Orientation theButtonsDir = Qt::Horizontal);

  /// Set label for the controls.
  /// It is a label for combo box and title for group of radio buttons.
  /// \param theText a text of the label
  void setLabel(const QString& theText);

  /// Set Icon for the label. Used only for combo box.
  /// \param theIcon a name of icon
  void setLabelIcon(const QString& theIcon);

  /// Set value: Id of button or item of combo box.
  /// \param theVal a value (from 0 to number of items)
  void setValue(int theVal);

  /// Set value: text of button or item of combo box.
  /// \param theVal a value (one of text items)
  void setValue(const QString& theVal);

  /// Set tool tip for label. Used only for combo box.
  void setTooltip(QString theTip);

  /// Returns currently selected value
  int value() const;

  /// Returns text of currently selected value
  QString textValue() const;

  /// Transfer focus on itself
  bool focusTo();

  /// Returns controls for activation
  QList<QWidget*> getControls() const;

  /// Set list of choice
  /// \param theChoiceList a string list of items
  void setChoiceList(const QStringList& theChoiceList);

signals:
  /// A signal raised on change of current value
  void valueChanged(int theVal);

private:
  /// Control type
  ControlType myType;

  /// A label for cmbo box
  QLabel* myLabel;

  /// A combo box represerntation of control
  QComboBox* myCombo;

  /// A group box for radio buttons
  QGroupBox* myGroupBox;

  /// A group of buttons
  QButtonGroup* myButtons;
};

#endif