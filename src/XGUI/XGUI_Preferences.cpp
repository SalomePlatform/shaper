// File:        XGUI_Preferences.cpp
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "XGUI_Preferences.h"
#include "XGUI_Constants.h"

#include <Config_PropManager.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_PreferenceMgr.h>

#include <QLayout>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

const QString XGUI_Preferences::VIEWER_SECTION = "Viewer";
const QString XGUI_Preferences::MENU_SECTION = "Menu";

SUIT_ResourceMgr* XGUI_Preferences::myResourceMgr = 0;

SUIT_ResourceMgr* XGUI_Preferences::resourceMgr()
{
  if (!myResourceMgr) {
    myResourceMgr = new SUIT_ResourceMgr("NewGeom");
    myResourceMgr->setCurrentFormat("xml");
  }
  return myResourceMgr;
}

bool XGUI_Preferences::editPreferences(XGUI_Prefs& theModified)
{
  XGUI_PreferencesDlg aDlg(resourceMgr(), QApplication::activeWindow());
  aDlg.exec();
  if (aDlg.isChanged()) {
    aDlg.modified(theModified);
    resourceMgr()->save();
    return true;
  }
  return false;
}

void XGUI_Preferences::updateConfigByResources()
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

void XGUI_Preferences::updateResourcesByConfig()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    myResourceMgr->setValue(QString(aProp->section().c_str()), QString(aProp->name().c_str()),
                            QString(aProp->value().c_str()));
  }
}

void XGUI_Preferences::resetConfig()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    aProp->setValue(aProp->defaultValue());
  }
}

void XGUI_Preferences::loadCustomProps()
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


void XGUI_Preferences::createEditContent(XGUI_IPrefMgr* thePref, int thePage)
{
  thePref->prefMgr()->setItemIcon(thePage, QIcon(":pictures/module.png"));
  createCustomPage(thePref, thePage);
}


void XGUI_Preferences::createCustomPage(XGUI_IPrefMgr* thePref, int thePageId)
{
  SUIT_ResourceMgr* aResMgr = XGUI_Preferences::resourceMgr();
  bool isResModified = false;

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
      if (!aResMgr->hasValue(aSection, aName)) {
        aResMgr->setValue(aSection, aName, QString(aProp->value().c_str()));
        isResModified = true;
      }
      // Add item
      if (aProp->type() != Config_Prop::Disabled)
        thePref->addPreference(QObject::tr(aProp->title().c_str()), aTab,
                               (SUIT_PreferenceMgr::PrefItemType) aProp->type(),
                               QString(aProp->section().c_str()), QString(aProp->name().c_str()));
    }
  }
}

//**********************************************************
//**********************************************************
//**********************************************************
class XGUI_PrefMgr: public XGUI_IPrefMgr
{
public:
  XGUI_PrefMgr(XGUI_PreferencesMgr* theMgr):myMgr(theMgr) {}

  virtual int addPreference(const QString& theLbl, int pId, 
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName )
  {
    return myMgr->addItem(theLbl, pId, theType, theSection, theName);
  }

  virtual SUIT_PreferenceMgr* prefMgr() const { return myMgr; }

private:
  XGUI_PreferencesMgr* myMgr;
};

//**********************************************************
//**********************************************************
//**********************************************************
XGUI_PreferencesDlg::XGUI_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent)
    : QDialog(theParent),
      myIsChanged(false)
{
  setWindowTitle(tr("Edit preferences"));

  QVBoxLayout* main = new QVBoxLayout(this);
  main->setMargin(5);
  main->setSpacing(5);

  myPreferences = new XGUI_PreferencesMgr(theResurces, this);
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
  setMinimumSize(800, 200);
}

XGUI_PreferencesDlg::~XGUI_PreferencesDlg()
{
}

void XGUI_PreferencesDlg::createEditors()
{
  int aPage = myPreferences->addItem(tr("Desktop"));
  myPreferences->setItemIcon(aPage, QIcon(":pictures/view_prefs.png"));

  createMenuPage(aPage);
  createViewerPage(aPage);

  aPage = myPreferences->addItem(tr("Module"));
  myPreferences->setItemIcon(aPage, QIcon(":pictures/module.png"));

  XGUI_PrefMgr aMgr(myPreferences);
  XGUI_Preferences::createEditContent(&aMgr, aPage);
}

void XGUI_PreferencesDlg::createViewerPage(int thePageId)
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
                                    XGUI_Preferences::VIEWER_SECTION, "background");
  myPreferences->setItemProperty("gradient_names", gradList, bgId);
  myPreferences->setItemProperty("gradient_ids", idList, bgId);
  myPreferences->setItemProperty("texture_enabled", true, bgId);
  myPreferences->setItemProperty("texture_center_enabled", true, bgId);
  myPreferences->setItemProperty("texture_tile_enabled", true, bgId);
  myPreferences->setItemProperty("texture_stretch_enabled", true, bgId);
  myPreferences->setItemProperty("custom_enabled", false, bgId);
  myPreferences->setItemProperty("image_formats", aImgFiles, bgId);
}

void XGUI_PreferencesDlg::createMenuPage(int thePageId)
{
  int aMenuTab = myPreferences->addItem(tr("Main menu"), thePageId);

  int aSizeGroup = myPreferences->addItem(tr("Size"), aMenuTab);
  myPreferences->setItemProperty("columns", 1, aSizeGroup);

  int aRowsNb = myPreferences->addItem(tr("Number of rows"), aSizeGroup,
                                       SUIT_PreferenceMgr::IntSpin, XGUI_Preferences::MENU_SECTION,
                                       "rows_number");
  myPreferences->setItemProperty("min", 1, aRowsNb);
  myPreferences->setItemProperty("max", 6, aRowsNb);
}

void XGUI_PreferencesDlg::accept()
{
  myPreferences->store();
  myIsChanged = true;

  // Save custom properties
  XGUI_Preferences::updateConfigByResources();
  QDialog::accept();
}

void XGUI_PreferencesDlg::modified(XGUI_Prefs& theModified) const
{
  theModified = myPreferences->modified();
}

void XGUI_PreferencesDlg::onDefault()
{
  // reset main resources
  QtxResourceMgr::WorkingMode aPrev = myPreferences->resourceMgr()->setWorkingMode
                                               (QtxResourceMgr::IgnoreUserValues);
  myPreferences->retrieve();
  myPreferences->resourceMgr()->setWorkingMode(aPrev);

  // reset plugin's resources
  XGUI_Preferences::resetConfig();
  XGUI_Preferences::updateResourcesByConfig();

  myPreferences->retrieve();
}

//**********************************************************
//**********************************************************
//**********************************************************
void XGUI_PreferencesMgr::changedResources(const ResourceMap& theMap)
{
  myModified.clear();
  ResourceMap::ConstIterator it;
  QString sec, param;
  for (it = theMap.begin(); it != theMap.end(); ++it) {
    XGUI_Pref aPref;
    it.key()->resource(aPref.first, aPref.second);
    myModified.append(aPref);
  }
}
