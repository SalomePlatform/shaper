// File:        ModuleBase_WidgetPoint2D.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetPoint2D_H
#define ModuleBase_WidgetPoint2D_H

#include <ModuleBase.h>
#include "ModuleBase_ModelWidget.h"

#include <QObject>

class ModelAPI_Feature;

class QGroupBox;
class QDoubleSpinBox;

/**\class ModuleBase_WidgetPoint2D
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetPoint2D : public ModuleBase_ModelWidget
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
  virtual bool storeValue(FeaturePtr theFeature);

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns whether the widget can accept focus, or if it corresponds to the given attribute
  /// \param theAttribute name
  virtual bool canFocusTo(const std::string& theAttributeName);

  /// Set focus to the current widget if it corresponds to the given attribute
  virtual void focusTo();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

private:
  std::string myFeatureAttributeID; ///< the identifier of the feature attribute
  QGroupBox* myGroupBox; ///< the parent group box for all intenal widgets
  QDoubleSpinBox* myXSpin; ///< the spin box for the X coordinate
  QDoubleSpinBox* myYSpin; ///< the spin box for the Y coordinate
};

#endif
