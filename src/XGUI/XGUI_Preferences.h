// File:        XGUI_Preferences.h
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef XGUI_Preferences_H
#define XGUI_Preferences_H

#include "XGUI.h"

#include <Config_PropManager.h>
#include <SUIT_PreferenceMgr.h>
#include <QDialog>

class SUIT_ResourceMgr;
class QWidget;

// Pair of values: section name, value name
typedef QPair<QString, QString> XGUI_Pref;
typedef QList<XGUI_Pref> XGUI_Prefs;


//***********************************************************************
/// Class for manipulation  with preferences in the application
class XGUI_EXPORT XGUI_Preferences
{
public:
  static const QString VIEWER_SECTION;
  static const QString MENU_SECTION;

  static bool editPreferences(XGUI_Prefs& theModified);

  static SUIT_ResourceMgr* resourceMgr();

private:
  static SUIT_ResourceMgr* myResourceMgr;
};

//***********************************************************************
/// Manager of preferences
class XGUI_EXPORT XGUI_PreferencesMgr : public SUIT_PreferenceMgr
{
  Q_OBJECT
public:
  XGUI_PreferencesMgr(QtxResourceMgr* theResource, QWidget* theParent)
    :SUIT_PreferenceMgr(theResource, theParent) {}

  virtual ~XGUI_PreferencesMgr() {}

  XGUI_Prefs modified() const { return myModified; }

protected:
  virtual void changedResources( const ResourceMap& theMap);

private:
  XGUI_Prefs myModified;
};


//***********************************************************************
/// Dialog box for preferences editing
class XGUI_EXPORT XGUI_PreferencesDlg: public QDialog
{
  Q_OBJECT
public:
  XGUI_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent = 0);
  virtual ~XGUI_PreferencesDlg();

  bool isChanged() const { return myIsChanged; }

  void modified(XGUI_Prefs& theModified) const;

public slots:
  virtual void accept();

private:
  void createEditors();
  void createViewerPage(int thePageId);
  void createMenuPage(int thePageId);
  void createCustomPage(int thePageId, Config_Properties& theProps);
  
  XGUI_PreferencesMgr* myPreferences;
  bool myIsChanged;
};


#endif