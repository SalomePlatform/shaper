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

#ifndef ModuleBase_WidgetDoubleValue_H
#define ModuleBase_WidgetDoubleValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class ModuleBase_ParamSpinBox;
class Config_WidgetAPI;
class QWidget;
class QLabel;
class QTimer;

/**
* \ingroup GUI
* A class of property panel widget for double value input
* It can be defined with "doublevalue" keyword. For example:
* \code
* <doublevalue id="x" label="X:" icon=":pictures/x_point.png" tooltip="X coordinate"/>
* \endcode
*/
class MODULEBASE_EXPORT ModuleBase_WidgetDoubleValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetDoubleValue(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetDoubleValue();

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Select the internal content if it can be selected. It is empty in the default realization
  virtual void selectContent();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

 public slots:
 // Delayed value chnged: when user starts typing something,
 // it gives him a 0,5 second to finish typing, when sends valueChnaged() signal
//  void onValueChanged();

protected:
  /// Returns true if the event is processed.
  virtual bool processEnter();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  //! Read value of corresponded attribute from data model to the input control
  // \return True in success
  virtual bool restoreValueCustom();

  /// Fills the widget with default values
  /// \return true if the widget current value is reset
  virtual bool resetCustom();

protected:
  /// Label of the widget
  QLabel* myLabel;

  /// Input value control
  ModuleBase_ParamSpinBox* mySpinBox;
};

#endif
