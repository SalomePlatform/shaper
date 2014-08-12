// File:        XGUI_Preferences.cpp
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "XGUI_Preferences.h"
#include "XGUI_Constants.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_PreferenceMgr.h>

#include <QLayout>
#include <QApplication>
#include <QDialogButtonBox>


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



//**********************************************************
//**********************************************************
//**********************************************************
XGUI_PreferencesDlg::XGUI_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent)
  : QDialog(theParent), myIsChanged(false)
{
  setWindowTitle( tr("Edit preferences") );

  QVBoxLayout* main = new QVBoxLayout(this);
  main->setMargin( 5 );
  main->setSpacing( 5 );

  myPreferences = new XGUI_PreferencesMgr(theResurces, this);
  main->addWidget( myPreferences );

  setFocusProxy( myPreferences );
  myPreferences->setFrameStyle( QFrame::Box | QFrame::Sunken );

  QDialogButtonBox* aBtnBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
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
  int aLFpage = myPreferences->addItem("Desktop");
  myPreferences->setItemIcon(aLFpage, QIcon(":pictures/view_prefs.png"));

  createMenuPage(aLFpage);
  createViewerPage(aLFpage);
}

void XGUI_PreferencesDlg::createViewerPage(int thePageId)
{
  int viewTab = myPreferences->addItem( tr("Viewer"), thePageId );

  QStringList gradList;
  gradList << tr("Horizontal gradient")     << tr("Vertical gradient")        <<
              tr("First diagonal gradient") << tr("Second diagonal gradient") <<
              tr("First corner gradient")   << tr("Second corner gradient")   <<
              tr("Third corner gradient")   << tr("Fourth corner gradient");
  
  QList<QVariant> idList;
  for (int i = 0; i < gradList.size(); i++)
    idList << i;

  int bgGroup = myPreferences->addItem( tr( "Background" ), viewTab );

  QString aImgFiles("Image files (*.bmp *.gif *.pix *.xwd *.rgb *.rs)");

  int bgId = myPreferences->addItem( tr("Viewer 3d" ), bgGroup,
                                  SUIT_PreferenceMgr::Background, 
                                  XGUI_Preferences::VIEWER_SECTION, "background" );
  myPreferences->setItemProperty( "gradient_names", gradList, bgId );
  myPreferences->setItemProperty( "gradient_ids", idList, bgId );
  myPreferences->setItemProperty( "texture_enabled", true, bgId );
  myPreferences->setItemProperty( "texture_center_enabled", true, bgId );
  myPreferences->setItemProperty( "texture_tile_enabled", true, bgId );
  myPreferences->setItemProperty( "texture_stretch_enabled", true, bgId );
  myPreferences->setItemProperty( "custom_enabled", false, bgId );
  myPreferences->setItemProperty( "image_formats", aImgFiles, bgId );
}

void XGUI_PreferencesDlg::createMenuPage(int thePageId)
{
  int aMenuTab = myPreferences->addItem(tr("Main menu"), thePageId );

  int aSizeGroup = myPreferences->addItem(tr("Size"), aMenuTab );
  myPreferences->setItemProperty( "columns", 2, aSizeGroup );

  int aItemSizeItem = myPreferences->addItem( tr( "Item size" ), 
                                              aSizeGroup, 
                                              SUIT_PreferenceMgr::Selector,
                                              XGUI_Preferences::MENU_SECTION, 
                                              "item_size" );
  QStringList aValuesList;
  QList<QVariant> anIndicesList;
  aValuesList << tr("Small") << tr("Medium") << tr("Large");
  anIndicesList << 20 << 25 << 30;
  myPreferences->setItemProperty( "strings", aValuesList,   aItemSizeItem );
  myPreferences->setItemProperty( "indexes", anIndicesList, aItemSizeItem );

  int aRowsNb = myPreferences->addItem( tr( "Number of rows" ), 
                                        aSizeGroup,
                                        SUIT_PreferenceMgr::IntSpin, 
                                        XGUI_Preferences::MENU_SECTION, 
                                        "rows_number" );
  myPreferences->setItemProperty( "min", 1, aRowsNb );
  myPreferences->setItemProperty( "max", 10, aRowsNb );
}

void XGUI_PreferencesDlg::accept()
{
  myPreferences->store();
  myIsChanged = true;
  QDialog::accept();
}

void XGUI_PreferencesDlg::modified(XGUI_Prefs& theModified) const
{
  theModified = myPreferences->modified();
}



//**********************************************************
//**********************************************************
//**********************************************************
void XGUI_PreferencesMgr::changedResources( const ResourceMap& theMap)
{
  myModified.clear();
  ResourceMap::ConstIterator it;
  QString sec, param;
  for (it = theMap.begin(); it != theMap.end(); ++it ) {
    XGUI_Pref aPref;
    it.key()->resource( aPref.first, aPref.second );
    myModified.append(aPref);
  }
}
