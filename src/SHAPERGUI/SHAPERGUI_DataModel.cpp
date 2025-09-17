// Copyright (C) 2014-2025  CEA, EDF
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

#include "SHAPERGUI_DataModel.h"
#include "SHAPERGUI.h"

#include <XGUI_Workshop.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ExchangePlugin_Dump.h>

#include <LightApp_Study.h>
#include <CAM_Application.h>
#include <CAM_DataObject.h>
#include <SUIT_Tools.h>
#include <SUIT_ResourceMgr.h>

#include <QFile>
#include <QDir>
#include <QTextStream>


#define DUMP_NAME "shaper_dump.py"


SHAPERGUI_DataModel::SHAPERGUI_DataModel(SHAPERGUI* theModule)
    : LightApp_DataModel(theModule), myStudyPath(""), myModule(theModule)
{
}

SHAPERGUI_DataModel::~SHAPERGUI_DataModel()
{
}

bool SHAPERGUI_DataModel::open(const QString& thePath, CAM_Study* theStudy, QStringList theFiles)
{
  LightApp_DataModel::open( thePath, theStudy, theFiles );
  if (theFiles.size() == 0)
    return false;

  myStudyPath = thePath;

  // If the file is Multi(contain all module files inside), the open SALOME functionality creates
  // these files in a temporary directory. After the open functionality is finished, it removes
  // these files (in the full SALOME mode).
  // The postponed loading of the files is realized in the SHAPER module. So, it is important do
  // not remove the opened files.
  // The following code creates a new tmp directory with a copy of files.
  QString aTmpDir = theFiles.first();

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( myModule->application()->activeStudy() );
  QString aNewTmpDir = aStudy->GetTmpDir("", false).c_str();

  QDir aDir(aTmpDir);
  QStringList aFiles = aDir.entryList(QDir::Files);
  QStringList::const_iterator anIt = aFiles.begin(), aLast = aFiles.end();
  for (; anIt != aLast; anIt++) {
    QString aFileName = *anIt;

    QString aCurrentFile = SUIT_Tools::addSlash(aTmpDir) + aFileName;
    XGUI_Workshop* aWorkShop = myModule->workshop();
    aWorkShop->openFile(aCurrentFile);
  }

  myModule->setIsOpened(true);
  return true;
}

bool SHAPERGUI_DataModel::save(QStringList& theFiles)
{
  // Publish to study before saving of the data model
  myModule->publishToStudy();

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
  QString aTmp = QString(aTmpDir.c_str());
  theFiles.append(aTmp);

  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isAutoUpdateBlocked())
    aMgr->blockAutoUpdate(false);

  //aWorkShop->saveDocument(QString(aTmpDir.c_str()), aFileNames);
  aWorkShop->setCurrentDataFile(aTmp + "shaper.shaper");
  aWorkShop->onSave();
  QString aName = aWorkShop->currentDataFile();
  aName.replace(QChar('\\'), QChar('/'));
  int aN = aName.lastIndexOf('/');
  theFiles.append(aName.right(aName.length() - aN - 1));

  return true;
}

bool SHAPERGUI_DataModel::saveAs(const QString& thePath, CAM_Study* theStudy, QStringList& theFiles)
{
  myStudyPath = thePath;
  return save(theFiles);
}

bool SHAPERGUI_DataModel::close()
{
  myModule->workshop()->closeDocument();
  return LightApp_DataModel::close();
}

bool SHAPERGUI_DataModel::create(CAM_Study* theStudy)
{
  return true;
}

bool SHAPERGUI_DataModel::isModified() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  return aMgr->isModified();
}

bool SHAPERGUI_DataModel::isSaved() const
{
  return !isModified();
}

void SHAPERGUI_DataModel::update(LightApp_DataObject* theObj, LightApp_Study* theStudy)
{
  // Nothing to do here: we always keep the data tree in the up-to-date state
  // The only goal of this method is to hide default behavior from LightApp_DataModel
  return;
}

void SHAPERGUI_DataModel::initRootObject()
{
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( module()->application()->activeStudy() );
  CAM_ModuleObject *aModelRoot = dynamic_cast<CAM_ModuleObject*>(root());
  if(study && aModelRoot == NULL) {
    aModelRoot = createModuleObject( study->root() );
    aModelRoot->setDataModel( this );
    setRoot(aModelRoot);
  }
}

void SHAPERGUI_DataModel::removeDirectory(const QString& theDirectoryName)
{
  Qtx::rmDir(theDirectoryName);
}

bool SHAPERGUI_DataModel::dumpPython(const QString& thePath, CAM_Study* theStudy,
                                     bool isMultiFile,  QStringList& theListOfFiles)
{
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(theStudy);
  if (!aStudy)
    return false;

  myModule->publishToStudy();

  std::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_Session::get()->activeDocument();
  ModelAPI_Session::get()->startOperation(ExchangePlugin_Dump::ID());
  FeaturePtr aFeature = aDoc->addFeature(ExchangePlugin_Dump::ID());
  if (aFeature.get()) {
    // keep path to the true dumping directory for external files dumping
    AttributeStringPtr aAttr = aFeature->string(ExchangePlugin_Dump::DUMP_DIR_ID());
    if (aAttr.get()) {
      QString aDirPath = QFileInfo(thePath).path();
      aAttr->setValue(aDirPath.toStdString());
    }

    // tmp path to write the script
    std::string aTmpDir = aStudy->GetTmpDir(thePath.toStdString().c_str(), isMultiFile);
    std::string aFileName = aTmpDir + DUMP_NAME;

    if (QFile::exists(aFileName.c_str())) {
      QFile::remove(aFileName.c_str());
    }

    aAttr = aFeature->string(ExchangePlugin_Dump::FILE_PATH_ID());
    if (aAttr.get())
      aAttr->setValue(aFileName);

    aAttr = aFeature->string(ExchangePlugin_Dump::FILE_FORMAT_ID());
    if (aAttr.get())
      aAttr->setValue(".py");

    AttributeBooleanPtr aAttrBool = aFeature->boolean(ExchangePlugin_Dump::MULTIFILE_ID());
    if(aAttrBool.get())
      aAttrBool->setValue(isMultiFile);

#ifdef HAVE_SALOME
    aFeature->boolean(ExchangePlugin_Dump::EXPORT_VARIABLES_ID())->setValue(true);
#endif

    ModelAPI_Session::get()->finishOperation();

    if (QFile::exists(aFileName.c_str())) {
      QFile aInFile(aFileName.c_str());
      if (!aInFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
      QTextStream aText(&aInFile);
      QString aTrace(aText.readAll());
      aInFile.close();

      if (isMultiFile) {
        QStringList aBuffer;
        aBuffer.push_back(QString("def RebuildData():"));
        QStringList aList(aTrace.split("\n"));
        foreach(QString aStr, aList) {
          QString s = "  " + aStr;
          aBuffer.push_back(s);
        }
        aTrace = aBuffer.join("\n");
      }

      QFile aOutFile(aFileName.c_str());
      // binary for SALOME, issue 16931
      if (!aOutFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

      QTextStream aOut(&aOutFile);
      aOut << aTrace << "\n";
      aOut.flush();
      aOutFile.close();

      theListOfFiles.append(aTmpDir.c_str());
      theListOfFiles.append(DUMP_NAME);
      return true;
    }
  }
  return false;
}
