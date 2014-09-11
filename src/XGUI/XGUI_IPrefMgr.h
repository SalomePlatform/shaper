// File:        XGUI_IPrefMgr.h
// Created:     10 Sept 2014
// Author:      Vitaly SMETANNIKOV


#ifndef XGUI_IPrefMgr_H
#define XGUI_IPrefMgr_H

#include <QString>
#include <SUIT_PreferenceMgr.h>

/** 
* An interface class which provides incapsulation of SUIT_PreferenceMgr class instance 
* It is used in order to make common interface to Preference manager in Salome 
* and this application
*/
class XGUI_IPrefMgr
{
public:
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

  /// Returns incapsulated preference manager
  virtual SUIT_PreferenceMgr* prefMgr() const = 0;
};

#endif