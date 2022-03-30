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

#include "ModuleBase_Preferences.h"
//#include "ModuleBase_Constants.h"

#include <Config_PropManager.h>

#include <TopAbs_ShapeEnum.hxx>

#include <SUIT_ResourceMgr.h>
#include <SUIT_PreferenceMgr.h>
#include <Qtx.h>

#include <QLayout>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

#ifdef WIN32
#pragma warning(disable : 4456) // for nested foreach
#endif

const QString ModuleBase_Preferences::VIEWER_SECTION = "Viewer";
const QString ModuleBase_Preferences::MENU_SECTION = "Menu";
const QString ModuleBase_Preferences::GENERAL_SECTION = "General";

SUIT_ResourceMgr* ModuleBase_Preferences::myResourceMgr = 0;

SUIT_ResourceMgr* ModuleBase_Preferences::resourceMgr()
{
  if (!myResourceMgr) {
    myResourceMgr = new SUIT_ResourceMgr("SHAPER");
    myResourceMgr->setCurrentFormat("xml");
  }
  return myResourceMgr;
}

bool ModuleBase_Preferences::editPreferences(ModuleBase_Prefs& theModified)
{
  ModuleBase_PreferencesDlg aDlg(resourceMgr(), QApplication::activeWindow());
  aDlg.exec();
  if (aDlg.isChanged()) {
    aDlg.modified(theModified);
    resourceMgr()->save();
    return true;
  }
  return false;
}

void ModuleBase_Preferences::updateConfigByResources()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    QString aVal = myResourceMgr->stringValue(QString(aProp->section().c_str()),
                                              QString(aProp->name().c_str()));
    if (!aVal.isEmpty()) {
      aProp->setValue(aVal.toStdString());
    }
  }
}

void ModuleBase_Preferences::updateResourcesByConfig()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    myResourceMgr->setValue(QString(aProp->section().c_str()), QString(aProp->name().c_str()),
                            QString(aProp->value().c_str()));
  }
}

void ModuleBase_Preferences::resetConfig()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    aProp->setValue(aProp->defaultValue());
  }
}

void ModuleBase_Preferences::loadCustomProps()
{
  if(!myResourceMgr)
    return;
  QStringList aSections = myResourceMgr->sections();
  foreach (QString aSection, aSections)
  {
    QStringList aParams = myResourceMgr->parameters(aSection);
    foreach (QString aParam, aParams)
    {
      Config_Prop* aProp = Config_PropManager::registerProp(aSection.toStdString(),
                                       aParam.toStdString(), "", Config_Prop::Disabled);
      aProp->setValue(myResourceMgr->stringValue(aSection, aParam).toStdString());
    }
  }
}


void ModuleBase_Preferences::createEditContent(ModuleBase_IPrefMgr* thePref, int thePage)
{
  thePref->prefMgr()->setItemIcon(thePage, QIcon(":pictures/module.png"));
  createGeneralTab(thePref, thePage);
  createCustomPage(thePref, thePage);
  updateSketchTab(thePref, thePage);
}

void ModuleBase_Preferences::resetResourcePreferences(SUIT_PreferenceMgr* thePref)
{
  if (!thePref)
    return;

  QtxResourceMgr::WorkingMode aPrev =
    thePref->resourceMgr()->setWorkingMode(QtxResourceMgr::IgnoreUserValues);
  thePref->retrieve();
  thePref->resourceMgr()->setWorkingMode(aPrev);
}

void ModuleBase_Preferences::resetConfigPropPreferences(SUIT_PreferenceMgr* thePref)
{
  resetConfig();
  updateResourcesByConfig();

  // retrieve the reset resource values to the preferences items
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  QStringList aValues;
  QStringList aSections;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    aValues.append(QString(aProp->name().c_str()));
    if (!aSections.contains(aProp->section().c_str()))
      aSections.append(aProp->section().c_str());
    QtxPreferenceItem* anItem = thePref->findItem(QString(aProp->title().c_str()), true);
    if (anItem)
      anItem->retrieve();
  }
}

