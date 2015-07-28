// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_IconFactory.h
// Created:     28 Jul 2015
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_IconFactory_H
#define PartSet_IconFactory_H

#include "PartSet.h"
#include <ModuleBase_IconFactory.h>
#include <Events_Listener.h>

#include <QMap>


/**\class PartSet_IconFactory
 * \ingroup GUI
 * \brief This is a class is redefined in order to provide
 * icons of objects for object browser specific for PartSetModule
 */
class PARTSET_EXPORT PartSet_IconFactory : public ModuleBase_IconFactory, public Events_Listener
{
public:
  /// Constructor
  PartSet_IconFactory();

  /// Returns Icon for the given object
  /// \param theObj an object
  virtual QIcon getIcon(ObjectPtr theObj);

  /// Event Listener method
  /// \param theMessage an event message
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

private:
  static QMap<QString, QString> myIcons;
};

#endif