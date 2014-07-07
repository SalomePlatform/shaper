#ifndef ModuleBase_IModule_H
#define ModuleBase_IModule_H

#include <QString>
#include <QObject>

class QAction;
class XGUI_Workshop;
class Config_WidgetAPI;
class ModuleBase_ModelWidget;

/**
* Interface to a module
*/
class ModuleBase_IModule: public QObject
{
public:
  /// Reads description of features from XML file 
  virtual void createFeatures() = 0;

  /// Called on creation of menu item in desktop
  virtual void featureCreated(QAction*) = 0;

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId) = 0;

  /// Called when it is necessary to update a command state (enable or disable it)
  virtual bool isFeatureEnabled(const QString& theCmdId) const = 0;

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent, 
    Config_WidgetAPI* theWidgetApi, QList<ModuleBase_ModelWidget*>& theModelWidgets)
  {
    return 0;
  }

  virtual ~ModuleBase_IModule() {};
};

//! This function must return a new module instance.
extern "C"
{
typedef ModuleBase_IModule* (*CREATE_FUNC)(XGUI_Workshop*);
}

#define CREATE_MODULE "createModule"

#endif //ModuleBase_IModule
