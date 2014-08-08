// File:        XGUI_Preferences.h
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef XGUI_Preferences_H
#define XGUI_Preferences_H

#include "XGUI.h"
#include <QDialog>

class SUIT_ResourceMgr;
class SUIT_PreferenceMgr;
class QWidget;

class XGUI_EXPORT XGUI_Preferences
{
public:

  static void editPreferences();

  static SUIT_ResourceMgr* resourceMgr();

private:
  static SUIT_ResourceMgr* myResourceMgr;
};


class XGUI_PreferencesDlg: public QDialog
{
  Q_OBJECT
public:
  XGUI_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent = 0);
  virtual ~XGUI_PreferencesDlg();

private:
  void createEditors();

  SUIT_PreferenceMgr* myPreferences;
};


#endif