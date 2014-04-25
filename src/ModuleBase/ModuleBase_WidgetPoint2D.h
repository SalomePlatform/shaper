// File:        ModuleBase_WidgetPoint2D.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetPoint2D_H
#define ModuleBase_WidgetPoint2D_H

#include <ModuleBase.h>
#include "ModuleBase_WidgetCustom.h"

#include <QObject>

class ModelAPI_Feature;

class QGroupBox;
class QDoubleSpinBox;

/**\class ModuleBase_WidgetPoint2D
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetPoint2D : public ModuleBase_WidgetCustom
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theTitle the group box title
  /// \theFeatureAttributeID the identifier of the feature attribute
  ModuleBase_WidgetPoint2D(QWidget* theParent, QString theTitle,
                           const std::string& theFeatureAttributeID);
  /// Destructor
  virtual ~ModuleBase_WidgetPoint2D();

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual void store(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

private:
  std::string myFeatureAttributeID; ///< the identifier of the feature attribute
  QGroupBox* myGroupBox; ///< the parent group box for all intenal widgets
  QDoubleSpinBox* myXSpin; ///< the spin box for the X coordinate
  QDoubleSpinBox* myYSpin; ///< the spin box for the Y coordinate
};

#endif
