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
 * \brief Base class for action operations
 *
 *  This is an action-like operation, which modifies the structure of data through
 *  starting/comitting transactions in the document. This operations are single stepped,
 *  and have no filled property panel, like Delete/Detach.
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

 public slots:
  /// Commits the operation. Change is modified state to true value.
  /// \return the result of commit
  virtual bool commit();
};

#endif
