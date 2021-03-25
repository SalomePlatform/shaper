// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef XGUI_TransparencyWidget_H
#define XGUI_TransparencyWidget_H

#include "XGUI.h"

#include <QWidget>

class QSlider;
class QLabel;

/**
* \ingroup GUI
* A class of a widget to chose transparency. range of values is [0, 1],
* where 0 - there is no transparency, 1 - the object is fully transparent.
*/
class XGUI_TransparencyWidget : public QWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget for the dialog
  /// \param theLabelText if not empty, the information label will be shown in the widget
  XGUI_EXPORT XGUI_TransparencyWidget(QWidget* theParent,
                                      const QString& theLabelText = QString());
  XGUI_EXPORT virtual ~XGUI_TransparencyWidget() {};

  /// Initializes the dialog with the given value.
  /// \param theValue transparency value
  void setValue(double theValue);

  /// Returns the current transparency value.
  /// \return value
  double getValue() const;

signals:
  void transparencyValueChanged();

private slots:
  /// Update spin value by slider value
  /// \param theValue the new slider value
  void onSliderValueChanged(int theValue);

private:
  QSlider* mySliderValue; /// slider to select value
  QLabel* myValLbl;
};

#endif
