// File:        XGUI_IPrefMgr.h
// Created:     10 Sept 2014
// Author:      Vitaly SMETANNIKOV


#ifndef XGUI_IPrefMgr_H
#define XGUI_IPrefMgr_H

#include <QString>
#include <SUIT_PreferenceMgr.h>

class XGUI_IPrefMgr
{
public:
  virtual int addPreference(const QString& theLbl, int pId, 
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName ) = 0;

  virtual SUIT_PreferenceMgr* prefMgr() const = 0;
};

#endif