void ModuleBase_Preferences::createGeneralTab(ModuleBase_IPrefMgr* thePref, int thePageId)
{
  int generalTab = thePref->addPreference(QObject::tr("General"), thePageId,
                                          SUIT_PreferenceMgr::Auto, QString(), QString());
  thePref->setItemProperty("columns", 2, generalTab);

  QStringList actItemList;
  actItemList << QObject::tr("Last part")
              << QObject::tr("All parts")
              << QObject::tr("No activation");

  QList<QVariant> actIdList;
  actIdList << 0 << 1 << 2;

  // Group related to opening a study
  int group = thePref->addPreference(QObject::tr("Opening a study"), generalTab,
                                     SUIT_PreferenceMgr::Auto, QString(), QString());

  int actId = thePref->addPreference(QObject::tr("Activate"), group, SUIT_PreferenceMgr::Selector,
                                     ModuleBase_Preferences::GENERAL_SECTION,
                                     "part_activation_study");
  thePref->setItemProperty("strings", actItemList, actId);
  thePref->setItemProperty("indexes", actIdList, actId);

  QStringList visuItemList;
  visuItemList << QObject::tr("As stored in HDF")
               << QObject::tr("Last item in each folder")
               << QObject::tr("All items")
               << QObject::tr("No visualization");

  QList<QVariant> visuIdList;
  visuIdList << 0 << 1 << 2 << 3;

  int visuId = thePref->addPreference(QObject::tr("Display"), group, SUIT_PreferenceMgr::Selector,
                                      ModuleBase_Preferences::GENERAL_SECTION,
                                      "part_visualization_study");
  thePref->setItemProperty("strings", visuItemList, visuId);
  thePref->setItemProperty("indexes", visuIdList, visuId);

  // Group related to running a python script
  group = thePref->addPreference(QObject::tr("Launching a python script"), generalTab,
                                 SUIT_PreferenceMgr::Auto, QString(), QString());

  visuItemList.clear();
  visuItemList << QObject::tr("Last item in each folder")
               << QObject::tr("All items")
               << QObject::tr("No visualization");

  visuIdList.clear();
  visuIdList << 0 << 1 << 2;

  visuId = thePref->addPreference(QObject::tr("Display"), group, SUIT_PreferenceMgr::Selector,
                                  ModuleBase_Preferences::GENERAL_SECTION,
                                  "part_visualization_script");
  thePref->setItemProperty("strings", visuItemList, visuId);
  thePref->setItemProperty("indexes", visuIdList, visuId);
}

void ModuleBase_Preferences::updateSketchTab(ModuleBase_IPrefMgr* thePref, int thePageId)
{
  int sketchTab   = thePref->addPreference(QObject::tr("Sketch"), thePageId,
                             SUIT_PreferenceMgr::Auto, QString(), QString());
  int allowChange = thePref->addPreference(
                           QObject::tr("Allow automatic constraint substitution/remove"),
                           sketchTab, SUIT_PreferenceMgr::GroupBox,
                           "Sketch", "allow_change_constraint");
  thePref->addPreference(QObject::tr("Notify automatic constraint substitution/remove"),
           allowChange, SUIT_PreferenceMgr::Bool, "Sketch", "notify_change_constraint");
}

