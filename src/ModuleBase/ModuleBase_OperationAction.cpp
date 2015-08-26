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
