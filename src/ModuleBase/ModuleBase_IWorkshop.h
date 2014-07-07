// File:        ModuleBase_IWorkshop.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_IWorkshop_H
#define ModuleBase_IWorkshop_H

#include "ModuleBase.h"

#include <ModelAPI_Feature.h>

#include <AIS_InteractiveContext.hxx>

#include <QObject>

class ModuleBase_IModule;

/**
* Class which provides access to Workshop object serveces
*/
class MODULEBASE_EXPORT ModuleBase_IWorkshop: public QObject
{
Q_OBJECT
public:
  ModuleBase_IWorkshop(QObject* theParent):QObject(theParent) {}

  virtual ~ModuleBase_IWorkshop() {};

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const = 0;

  //! Returns list of currently selected data objects
  virtual QList<FeaturePtr> selectedFeatures() const = 0; 

  //! Returns instance of loaded module
  virtual ModuleBase_IModule* module() const = 0;

signals:
  void selectionChanged();
};

#endif