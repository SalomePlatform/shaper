// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef MODULEBASE_OPERATIONDESCRIPTION_H
#define MODULEBASE_OPERATIONDESCRIPTION_H

#include <ModuleBase.h>
#include <ModuleBase_Operation.h>

#include <QObject>
#include <QString>
#include <QList>

#include <memory>

//class ModuleBase_ModelWidget;

/*!
 * \class ModuleBase_OperationDescription
 * \ingroup GUI
 * Description of operation from XML
 */
class MODULEBASE_EXPORT ModuleBase_OperationDescription
{
 public:
  /// Constructor
  /// \param theId - the operation identifier
  ModuleBase_OperationDescription(const QString& theId = "");
  /// Destructor
  virtual ~ModuleBase_OperationDescription();

  /// Unique name of the operation
  /// \return string name of the operation.
  const QString& operationId() const;

  /// Returns a short description of operation (will be
  /// inserted in title of property panel)
  const QString& description() const;

  /// Sets a short description of operation (will be
  /// inserted in title of property panel)
  void setDescription(const QString& theDescription);

  /// Returns XML representation of the operation's widget.
  /// \return XML QString
  const QString& xmlRepresentation() const;

  /// Sets XML representation of the operation's widget.
  /// \param theXMLRepresentation - XML QString
  void setXmlRepresentation(const QString& theXMLRepresentation);

  /// Returns true if there are no model widgets
  /// \return the boolean value
  bool hasXmlRepresentation() const;

 private:
  //!< Next fields could be extracted into a subclass;
   /// the operation identifier
  QString myOperationId;

  /// the short description of the opertaion
  QString myDescription;

  /// the xml definition
  QString myXmlRepresentation;
};

#endif //ModuleBase_OperationDescription_H
