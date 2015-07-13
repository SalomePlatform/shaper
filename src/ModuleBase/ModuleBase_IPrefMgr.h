// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IPrefMgr.h
// Created:     10 Sept 2014
// Author:      Vitaly SMETANNIKOV


#ifndef ModuleBase_IPrefMgr_H
#define ModuleBase_IPrefMgr_H

#include "ModuleBase.h"

#include <QString>
#include <SUIT_PreferenceMgr.h>

/** 
* \ingroup GUI
* An interface class which provides incapsulation of SUIT_PreferenceMgr class instance 
* It is used in order to make common interface to Preference manager in Salome 
* and this application
*/
class MODULEBASE_EXPORT ModuleBase_IPrefMgr
{
public:
  /// Default constructor
  ModuleBase_IPrefMgr();
  /// Virtual destructor
  virtual ~ModuleBase_IPrefMgr();

  /** 
  * Add preference item into preference dialog box
  * \param theLbl - label of the item
  * \param pId - id of container item
  * \param theType - type of the item
  * \param theSection - resouce section name
  * \param theName - name of the resource
  * Returns Id of the ctreated item
  */
  virtual int addPreference(const QString& theLbl, int pId, 
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName ) = 0;

  /**
  * \brief Set property of an item
  * \param thePropName the property name
  * \param theValue the value of the property
  * \param theId id of the property
  */
  virtual void setItemProperty(const QString& thePropName,
                               const QVariant& theValue,
                               const int theId = -1) = 0;

  /// Returns incapsulated preference manager
  virtual SUIT_PreferenceMgr* prefMgr() const = 0;
};

#endif
