// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Preferences.h
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_Preferences_H
#define ModuleBase_Preferences_H

#include "ModuleBase.h"
#include "ModuleBase_IPrefMgr.h"

#include <SUIT_PreferenceMgr.h>
#include <QDialog>

class SUIT_ResourceMgr;
class QWidget;

/// Pair of values: section name, value name
typedef QPair<QString, QString> ModuleBase_Pref;

/// List of preferences
typedef QList<ModuleBase_Pref> ModuleBase_Prefs;

//***********************************************************************
/// \ingroup GUI
/// Class for manipulation  with preferences in the application
class MODULEBASE_EXPORT ModuleBase_Preferences
{
 public:
   /// Name of preferences of viewer section
  static const QString VIEWER_SECTION;

   /// Name of preferences of menu section
  static const QString MENU_SECTION;

  /// Shows a dialog box to edit preferences
  /// \param theModified a list of modified preferences
  static bool editPreferences(ModuleBase_Prefs& theModified);

  /// Returns currently installed resource manager
  static SUIT_ResourceMgr* resourceMgr();

  /// Sets a resource manager
  /// It is used in case of necessity to define external resource manager (not NewGeom)
  /// \param theMgr resource manager
  static void setResourceMgr(SUIT_ResourceMgr* theMgr) { myResourceMgr = theMgr; }

  /// Updates Config_PropManager properties by module from SUIT_ResourceMgr
  static void updateConfigByResources();

  /// Updates SUIT_ResourceMgr values by Config_PropManager properties
  static void updateResourcesByConfig();

  /// Set default values to the Config_PropManager properties
  static void resetConfig();

  /// Loads properties defined by module to Config_PropManager
  static void loadCustomProps();

  /// Create editable content
  /// \param thePref interface to preference manager
  /// \param thePage an id of a page
  static void createEditContent(ModuleBase_IPrefMgr* thePref, int thePage);

private:
  /// Creates content of preferences editing widget
  static void createCustomPage(ModuleBase_IPrefMgr* thePref, int thePageId);

  static SUIT_ResourceMgr* myResourceMgr;
};

//***********************************************************************
/// \ingroup GUI
/// Manager of preferences
class MODULEBASE_EXPORT ModuleBase_PreferencesMgr : public SUIT_PreferenceMgr
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theResource resource manager
   /// \param theParent a paren widget
  ModuleBase_PreferencesMgr(QtxResourceMgr* theResource, QWidget* theParent)
      : SUIT_PreferenceMgr(theResource, theParent)
  {
  }

  virtual ~ModuleBase_PreferencesMgr()
  {
  }

  /// Returns True if preferences were modified
  ModuleBase_Prefs modified() const
  {
    return myModified;
  }

 protected:
   /// Store changed resource
  virtual void changedResources(const ResourceMap& theMap);

 private:
  ModuleBase_Prefs myModified;
};

//***********************************************************************
/// \ingroup GUI
/// Dialog box for preferences editing
class MODULEBASE_EXPORT ModuleBase_PreferencesDlg : public QDialog
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theResurces resources manager
   /// \param theParent a parent widget
  ModuleBase_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent = 0);
  virtual ~ModuleBase_PreferencesDlg();

  /// Returns True if preferences were changed
  bool isChanged() const
  {
    return myIsChanged;
  }

  /// Returns list of modified preferences
  /// \param theModified output list of modified preferences
  void modified(ModuleBase_Prefs& theModified) const;

 public slots:
   /// A slot called on Ok button press
  virtual void accept();

protected slots:
  /// A slot called on Default button press
  void onDefault();

 private:
   /// Create editors for aplication properties
   void createEditors();

   /// Create a viewer page in dialog box
   void createViewerPage(int thePageId);
   
   /// Create menu properties page in the dialog box
   void createMenuPage(int thePageId);

  ModuleBase_PreferencesMgr* myPreferences;
  bool myIsChanged;
};

#endif
