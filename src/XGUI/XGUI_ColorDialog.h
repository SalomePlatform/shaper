// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef XGUI_ColorDialog_H
#define XGUI_ColorDialog_H

#include "XGUI.h"

#include <QDialog>

class QButtonGroup;
class QtxColorButton;

/**
* \ingroup GUI
* A class of dialog to chose a color. The color can be set in two ways: use a random value or
* a certain color. There is a radio button to provide this choice. The color button is 
* visualized to select a certain color.
*/ 
class XGUI_ColorDialog : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget for the dialog
  XGUI_EXPORT XGUI_ColorDialog(QWidget* theParent);

  XGUI_EXPORT virtual ~XGUI_ColorDialog() {};

  /// Returns whether the random state of color is chosen
  /// \return a boolean value
  bool isRandomColor() const;

  /// Initializes the dialog with the given value. Set choice on certain value and fill it by.
  /// \param theValue an RGB components value
  void setColor(const std::vector<int>& theValue);

  /// Returns a container with the current color value.
  /// These are tree int values for RGB definition.
  /// These value is depend on the random choice and eighter
  /// a next random color or a certain color.
  /// \return a vector of values
  std::vector<int> getColor() const;

  /// Returns a container with the current color value.
  /// These are tree int values for RGB definition.
  /// It returns the next random color.
  /// \return a vector of values
  std::vector<int> getRandomColor() const;

private:
  QButtonGroup* myButtonGroup; /// a group, contained random and certain color radio button choice
  QtxColorButton* myColorButton; /// a control to select a color
};

#endif
