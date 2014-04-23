/*
 * ModuleBase_PropPanelOperation.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_PROPPANELOPERATION_H
#define MODULEBASE_PROPPANELOPERATION_H

#include <ModuleBase.h>
#include <ModuleBase_Operation.h>

#include <QObject>
#include <QString>

#include <memory>

/*!
 * \class ModuleBase_PropPanelOperation
 *
 */
class MODULEBASE_EXPORT ModuleBase_PropPanelOperation: public ModuleBase_Operation
{

  Q_OBJECT

public:
  ModuleBase_PropPanelOperation(const QString& theId = "", QObject* parent = 0);
  virtual ~ModuleBase_PropPanelOperation();

  /*!
   * \brief Replied whether the operation should be commited after the start, or the operation itself
   *  do that. The default realization provides the check by the operation having the xml prepresentation 
   *  @return the boolean value
   */
  virtual bool isPerformedImmediately() const
  {
    return xmlRepresentation().isEmpty();
  }
  /*!
   *  \brief Returns XML representation of the operation's widget.
   *  \return XML QString
   *
   *  Returns XML representation of the operation's widget.
   */
  const QString& xmlRepresentation() const
  {
    return myXmlRepr;
  }

  /*!
   *  \brief Sets XML representation of the operation's widget.
   *  \param xmlRepr - XML QString
   *
   *  Sets XML representation of the operation's widget.
   */
  void setXmlRepresentation(const QString& xmlRepr)
  {
    this->myXmlRepr = xmlRepr;
  }


  /*
   *  Returns a short description of operation (will be
   *  inserted in title of property panel)
   */
  const QString& description() const
  {
    return myDescription;
  }

  /*
   *  Sets a short description of operation (will be
   *  inserted in title of property panel)
   */
  void setDescription(const QString& theDescription)
  {
    this->myDescription = theDescription;
  }

private:
  //!< Next fields could be extracted into a subclass;
  QString myXmlRepr;
  QString myDescription;
};

#endif //MODULEBASE_PROPPANELOPERATION_H
