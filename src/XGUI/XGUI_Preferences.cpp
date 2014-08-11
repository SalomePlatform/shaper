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

SUIT_ResourceMgr* XGUI_Preferences::myResourceMgr = 0;


SUIT_ResourceMgr* XGUI_Preferences::resourceMgr()
{
  if (!myResourceMgr) {
    myResourceMgr = new SUIT_ResourceMgr("NewGeom");
    myResourceMgr->setCurrentFormat("xml");
  }
  return myResourceMgr;
}

void XGUI_Preferences::editPreferences()
{
  XGUI_PreferencesDlg aDlg(resourceMgr(), QApplication::activeWindow());
  aDlg.exec();
}



//**********************************************************
//**********************************************************
//**********************************************************
XGUI_PreferencesDlg::XGUI_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent)
  : QDialog(theParent)
{
  setWindowTitle( tr("Edit preferences") );

  QVBoxLayout* main = new QVBoxLayout(this);
  main->setMargin( 5 );
  main->setSpacing( 5 );

  myPreferences = new SUIT_PreferenceMgr(theResurces, this);
  main->addWidget( myPreferences );

  setFocusProxy( myPreferences );
  myPreferences->setFrameStyle( QFrame::Box | QFrame::Sunken );
  //myPreferences->show();

  QDialogButtonBox* aBtnBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  main->addWidget(aBtnBox);
  connect(aBtnBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aBtnBox, SIGNAL(rejected()), this, SLOT(reject()));  

  myPreferences->retrieve();
  createEditors();
}

XGUI_PreferencesDlg::~XGUI_PreferencesDlg()
{
}

void XGUI_PreferencesDlg::createEditors()
{
  int aLFpage = myPreferences->addItem("Look&Feel");
  myPreferences->setItemIcon(aLFpage, QIcon(":pictures/view_prefs.png"));

  int viewTab = myPreferences->addItem( tr("Viewer"), aLFpage );

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
                                  SUIT_PreferenceMgr::Background, "Viewer", "background" );
  myPreferences->setItemProperty( "gradient_names", gradList, bgId );
  myPreferences->setItemProperty( "gradient_ids", idList, bgId );
  myPreferences->setItemProperty( "texture_enabled", true, bgId );
  myPreferences->setItemProperty( "texture_center_enabled", true, bgId );
  myPreferences->setItemProperty( "texture_tile_enabled", true, bgId );
  myPreferences->setItemProperty( "texture_stretch_enabled", true, bgId );
  myPreferences->setItemProperty( "custom_enabled", false, bgId );
  myPreferences->setItemProperty( "image_formats", aImgFiles, bgId );

}





