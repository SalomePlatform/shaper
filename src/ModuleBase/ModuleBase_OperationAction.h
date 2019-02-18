// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
