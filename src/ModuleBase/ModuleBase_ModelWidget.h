// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ModelWidget_H
#define ModuleBase_ModelWidget_H

#include <ModuleBase.h>

#include <ModelAPI_Feature.h>

#include <QObject>

#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;
class QKeyEvent;

/**\class ModuleBase_ModelWidget
 * \brief An abstract custom widget class. This class realization is assumed to create some controls.
 * The controls values modification should send signal about values change.
 *
 * Common interface for widgets in the property panel.
 * Every widget are able to save/restore data from the model and/or to contain other widgets.
 *
 */
class MODULEBASE_EXPORT ModuleBase_ModelWidget : public QObject
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  ModuleBase_ModelWidget(QObject* theParent) :QObject(theParent) {};
  /// Destructor
  virtual ~ModuleBase_ModelWidget() {};

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) = 0;

  virtual bool restoreValue(FeaturePtr theFeature) = 0;

  /// Returns whether the widget can accept focus, or if it corresponds to the given attribute
  /// \param theAttribute name
  virtual bool canFocusTo(const std::string& theAttributeName) = 0;

  /// Set focus to the current widget if it corresponds to the given attribute
  virtual void focusTo() = 0;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const = 0;

signals:
  /// The signal about widget values changed
  void valuesChanged();
  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theAttributeName a name of the attribute
  /// \param theEvent key release event
  void keyReleased(const std::string& theAttributeName, QKeyEvent* theEvent);
};

#endif
