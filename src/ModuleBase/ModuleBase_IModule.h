#ifndef ModuleBase_IModule_H
#define ModuleBase_IModule_H

#include "ModuleBase.h"
#include "ModuleBase_IWorkshop.h"

#include <QString>
#include <QObject>


class QAction;
class Config_WidgetAPI;
class ModuleBase_ModelWidget;
class ModuleBase_Operation;
class ModuleBase_IWorkshop;

/**
 * Interface to a module
 */
class MODULEBASE_EXPORT ModuleBase_IModule : public QObject
{
 public:

   ModuleBase_IModule(ModuleBase_IWorkshop* theParent): QObject(theParent), myWorkshop(theParent) {}

  virtual ~ModuleBase_IModule() {}

  /// Reads description of features from XML file 
  virtual void createFeatures() = 0;

  /// Called on creation of menu item in desktop
  virtual void featureCreated(QAction*) = 0;

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId);

  /// Called when it is necessary to update a command state (enable or disable it)
  //virtual bool isFeatureEnabled(const QString& theCmdId) const = 0;

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi,
                                      QList<ModuleBase_ModelWidget*>& theModelWidgets)
  {
    return 0;
  }

  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

 protected:
  /// Sends the operation for launching
  /// \param theOperation the operation
  void sendOperation(ModuleBase_Operation* theOperation);

  /// Creates a new operation
  /// \param theCmdId the operation name
  /// \param theFeatureKind a kind of feature to get the feature xml description
  virtual ModuleBase_Operation* createOperation(const std::string& theCmdId,
                                        const std::string& theFeatureKind = "") = 0;


protected:

  ModuleBase_IWorkshop* myWorkshop;

};

//! This function must return a new module instance.
extern "C" {
typedef ModuleBase_IModule* (*CREATE_FUNC)(ModuleBase_IWorkshop*);
}

#define CREATE_MODULE "createModule"

#endif //ModuleBase_IModule
