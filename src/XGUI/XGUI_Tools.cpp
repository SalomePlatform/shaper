// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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
#include <Events_InfoMessage.h>

#include <GeomAPI_Shape.h>

#include <QDir>
#include <QMessageBox>

#include <iostream>
#include <sstream>
#include <string>

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
  while (!fPath.isEmpty() && (fPath[fPath.length() - 1] == '\\' || fPath[fPath.length() - 1] == '/'))
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
      aObjectNames << QString::fromStdString(aObj->data()->name());
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
  std::string aNotActivatedNames;
  if (!ModelAPI_Tools::allDocumentsActivated(aNotActivatedNames)) {
    bool aFoundPartSetObject = ModuleBase_Tools::hasModuleDocumentFeature(theFeatures);
    if (aFoundPartSetObject) {
      QMessageBox::StandardButton aRes = QMessageBox::warning(theParent, QObject::tr("Warning"),
               QObject::tr("Selected objects can be used in Part documents which are not loaded: \
%1. Whould you like to continue?").arg(aNotActivatedNames.c_str()),
               QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
      aResult = aRes == QMessageBox::Yes;
    }
  }
  return aResult;
}

//******************************************************************
bool canRename(const ObjectPtr& theObject, const QString& theName)
{
  if (std::dynamic_pointer_cast<ModelAPI_ResultParameter>(theObject).get()) {
    double aValue;
    ResultParameterPtr aParam;
    if (ModelAPI_Tools::findVariable(theObject->document(), 
          FeaturePtr(), qPrintable(theName), aValue, aParam)) {
      QString aErrMsg(QObject::tr("Selected parameter can not be renamed to: %1. \
 There is a parameter with the same name. Its value is: %2.").arg(qPrintable(theName)).arg(aValue));
      // We can not use here a dialog box for message - it will crash editing process in ObjectBrowser
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
  return aConnector->workshop();
}

}
