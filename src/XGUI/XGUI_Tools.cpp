// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "XGUI_Tools.h"

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"

#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_Tools.h"

#include <TopoDS_Shape.hxx>
#include <ModelAPI_Object.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <Events_InfoMessage.h>

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <TopoDS_Shape.hxx>

#include <QDir>
#include <QMessageBox>

#include <iostream>
#include <sstream>
#include <string>

#ifndef WIN32
# include <sys/stat.h>
# include <dirent.h>
# include <unistd.h>
# define _separator_ '/'
#else
#include <io.h>
#define F_OK 0
#define access _access
# include <windows.h>
# define _separator_ '\\'
#endif

namespace XGUI_Tools {
//******************************************************************
QString dir(const QString& path, bool isAbs)
{
  QDir aDir = QFileInfo(path).dir();
  QString dirPath = isAbs ? aDir.absolutePath() : aDir.path();
  if (dirPath == QString("."))
    dirPath = QString();
  return dirPath;
}

//******************************************************************
QString file(const QString& path, bool withExt)
{
  QString fPath = path;
  while (!fPath.isEmpty() && (fPath[fPath.length() - 1] == '\\' ||
          fPath[fPath.length() - 1] == '/'))
    fPath.remove(fPath.length() - 1, 1);

  if (withExt)
    return QFileInfo(fPath).fileName();
  else
    return QFileInfo(fPath).completeBaseName();
}

//******************************************************************
QString addSlash(const QString& path)
{
  QString res = path;
  if (!res.isEmpty() && res.at(res.length() - 1) != QChar('/')
      && res.at(res.length() - 1) != QChar('\\'))
    res += QDir::separator();
  return res;
}

//******************************************************************
QString unionOfObjectNames(const QObjectPtrList& theObjects, const QString& theSeparator)
{
  QStringList aObjectNames;
  foreach (ObjectPtr aObj, theObjects) {
    if (aObj->data()->isValid())
      aObjectNames << QString::fromStdWString(aObj->data()->name());
  }
  if (aObjectNames.count() == 0)
    return QString();
  if (aObjectNames.count() == 1)
    return aObjectNames.first();
  return aObjectNames.join(theSeparator);
}

//******************************************************************
bool isModelObject(FeaturePtr theFeature)
{
  return theFeature && !theFeature->data();
}

//******************************************************************
std::string featureInfo(FeaturePtr theFeature)
{
  std::ostringstream aStream;
  if (theFeature)
    aStream << theFeature.get() << " " << theFeature->getKind();
  return QString(aStream.str().c_str()).toStdString();
}

//******************************************************************
/*FeaturePtr realFeature(const FeaturePtr theFeature)
 {
 if (theFeature->data()) {
 return theFeature;
 } else {
 ObjectPtr aObject = std::dynamic_pointer_cast<ModelAPI_Object>(theFeature);
 return aObject->featureRef();
 }
 }*/


//******************************************************************
bool canRemoveOrRename(QWidget* theParent, const std::set<FeaturePtr>& theFeatures)
{
  bool aResult = true;
  std::wstring aNotActivatedNames;
  if (!ModelAPI_Tools::allDocumentsActivated(aNotActivatedNames)) {
    bool aFoundPartSetObject = ModuleBase_Tools::hasModuleDocumentFeature(theFeatures);
    if (aFoundPartSetObject) {
      const char* aKeyStr = "Selected objects can be used in Part documents which are not loaded: "
                            "%1. Whould you like to continue?";
      QMessageBox::StandardButton aRes = QMessageBox::warning(theParent, QObject::tr("Warning"),
               QObject::tr(aKeyStr).arg(QString::fromStdWString(aNotActivatedNames)),
               QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
      aResult = aRes == QMessageBox::Yes;
    }
  }
  return aResult;
}

//******************************************************************
bool isAscii(const QString& theStr)
{
  char aCh;
  for (int i = 0; i < theStr.size(); i++) {
    aCh = theStr[i].toLatin1();
    if (aCh == 0)
      return false;
    if ((aCh >= 0x30) && (aCh <= 0x39))
      continue;
    else if ((aCh >= 0x41) && (aCh <= 0x5A))
      continue;
    else if ((aCh >= 0x61) && (aCh <= 0x7A))
      continue;
    else if (aCh == 0x5f)
      continue;
    else
      return false;
  }
  return true;
}

//******************************************************************
bool canRename(const ObjectPtr& theObject, const QString& theName)
{
  std::string aType = theObject->groupName();
  if (aType == ModelAPI_ResultParameter::group()) {
    // For parameters names only ASCII symbols have to be used
    if (!isAscii(theName))
      return false;

    double aValue;
    ResultParameterPtr aParam;
    if (ModelAPI_Tools::findVariable(theObject->document(),
          FeaturePtr(), theName.toStdWString(), aValue, aParam)) {
      const char* aKeyStr = "Selected parameter can not be renamed to: %1. "
                            "There is a parameter with the same name. Its value is: %2.";
      QString aErrMsg(QObject::tr(aKeyStr).arg(theName).arg(aValue));
      // We can not use here a dialog box for message -
      // it will crash editing process in ObjectBrowser
      Events_InfoMessage("XGUI_Tools", aErrMsg.toStdString()).send();
      return false;
    }
  }
  else {
    DocumentPtr aDoc = theObject->document();
    ObjectPtr aObj =
      aDoc->objectByName(aType, theName.toStdWString());

    if (aObj.get() && theObject != aObj) {
      QString aErrMsg(QObject::tr("Name %2 already exists in %1.").
        arg(aType.c_str()).arg(theName));
      // We can not use here a dialog box for message -
      // it will crash editing process in ObjectBrowser
      Events_InfoMessage("XGUI_Tools", aErrMsg.toStdString()).send();
      return false;
    }
  }

  return true;
}

//**************************************************************

XGUI_Workshop* workshop(ModuleBase_IWorkshop* theWorkshop)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  return aConnector ? aConnector->workshop() : 0;
}


//********************************************************************
QString generateName(const ModuleBase_ViewerPrsPtr& thePrs)
{
  if (!thePrs.get() || !thePrs->object().get())
    return "Undefined";

  GeomShapePtr aContext;
  ObjectPtr anObject = thePrs->object();
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  if (aResult.get())
    aContext = aResult->shape();
  else {
    // TODO if there is this case
  }

  QString aName = QString::fromStdWString(anObject->data()->name());
  if (aContext.get()) {
    GeomShapePtr aSubShape(new GeomAPI_Shape());
    TopoDS_Shape aShape = ModuleBase_Tools::getSelectedShape(thePrs);
    if (!aShape.IsNull()) {
      aSubShape->setImpl(new TopoDS_Shape(aShape));
      if (!aSubShape->isEqual(aContext)) {
        QString aTypeName;
        switch (aShape.ShapeType()) {
        case TopAbs_COMPOUND:
          aTypeName = "compound";
          break;
        case TopAbs_COMPSOLID:
          aTypeName = "compsolid";
          break;
        case TopAbs_SOLID:
          aTypeName = "solid";
          break;
        case TopAbs_SHELL:
          aTypeName = "shell";
          break;
        case TopAbs_FACE:
          aTypeName = "face";
          break;
        case TopAbs_WIRE:
          aTypeName = "wire";
          break;
        case TopAbs_EDGE:
          aTypeName = "edge";
          break;
        case TopAbs_VERTEX:
          aTypeName = "vertex";
          break;
        case TopAbs_SHAPE:
          aTypeName = "shape";
          break;
        }
        int aId = GeomAlgoAPI_CompoundBuilder::id(aContext, aSubShape);
        aName += QString("/%1_%2").arg(aTypeName).arg(aId);
      }
    }
  }
  return aName;
}

std::wstring strToWStr(const std::string& theStr) {
  size_t aLen = theStr.size();
  std::wstring aResult(aLen, L'#');
  mbstowcs(&aResult[0], theStr.c_str(), aLen);
  return aResult;
}

std::string getTmpDirByPath( const std::string& theTmpPath)
{
  std::string aTmpDir = theTmpPath;
  if (aTmpDir == "" || access(aTmpDir.c_str() , F_OK) != 0) {
#ifdef WIN32
    char *Tmp_dir = getenv("TEMP");
    if (Tmp_dir == NULL) {
      Tmp_dir = getenv("TMP");
      if (Tmp_dir == NULL)
        aTmpDir = "C:\\";
      else
        aTmpDir = Tmp_dir;
    }
    else
      aTmpDir = Tmp_dir;
#else
    aTmpDir = "/tmp/";
#endif
  }

  if (aTmpDir.back() != _separator_)
    aTmpDir += _separator_;

  srand((unsigned int)time( NULL ));
  //Get a random number to present a name of a sub directory
  int aRND = 999 + (int)(100000.0*rand()/(RAND_MAX+1.0));
  char buffer[127];
  sprintf(buffer, "%d", aRND);
  std::string aSubDir(buffer);
  if (aSubDir.size() <= 1)
    aSubDir = "123049876";

  aTmpDir += aSubDir; //Get RND sub directory

  std::string aDir = aTmpDir;

  for(aRND = 0; access(aDir.c_str() , F_OK) == 0; aRND++) {
    sprintf( buffer, "%d", aRND );
    aDir = aTmpDir + buffer;  //Build a unique directory name
  }
  if (aDir.back() != _separator_)
    aDir += _separator_;

#ifdef WIN32
  CreateDirectory(strToWStr(aDir).c_str(), NULL);
#else
  mkdir( aDir.c_str(), 0x1ff );
#endif

  return aDir;
}

std::string getTmpDirByEnv( const char* thePathEnv)
{
  char* aVal = thePathEnv[0] == 0 ? 0 : getenv(thePathEnv);
  std::string dir = aVal ? aVal : "";
  return getTmpDirByPath(dir).c_str();
}

void removeTemporaryFiles(const std::string& theDirectory,
  const std::list<std::string>& theFiles)
{
  std::string aDirName = theDirectory;

  std::list<std::string>::const_iterator aFilesIter = theFiles.cbegin();
  for(; aFilesIter != theFiles.cend(); aFilesIter++) {
    const std::string& aFile = *aFilesIter;
    if(access(aFile.c_str() , F_OK) != 0)
      continue;

#ifdef WIN32
    DeleteFile(strToWStr(aFile).c_str());
#else
    unlink(aFile.c_str());
#endif
  }

  if(access(aDirName.c_str() , F_OK) == 0) {
#ifdef WIN32
    RemoveDirectory(strToWStr(aDirName).c_str());
#else
    rmdir(aDirName.c_str());
#endif
  }
}

}
