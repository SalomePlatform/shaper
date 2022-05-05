// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef SamplePanelPlugin_ModelWidget_H_
#define SamplePanelPlugin_ModelWidget_H_

#include <ModuleBase_ModelWidget.h>

#include <ModelAPI_Feature.h>

class Config_WidgetAPI;

class QWidget;
class QComboBox;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of some feature.
 */
class SamplePanelPlugin_ModelWidget : public ModuleBase_ModelWidget
{
 Q_OBJECT
public:
  /// Constructs a model widget
  SamplePanelPlugin_ModelWidget(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Destructs the model widget
  virtual ~SamplePanelPlugin_ModelWidget() {}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

private:
  QWidget* myMainWidget; // parent control for all widget controls
  QComboBox* myComboBox; // control for value attribute of the current feature
  int myDefaultValue; /// the default combo box value
};

#endif /* SamplePanelPlugin_ModelWidget_H_ */
