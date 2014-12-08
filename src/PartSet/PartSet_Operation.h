// File:        PartSet_Operation.h
// Created:     24 Nov 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Operation_H
#define PartSet_Operation_H

#include "PartSet.h"

#include <ModuleBase_Operation.h>

class ModuleBase_IWorkshop;

class PARTSET_EXPORT PartSet_Operation : public ModuleBase_Operation
{
Q_OBJECT
 public:
   PartSet_Operation(const QString& theId = "", QObject* theParent = 0)
     :ModuleBase_Operation(theId, theParent) {}

   void setWorkshop(ModuleBase_IWorkshop* theWorkshop) { myWorkshop = theWorkshop; } 

public slots:
  void onSelectionChanged();

private:
  ModuleBase_IWorkshop* myWorkshop;
};

#endif