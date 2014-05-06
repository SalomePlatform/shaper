/*
 * ModuleBase_OperationDescription.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_OPERATIONDESCRIPTION_H
#define MODULEBASE_OPERATIONDESCRIPTION_H

#include <ModuleBase.h>
#include <ModuleBase_Operation.h>

#include <QObject>
#include <QString>

#include <memory>

/*!
 * \class ModuleBase_OperationDescription
 *
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

  /// Returns XML representation of the operation's widget.
  /// \return XML QString
  const QString& xmlRepresentation() const;

  /// Sets XML representation of the operation's widget.
  /// \param xmlRepr - XML QString
  void setXmlRepresentation(const QString& xmlRepr);

  /// Returns a short description of operation (will be
  /// inserted in title of property panel)
  const QString& description() const;

  /// Sets a short description of operation (will be
  /// inserted in title of property panel)
  void setDescription(const QString& theDescription);

private:
  //!< Next fields could be extracted into a subclass;
  QString myOperationId; /// the operation identifier
  QString myXmlRepr; /// the XML representation of the operation's widget
  QString myDescription; /// the short description of the opertaion
};

#endif //ModuleBase_OperationDescription_H
