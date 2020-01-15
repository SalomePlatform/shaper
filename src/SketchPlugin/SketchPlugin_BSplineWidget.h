// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_BSplineWidget_H
#define SketchPlugin_BSplineWidget_H


#include <SketchPlugin.h>

#include <ModelAPI_Feature.h>

#include <ModuleBase_LabelValue.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ParamSpinBox.h>

class QGroupBox;


/** \brief Represent a content of the property panel to show/modify parameters of B-spline curve.
 *  \ingroup GUI
 */
class SketchPlugin_BSplineWidget : public ModuleBase_ModelWidget
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  SketchPlugin_BSplineWidget(QWidget* theParent,
                             const Config_WidgetAPI* theData);

  virtual ~SketchPlugin_BSplineWidget() {}

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Set feature which is processing by active operation
  /// \param theFeature a feature object
  /// \param theToStoreValue a value about necessity to store the widget value to the feature
  /// \param isUpdateFlushed a flag if update should be flushed on store value
  virtual void setFeature(const FeaturePtr& theFeature, const bool theToStoreValue = false,
                          const bool isUpdateFlushed = true);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Create group of widgets related to coordinates of pole and its weight
  void addPoleWidget();

  /// Update attributes of B-spline feature
  void storePolesAndWeights() const;

private:
  struct BSplinePoleWidgets {
    ModuleBase_LabelValue* myX;
    ModuleBase_LabelValue* myY;
    ModuleBase_ParamSpinBox* myWeight;
  };

  QGroupBox* myPolesGroupBox; ///< widget to show poles and weights of B-spline curve
  std::list<BSplinePoleWidgets> myPoles; ///< list of B-spline poles and their weights
};

#endif