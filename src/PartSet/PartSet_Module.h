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
  void onVisualizePreview();

private:
  XGUI_Workshop* myWorkshop;

  std::map<std::string, std::string> myFeaturesInFiles;
};

#endif
