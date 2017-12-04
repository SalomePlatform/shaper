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

class MODULEBASE_EXPORT ModuleBase_ChoiceCtrl: public QWidget
{
Q_OBJECT
public:
  enum ControlType {
    RadioButtons,
    ComboBox
  };

  ModuleBase_ChoiceCtrl(QWidget* theParent,
                        const QStringList& theChoiceList,
                        const QStringList& theIconsList,
                        ControlType theType = RadioButtons,
                        Qt::Orientation theButtonsDir = Qt::Horizontal);

  void setLabel(const QString& theText);

  void setLabelIcon(const QString& theIcon);

  void setValue(int theVal);

  void setTooltip(QString theTip);

  int value() const;

  bool focusTo();

  QList<QWidget*> getControls() const;

  void setChoiceList(const QStringList& theChoiceList);

signals:
  void valueChanged(int theVal);

private:
  ControlType myType;
  QLabel* myLabel;
  QComboBox* myCombo;
  QGroupBox* myGroupBox;
  QButtonGroup* myButtons;
};

#endif