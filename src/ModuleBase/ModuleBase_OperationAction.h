// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_OperationAction.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_OperationAction_H
#define ModuleBase_OperationAction_H

#include <ModuleBase.h>

#include <ModuleBase_Operation.h>

/*!
 * \class ModuleBase_OperationAction
 * \ingroup GUI
 * \brief Base class for all operations
 *
 *  Base class for all operations. If you perform an action it is reasonable to create
 *  operation intended for this. This is a base class for all operations which provides
 *  mechanism for correct starting operations, starting operations above already started
 *  ones, committing operations and so on. To create own operation it is reasonable to
 *  inherit it from this class and redefines virtual methods to provide own behavior
 */

class MODULEBASE_EXPORT ModuleBase_OperationAction : public ModuleBase_Operation
{
Q_OBJECT

 public:

  /// Constructor
  /// \param theId the operation identifier
  /// \param theParent the QObject parent
  ModuleBase_OperationAction(const QString& theId = "", QObject* theParent = 0);
  /// Destructor
  virtual ~ModuleBase_OperationAction();
};

#endif
