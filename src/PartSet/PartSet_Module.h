#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <XGUI_Module.h>
#include <XGUI_Command.h>

#include <QMap>
#include <QObject>

#include <string>

class PARTSET_EXPORT PartSet_Module: public QObject, public XGUI_Module
{
Q_OBJECT

public:
  PartSet_Module(XGUI_Workshop* theWshop);
  virtual ~PartSet_Module();

  virtual void createFeatures();
  virtual void featureCreated(XGUI_Command* theFeature);
  std::string featureFile(const std::string&);

public slots:
  void onFeatureTriggered();
  /// SLOT, that is called after the operation is started. Perform some specific for module
  /// actions, e.g. connect the sketch feature to the viewer selection and show the sketch preview.
  void onOperationStarted();
  /// SLOT, that is called after the operation is stopped. Disconnect the sketch feature
  /// from the viewer selection and show the sketch preview.
  void onOperationStopped(ModuleBase_Operation* theOperation);

private:
  /// Displays or erase the current operation preview, if it has it.
  /// \param isDisplay the state whether the presentation should be displayed or erased
  void visualizePreview(bool isDisplay);

private:
  XGUI_Workshop* myWorkshop;

  std::map<std::string, std::string> myFeaturesInFiles;
};

#endif
