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

#ifndef XGUI_DeflectionDialog_H
#define XGUI_DeflectionDialog_H

#include "XGUI.h"

#include <QDialog>

class QButtonGroup;
class QDoubleSpinBox;

/**
* \ingroup GUI
* A class of dialog to chose a deflection value. The deflection value is in range [0, 1]
*/ 
class XGUI_DeflectionDialog : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget for the dialog
  XGUI_EXPORT XGUI_DeflectionDialog(QWidget* theParent);

  XGUI_EXPORT virtual ~XGUI_DeflectionDialog() {};

  /// Initializes the dialog with the given value.
  /// \param theValue the deflection value
  void setDeflection(const double& theDeflection);

  /// Returns the current deflection value.
  /// \return a double value
  double getDeflection() const;

private:
  QButtonGroup* myButtonGroup; /// a group, contained random and certain color radio button choice
  QDoubleSpinBox* myDeflection; /// a deflection value
};

#endif
