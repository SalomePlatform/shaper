// File:        ModuleBase_WidgetFeature.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetFeature_H
#define ModuleBase_WidgetFeature_H

#include <ModuleBase.h>
#include "ModuleBase_ModelWidget.h"

#include <QObject>
#include <QStringList>

class ModuleBase_WidgetValue;
class ModelAPI_Feature;
class QWidget;
class QLabel;
class QLineEdit;

/**\class ModuleBase_WidgetFeature
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetFeature : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetFeature(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~ModuleBase_WidgetFeature();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setValue(ModuleBase_WidgetValue* theValue);

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const;

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Fill the widget values by given point
  /// \param thePoint the point
  /// \return the boolean result of the feature set
  bool setFeature(const FeaturePtr& theFeature);

  /// Returns current widget feature
  /// \return the feature
  const FeaturePtr& feature() const { return myFeature; }

  /// Returns the widget editor
  /// \return the editor
  QLineEdit* editor() const { return myEditor; }

  /// Returns the possible feature kinds
  /// \return the list of kinds
  const QStringList& featureKinds() const { return myFeatureKinds; }

private:
  FeaturePtr myFeature; ///< the current widget feature
  QStringList myFeatureKinds; ///< the kinds of possible features

  QWidget*   myContainer; /// the parent top control
  QLabel*    myLabel; /// the editor information label
  QLineEdit* myEditor; ///< the feature editor to visualize the feature name
};

#endif
