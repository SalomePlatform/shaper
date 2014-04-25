#ifndef XGUI_Module_H
#define XGUI_Module_H

#include <XGUI_Workshop.h>
#include <QString>

class XGUI_Command;

class XGUI_Module
{
public:
  virtual void createFeatures() = 0;
  virtual void featureCreated(XGUI_Command*) = 0;
  virtual void launchOperation(const QString& theCmdId) = 0;

  virtual ~XGUI_Module() {};
};

//! This function must return a new module instance.
extern "C"
{
typedef XGUI_Module* (*CREATE_FUNC)(XGUI_Workshop*);
}

#define CREATE_MODULE "createModule"

#endif //XGUI_Module
