// File:        XGUI_ModuleConnector.h
// Created:     3 June 2014
// Author:      Vitaly Smetannikov

#ifndef XGUI_ModuleConnector_H
#define XGUI_ModuleConnector_H


#include "XGUI.h"
#include "XGUI_Constants.h"

#include <ModuleBase_IWorkshop.h>

class Handle_AIS_InteractiveContext;
class XGUI_Workshop;

/**
* Implementation of IWorkshop interface which provides access to Workshop sevices at module level
*/
class XGUI_EXPORT XGUI_ModuleConnector: public ModuleBase_IWorkshop
{
  Q_OBJECT
public:
  XGUI_ModuleConnector(XGUI_Workshop* theWorkshop);

  virtual ~XGUI_ModuleConnector();

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const;

  //! Returns list of currently selected data objects
  QFeatureList selectedFeatures() const; 

private:
  XGUI_Workshop* myWorkshop;
};

#endif