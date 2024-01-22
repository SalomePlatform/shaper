// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef SAMPLEPANELPLUGIN_PANEL_H_
#define SAMPLEPANELPLUGIN_PANEL_H_

#include <QWidget>

#include <ModelAPI_Feature.h>

class QComboBox;

/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of some feature.
 */
class SamplePanelPlugin_Panel : public QWidget
{
 Q_OBJECT
public:
  /// Constructs a panel page
  SamplePanelPlugin_Panel(QWidget* theParent);
  /// Destructs the page
  virtual ~SamplePanelPlugin_Panel() {}

  /// Fill the panel by the feature
  /// \param theFeature a feature to fill the panel controls
  void setFeature(const FeaturePtr& theFeature);

protected slots:
  /// Update feature attribute by the current value of combo box
  /// \param theIndex a combo box modified value
  void onCurrentIndexChanged(int theIndex);

private:
  FeaturePtr myFeature; // the feature which corresponds to the current panel
  QComboBox* myComboBox; // control for value attribute of the current feature
  int myDefaultValue; /// the default combo box value
};

#endif /* SAMPLEPANELPLUGIN_PANEL_H_ */