void ModuleBase_Preferences::createCustomPage(ModuleBase_IPrefMgr* thePref, int thePageId)
{
  SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();

  // Make a Tab from each section
  std::list<std::string> aSections = Config_PropManager::getSections();
  std::list<std::string>::const_iterator it;
  for (it = aSections.cbegin(); it != aSections.cend(); ++it) {
    Config_Properties aProps = Config_PropManager::getProperties(*it);
    int aTab = thePref->prefMgr()->addItem(QString((*it).c_str()), thePageId);
    thePref->prefMgr()->setItemProperty("columns", 2, aTab);

    Config_Properties::const_iterator aIt;
    for (aIt = aProps.cbegin(); aIt != aProps.cend(); ++aIt) {
      Config_Prop* aProp = (*aIt);
      // check that the property is defined
      QString aSection(aProp->section().c_str());
      QString aName(aProp->name().c_str());
      if (!aResMgr->hasValue(aSection, aName))
        aResMgr->setValue(aSection, aName, QString(aProp->value().c_str()));

      // Add item
      if (aProp->type() != Config_Prop::Disabled) {
        SUIT_PreferenceMgr::PrefItemType aPrefType = SUIT_PreferenceMgr::Auto;
        switch (aProp->type()) {
        case Config_Prop::Directory:
          aPrefType = SUIT_PreferenceMgr::File;
          break;
        case Config_Prop::Cursor:
          aPrefType = SUIT_PreferenceMgr::Selector;
          break;
        default:
          aPrefType = (SUIT_PreferenceMgr::PrefItemType) aProp->type();
        }

        int anId = thePref->addPreference(QObject::tr(aProp->title().c_str()), aTab, aPrefType,
                                          QString::fromStdString(aProp->section()),
                                          QString::fromStdString(aProp->name()));

        switch (aProp->type()) {
        case Config_Prop::Directory:
          thePref->setItemProperty("path_type", Qtx::PT_Directory, anId);
          break;
        case SUIT_PreferenceMgr::DblSpin:
          if (aProp->min() != "") {
            double aMin = QString(aProp->min().c_str()).toDouble();
            thePref->setItemProperty("min", aMin, anId);
          }
          if (aProp->max() != "") {
            double aMax = QString(aProp->max().c_str()).toDouble();
            thePref->setItemProperty("max", aMax, anId);
          }
          break;
        case SUIT_PreferenceMgr::IntSpin:
          if (aProp->min() != "") {
            int aMin = QString(aProp->min().c_str()).toInt();
            thePref->setItemProperty("min", aMin, anId);
          }
          if (aProp->max() != "") {
            int aMax = QString(aProp->max().c_str()).toInt();
            thePref->setItemProperty("max", aMax, anId);
          }
          break;
        case Config_Prop::Cursor:
          {
            QList<QVariant> aIndicesList;
            QList<QVariant> aIconsList;
            aIndicesList << 0 << 1 << 2;
            aIconsList << QPixmap(":pictures/ArrowCursor.png") <<
              QPixmap(":pictures/CrossCursor.png") <<
              QPixmap(":pictures/HandCursor.png");

            thePref->setItemProperty("indexes", aIndicesList, anId);
            thePref->setItemProperty("icons", aIconsList, anId);
          }
          break;
        default: // [to avoid compilation warnings]
          break;
        }
      }
    }
  }
}

/**
* Implementation of preferences manager interface
*/
class ModuleBase_PrefMgr: public ModuleBase_IPrefMgr
{
public:
  /// Constructor
  /// \param theMgr a preferences manager
  ModuleBase_PrefMgr(ModuleBase_PreferencesMgr* theMgr):myMgr(theMgr) {}

  virtual int addPreference(const QString& theLbl, int pId,
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName )
  {
    return myMgr->addItem(theLbl, pId, theType, theSection, theName);
  }

  virtual void setItemProperty( const QString& thePropName, const QVariant& theValue,
                               const int theId = -1) {
    myMgr->setItemProperty(thePropName, theValue, theId);
  }

  virtual SUIT_PreferenceMgr* prefMgr() const { return myMgr; }

private:
  ModuleBase_PreferencesMgr* myMgr;
};

//**********************************************************
//**********************************************************
//**********************************************************
ModuleBase_PreferencesDlg::ModuleBase_PreferencesDlg(SUIT_ResourceMgr* theResurces,
  QWidget* theParent)
    : QDialog(theParent,
      Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
      myIsChanged(false)
{
  setWindowTitle(tr("Edit preferences"));

  QVBoxLayout* main = new QVBoxLayout(this);
  main->setMargin(5);
  main->setSpacing(5);

  myPreferences = new ModuleBase_PreferencesMgr(theResurces, this);
  main->addWidget(myPreferences);

  setFocusProxy(myPreferences);
  myPreferences->setFrameStyle(QFrame::Box | QFrame::Sunken);

  QDialogButtonBox* aBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
                                                   QDialogButtonBox::Reset,
                                                   Qt::Horizontal, this);
  QPushButton* aDefaultButton = aBtnBox->button(QDialogButtonBox::Reset);
  aDefaultButton->setText(tr("Default"));
  connect(aDefaultButton, SIGNAL(clicked()), this, SLOT(onDefault()));

  main->addWidget(aBtnBox);
  connect(aBtnBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aBtnBox, SIGNAL(rejected()), this, SLOT(reject()));
  createEditors();

  myPreferences->retrieve();
}

