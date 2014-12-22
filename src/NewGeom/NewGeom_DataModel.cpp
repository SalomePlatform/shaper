// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "NewGeom_DataModel.h"
#include "NewGeom_Module.h"

#include <XGUI_Workshop.h>

#include <ModelAPI_Session.h>

#include <LightApp_Study.h>
#include <CAM_Application.h>
#include <SUIT_Tools.h>
#include <SUIT_ResourceMgr.h>

#include <QFile>
#include <QDir>

NewGeom_DataModel::NewGeom_DataModel(NewGeom_Module* theModule)
    : LightApp_DataModel(theModule), myStudyPath(""), myModule(theModule)
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

  myStudyPath = thePath;

  // If the file is Multi(contain all module files inside), the open SALOME functionality creates
  // these files in a temporary directory. After the open functionality is finished, it removes
  // these files (in the full SALOME mode).
  // The postponed loading of the files is realized in the NewGEOM module. So, it is important do
  // not remove the opened files.
  // The following code creates a new tmp directory with a copy of files.
  QString aTmpDir = theFiles.first();

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( myModule->application()->activeStudy() );
  QString aNewTmpDir = aStudy->GetTmpDir("", false).c_str();

  bool isDone = true;
  QDir aDir(aTmpDir);
  QStringList aFiles = aDir.entryList(QDir::Files);
  QStringList::const_iterator anIt = aFiles.begin(), aLast = aFiles.end();
  for (; anIt != aLast; anIt++) {
    QString aFileName = *anIt;

    QString aCurrentFile = SUIT_Tools::addSlash(aTmpDir) + aFileName;
    QString aNewFile = SUIT_Tools::addSlash(aNewTmpDir) + aFileName;
    if (!QFile::copy(aCurrentFile, aNewFile))
      isDone = false;
  }
  if (isDone) {
    myTmpDirectory = aNewTmpDir;
  }
  else {
    removeDirectory(aNewTmpDir);
    myTmpDirectory = "";
  }

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->load(qPrintable(aNewTmpDir));
  myModule->setIsOpened(true);
  return true;
}

bool NewGeom_DataModel::save(QStringList& theFiles)
{
  LightApp_DataModel::save( theFiles );
  XGUI_Workshop* aWorkShop = myModule->workshop();
  std::list<std::string> aFileNames;

  CAM_Application* anApp = myModule->application();
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(anApp->activeStudy());
  SUIT_ResourceMgr* aResMgr = anApp->resourceMgr();

  // it is important to check whether the file is saved in the multi-files mode in order to save
  // files in temporary directories, which are removed in the full SALOME mode after copiying
  // the files content in a result file.
  bool isMultiFile = aResMgr ? aResMgr->booleanValue("Study", "multi_file", false) : false;

  std::string aTmpDir = aStudy->GetTmpDir(qPrintable(myStudyPath), isMultiFile);
  //std::string aTmpDir = aStudy->GetTmpDir("", false);//true );
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
  removeDirectory(myTmpDirectory);
  myTmpDirectory = "";
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

void NewGeom_DataModel::removeDirectory(const QString& theDirectoryName)
{
  Qtx::rmDir(theDirectoryName);
}

