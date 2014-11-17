#include "NewGeom_DataModel.h"
#include "NewGeom_Module.h"

#include <XGUI_Workshop.h>

#include <ModelAPI_Session.h>

#include <LightApp_Study.h>
#include <CAM_Application.h>
#include <SUIT_Tools.h>

#include <QFile>


NewGeom_DataModel::NewGeom_DataModel(NewGeom_Module* theModule)
    : LightApp_DataModel(theModule), myModule(theModule)
{
}

NewGeom_DataModel::~NewGeom_DataModel()
{
}

bool NewGeom_DataModel::open(const QString& thePath, CAM_Study* theStudy, QStringList theFiles)
{
  LightApp_DataModel::open( thePath, theStudy, theFiles );
  if (theFiles.size() == 0)
    return false;

  QString aFile = theFiles.first();

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->load(qPrintable(aFile));
  myModule->setIsOpened(true);
  myStudyPath = aFile;
  return true;
}

bool NewGeom_DataModel::save(QStringList& theFiles)
{
  LightApp_DataModel::save( theFiles );
  XGUI_Workshop* aWorkShop = myModule->workshop();
  std::list<std::string> aFileNames;

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( myModule->application()->activeStudy() );
  std::string aTmpDir = aStudy->GetTmpDir(qPrintable(myStudyPath), true );
  theFiles.append(QString(aTmpDir.c_str()));

  aWorkShop->saveDocument(QString(aTmpDir.c_str()), aFileNames);
  std::list<std::string>::iterator aIt;
  for (aIt = aFileNames.begin(); aIt != aFileNames.end(); ++aIt) {
    QString aName((*aIt).c_str());
    aName.replace(QChar('\\'), QChar('/'));
    int aN = aName.lastIndexOf('/');
    theFiles.append(aName.right(aName.length() - aN - 1));
  }
  return true;
}

bool NewGeom_DataModel::saveAs(const QString& thePath, CAM_Study* theStudy, QStringList& theFiles)
{
  myStudyPath = thePath;
  return save(theFiles);
}

bool NewGeom_DataModel::close()
{
  myModule->workshop()->closeDocument();
  return LightApp_DataModel::close();
}

bool NewGeom_DataModel::create(CAM_Study* theStudy)
{
  return true;
}

bool NewGeom_DataModel::isModified() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  return aMgr->isModified();
}

bool NewGeom_DataModel::isSaved() const
{
  return !isModified();
}

void NewGeom_DataModel::update(LightApp_DataObject* theObj, LightApp_Study* theStudy)
{
}
