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

#ifndef ModuleBase_LabelValue_H
#define ModuleBase_LabelValue_H

#include "ModuleBase.h"

#include <QWidget>

class QLabel;

/**
* \ingroup GUI
* Implementation of model widget for a label control
*/
class MODULEBASE_EXPORT ModuleBase_LabelValue : public QWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theText a text value
  /// \param theToolTip a tool tip value
  /// \param theIcon a icon value
  ModuleBase_LabelValue(QWidget* theParent, const QString& theText,
                        const QString& theToolTip = "",
                        const QString& theIcon = "",
                        int thePrecision = -12);

  virtual ~ModuleBase_LabelValue();

  /// Fills the label value with the given value
  /// \param theValue a value
  void setValue(const double theValue);

  /// Returns double value
  /// \return the value
  double value() const { return myValue; }

protected:
  QLabel* myLabel;  ///< A label information control
  QLabel* myLabelValue; ///< A label value control

  double myValue; ///< A cashed value to avoid a string conversion
  int myPrecision; ///< Precision value
};

#endif
