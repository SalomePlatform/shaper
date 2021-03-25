// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
