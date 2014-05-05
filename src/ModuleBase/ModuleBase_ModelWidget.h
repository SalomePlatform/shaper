// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ModelWidget_H
#define ModuleBase_ModelWidget_H

#include <ModuleBase.h>

#include <QObject>

#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;

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
  virtual bool storeValue(boost::shared_ptr<ModelAPI_Feature> theFeature) = 0;

  virtual bool restoreValue(boost::shared_ptr<ModelAPI_Feature> theFeature) = 0;


signals:
  /// The signal about widget values changed
  void valuesChanged();
};

#endif