ModuleBase_PreferencesDlg::~ModuleBase_PreferencesDlg()
{
}

void ModuleBase_PreferencesDlg::createEditors()
{
  int aPage = myPreferences->addItem(tr("Desktop"));
  myPreferences->setItemIcon(aPage, QIcon(":pictures/view_prefs.png"));

  createMenuPage(aPage);
  createViewerPage(aPage);

  aPage = myPreferences->addItem(tr("Module"));
  myPreferences->setItemIcon(aPage, QIcon(":pictures/module.png"));

  ModuleBase_PrefMgr aMgr(myPreferences);
  ModuleBase_Preferences::createEditContent(&aMgr, aPage);
}

void ModuleBase_PreferencesDlg::createViewerPage(int thePageId)
{
  int viewTab = myPreferences->addItem(tr("Viewer"), thePageId);

  QStringList gradList;
  gradList << tr("Horizontal gradient") << tr("Vertical gradient") << tr("First diagonal gradient")
           << tr("Second diagonal gradient") << tr("First corner gradient")
           << tr("Second corner gradient") << tr("Third corner gradient")
           << tr("Fourth corner gradient");

  QList<QVariant> idList;
  for (int i = 0; i < gradList.size(); i++)
    idList << i;

  int bgGroup = myPreferences->addItem(tr("Background"), viewTab);

  QString aImgFiles("Image files (*.bmp *.gif *.pix *.xwd *.rgb *.rs)");

  int bgId = myPreferences->addItem(tr("Viewer 3d"), bgGroup, SUIT_PreferenceMgr::Background,
                                    ModuleBase_Preferences::VIEWER_SECTION, "background");
  myPreferences->setItemProperty("gradient_names", gradList, bgId);
  myPreferences->setItemProperty("gradient_ids", idList, bgId);
  myPreferences->setItemProperty("texture_enabled", true, bgId);
  myPreferences->setItemProperty("texture_center_enabled", true, bgId);
  myPreferences->setItemProperty("texture_tile_enabled", true, bgId);
  myPreferences->setItemProperty("texture_stretch_enabled", true, bgId);
  myPreferences->setItemProperty("custom_enabled", false, bgId);
  myPreferences->setItemProperty("image_formats", aImgFiles, bgId);

  // Create other parameters group in viewer tab
  int selectionGroup = myPreferences->addItem(tr("Default selection"), viewTab);
  myPreferences->setItemProperty("columns", 3, selectionGroup);
  myPreferences->addItem(tr("Faces"), selectionGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "face-selection");
  myPreferences->addItem(tr("Edges"), selectionGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "edge-selection");
  myPreferences->addItem(tr("Vertices"), selectionGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "vertex-selection");

  int sensitivityGroup = myPreferences->addItem(tr("Selection sensitivity"), viewTab);
  myPreferences->setItemProperty("columns", 2, sensitivityGroup);
  myPreferences->addItem(tr("Vertex"), sensitivityGroup, SUIT_PreferenceMgr::DblSpin,
                         ModuleBase_Preferences::VIEWER_SECTION, "point-selection-sensitivity");
  myPreferences->addItem(tr("Edge"), sensitivityGroup, SUIT_PreferenceMgr::DblSpin,
                         ModuleBase_Preferences::VIEWER_SECTION, "edge-selection-sensitivity");

  int highlightGroup = myPreferences->addItem(tr("Additional highlighting"), viewTab);
  myPreferences->setItemProperty("columns", 2, highlightGroup);
  myPreferences->addItem(tr("In 3d mode"), highlightGroup,
    SUIT_PreferenceMgr::Bool, ModuleBase_Preferences::VIEWER_SECTION, "highlighting-3d");
  myPreferences->addItem(tr("In 2d mode"), highlightGroup,
    SUIT_PreferenceMgr::Bool, ModuleBase_Preferences::VIEWER_SECTION, "highlighting-2d");

  int colorScaleGroup = myPreferences->addItem(tr("Color scale"), viewTab);
  myPreferences->setItemProperty("columns", 4, colorScaleGroup);
  int aItem = myPreferences->addItem(tr("X position"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_x_position");
  myPreferences->setItemProperty("min", 0, aItem);
  myPreferences->setItemProperty("max", 1, aItem);

  aItem = myPreferences->addItem(tr("Y position"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_y_position");
  myPreferences->setItemProperty("min", 0, aItem);
  myPreferences->setItemProperty("max", 1, aItem);

  aItem = myPreferences->addItem(tr("Width"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_width");
  myPreferences->setItemProperty("min", 0, aItem);
  myPreferences->setItemProperty("max", 1, aItem);

  aItem = myPreferences->addItem(tr("Height"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_height");
  myPreferences->setItemProperty("min", 0, aItem);
  myPreferences->setItemProperty("max", 1, aItem);

  aItem = myPreferences->addItem(tr("Intervals number"), colorScaleGroup,
    SUIT_PreferenceMgr::Integer, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_nb_intervals");
  myPreferences->setItemProperty("min", 0, aItem);
  myPreferences->setItemProperty("max", 100, aItem);

  aItem = myPreferences->addItem(tr("Text height"), colorScaleGroup,
    SUIT_PreferenceMgr::Integer, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_text_height");
  myPreferences->setItemProperty("min", 0, aItem);
  myPreferences->setItemProperty("max", 100, aItem);

  aItem = myPreferences->addItem(tr("Text color"), colorScaleGroup,
    SUIT_PreferenceMgr::Color, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_text_color");
}

void ModuleBase_PreferencesDlg::createMenuPage(int thePageId)
{
  int aMenuTab = myPreferences->addItem(tr("Main menu"), thePageId);

  int aSizeGroup = myPreferences->addItem(tr("Size"), aMenuTab);
  myPreferences->setItemProperty("columns", 1, aSizeGroup);

  int aRowsNb = myPreferences->addItem(tr("Number of rows"), aSizeGroup,
                                       SUIT_PreferenceMgr::IntSpin,
                                       ModuleBase_Preferences::MENU_SECTION,
                                       "rows_number");
  myPreferences->setItemProperty("min", 1, aRowsNb);
  myPreferences->setItemProperty("max", 6, aRowsNb);

  myPreferences->addItem(tr("Show Status Bar"), aSizeGroup,
                         SUIT_PreferenceMgr::Bool, ModuleBase_Preferences::MENU_SECTION,
                         "status_bar");
}

void ModuleBase_PreferencesDlg::accept()
{
  myPreferences->store();
  myIsChanged = true;

  // Save custom properties
  ModuleBase_Preferences::updateConfigByResources();
  QDialog::accept();
}

void ModuleBase_PreferencesDlg::modified(ModuleBase_Prefs& theModified) const
{
  theModified = myPreferences->modified();
}

void ModuleBase_PreferencesDlg::onDefault()
{
  // reset main resources. It throwns all resource manager items to the
  // initial/default state. If there is no a default state of the item,
  // it will be filled with an empty value. It concernerned to plugin
  // config items, like visualization color. The main xml do not contains
  // default values for them. So, it is important to reset the config
  // properties after reseting the resources preferences.
  ModuleBase_Preferences::resetResourcePreferences(myPreferences);
  // reset plugin's resources. It fills the config resources with the default
  // values, stores result in the resource manager and retrieve the preferences
  // items with these values.
  ModuleBase_Preferences::resetConfigPropPreferences(myPreferences);
}

void ModuleBase_PreferencesDlg::showEvent(QShowEvent* theEvent)
{
  QDialog::showEvent(theEvent);
  adjustSize();
}

//**********************************************************
//**********************************************************
//**********************************************************
void ModuleBase_PreferencesMgr::changedResources(const ResourceMap& theMap)
{
  myModified.clear();
  ResourceMap::ConstIterator it;
  QString sec, param;
  for (it = theMap.begin(); it != theMap.end(); ++it) {
    ModuleBase_Pref aPref;
    it.key()->resource(aPref.first, aPref.second);
    myModified.append(aPref);
  }
}
