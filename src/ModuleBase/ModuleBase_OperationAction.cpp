// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_OperationAction.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include "ModuleBase_OperationAction.h"

ModuleBase_OperationAction::ModuleBase_OperationAction(const QString& theId, QObject* theParent)
 : ModuleBase_Operation(theId, theParent)
{
}

ModuleBase_OperationAction::~ModuleBase_OperationAction()
{
}

bool ModuleBase_OperationAction::commit()
{
  // the action is supposed to perform a single modification, so the operation returns modified state
  setIsModified(true);

  return ModuleBase_Operation::commit();
}
