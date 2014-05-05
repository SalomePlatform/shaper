// File:        ModuleBase_WidgetCustom.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetCustom_H
#define ModuleBase_WidgetCustom_H

#include <ModuleBase.h>
#include <ModuleBase_IModelWidget.h>

#include <QObject>

#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;

/**\class ModuleBase_WidgetCustom
 * \ingroup GUI
 * \brief An abstract custom widget class. This class realization is assumed to create some controls.
 * The controls values modification should send signal about values change. Method store is used to fill
 * the feature with the 
 */
class MODULEBASE_EXPORT ModuleBase_WidgetCustom : public QObject
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  ModuleBase_WidgetCustom(QObject* theParent) :QObject(theParent) {};
  /// Destructor
  virtual ~ModuleBase_WidgetCustom() {};

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual void store(boost::shared_ptr<ModelAPI_Feature> theFeature) = 0;

signals:
  /// The signal about widget values changed
  void valuesChanged();
};

#endif
