// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_Tools.h"

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
#include <Events_Error.h>

#include <GeomAPI_Shape.h>

#include <QDir>
#include <QMessageBox>

#include <iostream>
#include <sstream>

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
    if (!aObj->data()->isValid())
      continue;
    aObjectNames << QString::fromStdString(aObj->data()->name());
  }
  return aObjectNames.join(", ");
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
bool canRemoveOrRename(QWidget* theParent, const QObjectPtrList& theObjects)
{
  bool aResult = true;
  QString aNotActivatedNames;
  if (!XGUI_Tools::allDocumentsActivated(aNotActivatedNames)) {
    DocumentPtr aModuleDoc = ModelAPI_Session::get()->moduleDocument();
    bool aFoundPartSetObject = false;
    foreach (ObjectPtr aObj, theObjects) {
      if (aObj->groupName() == ModelAPI_ResultPart::group())
        continue;
      aFoundPartSetObject = aObj->document() == aModuleDoc;
    }
    if (aFoundPartSetObject) {
      QMessageBox::StandardButton aRes = QMessageBox::warning(theParent, QObject::tr("Warning"),
               QObject::tr("Selected objects can be used in Part documents which are not loaded: \
%1. Whould you like to continue?").arg(aNotActivatedNames),
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
    if (ModelAPI_Tools::findVariable(theObject->document(), qPrintable(theName), aValue, aParam)) {
      QString aErrMsg(QObject::tr("Selected parameter can not be renamed to: %1. \
 There is a parameter with the same name. Its value is: %2.").arg(qPrintable(theName)).arg(aValue));
      // We can not use here a dialog box for message - it will crash editing process in ObjectBrowser
      Events_Error::send(aErrMsg.toStdString());
      return false;
    }
  }

  return true;
}

//******************************************************************
bool allDocumentsActivated(QString& theNotActivatedNames)
{
  bool anAllPartActivated = true;
  QStringList aRefNames;

  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  int aSize = aRootDoc->size(ModelAPI_ResultPart::group());
  for (int i = 0; i < aSize; i++) {
    ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), i);
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
    if (!aPart->isActivated()) {
      anAllPartActivated = false;
      aRefNames.append(aObject->data()->name().c_str());
    }
  }
  theNotActivatedNames = aRefNames.join(", ");
  return anAllPartActivated;
}

//**************************************************************
void refsToFeatureInFeatureDocument(const ObjectPtr& theObject, std::set<FeaturePtr>& theRefFeatures)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature.get()) {
    DocumentPtr aFeatureDoc = aFeature->document();
    // 1. find references in the current document
    aFeatureDoc->refsToFeature(aFeature, theRefFeatures, false);
  }
}

//**************************************************************
bool isSubOfComposite(const ObjectPtr& theObject, const FeaturePtr& theFeature)
{
  bool isSub = false;
  CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aComposite.get()) {
    isSub = aComposite->isSub(theObject);
    // the recursive is possible, the parameters are sketch circle and extrusion cut. They are
    // separated by composite sketch feature
    if (!isSub) {
      int aNbSubs = aComposite->numberOfSubs();
      for (int aSub = 0; aSub < aNbSubs && !isSub; aSub++) {
        isSub = isSubOfComposite(theObject, aComposite->subFeature(aSub));
      }
    }
  }
  return isSub;
}

//**************************************************************
bool isSubOfComposite(const ObjectPtr& theObject)
{
  bool isSub = false;
  std::set<FeaturePtr> aRefFeatures;
  refsToFeatureInFeatureDocument(theObject, aRefFeatures);
  std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                       aLast = aRefFeatures.end();
  for (; anIt != aLast && !isSub; anIt++) {
    isSub = isSubOfComposite(theObject, *anIt);
  }
  return isSub;
}

//**************************************************************
void refsToFeatureInAllDocuments(const ObjectPtr& theSourceObject, const ObjectPtr& theObject,
                                 const QObjectPtrList& theIgnoreList,
                                 std::set<FeaturePtr>& theDirectRefFeatures, 
                                 std::set<FeaturePtr>& theIndirectRefFeatures,
                                 std::set<FeaturePtr>& theAlreadyProcessed)
{
  refsDirectToFeatureInAllDocuments(theSourceObject, theObject, theIgnoreList, theDirectRefFeatures, 
                                    theAlreadyProcessed);

  // Run recursion. It is possible recursive dependency, like the following: plane, extrusion uses plane,
  // axis is built on extrusion. Delete of a plane should check the dependency from the axis also.
  std::set<FeaturePtr>::const_iterator aFeatureIt = theDirectRefFeatures.begin();
  for (; aFeatureIt != theDirectRefFeatures.end(); ++aFeatureIt) {
    std::set<FeaturePtr> aRecursiveRefFeatures;
    refsToFeatureInAllDocuments(theSourceObject, *aFeatureIt, theIgnoreList,
      aRecursiveRefFeatures, aRecursiveRefFeatures, theAlreadyProcessed);
    theIndirectRefFeatures.insert(aRecursiveRefFeatures.begin(), aRecursiveRefFeatures.end());
  }

}

//**************************************************************
void refsDirectToFeatureInAllDocuments(const ObjectPtr& theSourceObject, const ObjectPtr& theObject,
                                       const QObjectPtrList& theIgnoreList,
                                       std::set<FeaturePtr>& theDirectRefFeatures, 
                                       std::set<FeaturePtr>& theAlreadyProcessed)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (!aFeature.get())
    return;
  if (theAlreadyProcessed.find(aFeature) != theAlreadyProcessed.end())
    return;
  theAlreadyProcessed.insert(aFeature);

  //convert ignore object list to containt sub-features if the composite feature is in the list
  QObjectPtrList aFullIgnoreList;
  QObjectPtrList::const_iterator anIIt = theIgnoreList.begin(), anILast = theIgnoreList.end();
  for (; anIIt != anILast; anIIt++) {
    aFullIgnoreList.append(*anIIt);
    CompositeFeaturePtr aComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*anIIt);
    // if the current feature is aborted, the composite is removed and has invalid data
    if (aComposite.get() && aComposite->data()->isValid()) {
      int aNbSubs = aComposite->numberOfSubs();
      for (int aSub = 0; aSub < aNbSubs; aSub++) {
        aFullIgnoreList.append(aComposite->subFeature(aSub));
      }
    }
  }

  // 1. find references in the current document
  std::set<FeaturePtr> aRefFeatures;
  refsToFeatureInFeatureDocument(theObject, aRefFeatures);
  std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                       aLast = aRefFeatures.end();
  for (; anIt != aLast; anIt++) {
    // composite feature should not be deleted when the sub feature is to be deleted
    if (!isSubOfComposite(theSourceObject, *anIt) && !aFullIgnoreList.contains(*anIt))
      theDirectRefFeatures.insert(*anIt);
  }

  // 2. find references in all documents if the document of the feature is
  // "PartSet". Features of this document can be used in all other documents
  DocumentPtr aFeatureDoc = aFeature->document();

  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aModuleDoc = aMgr->moduleDocument();
  if (aFeatureDoc == aModuleDoc) {
    // the feature and results of the feature should be found in references
    std::list<ObjectPtr> aObjects;
    aObjects.push_back(aFeature);
    typedef std::list<std::shared_ptr<ModelAPI_Result> > ResultsList;
    const ResultsList& aResults = aFeature->results();
    ResultsList::const_iterator aRIter = aResults.begin();
    for (; aRIter != aResults.cend(); aRIter++) {
      ResultPtr aRes = *aRIter;
      if (aRes.get())
        aObjects.push_back(aRes);
    }
    // get all opened documents; found features in the documents;
    // get a list of objects where a feature refers;
    // search in these objects the deleted objects.
    SessionPtr aMgr = ModelAPI_Session::get();
    std::list<DocumentPtr> anOpenedDocs = aMgr->allOpenedDocuments();
    std::list<DocumentPtr>::const_iterator anIt = anOpenedDocs.begin(),
                                            aLast = anOpenedDocs.end();
    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    for (; anIt != aLast; anIt++) {
      DocumentPtr aDocument = *anIt;
      if (aDocument == aFeatureDoc)
        continue; // this document has been already processed in 1.1

      int aFeaturesCount = aDocument->size(ModelAPI_Feature::group());
      for (int aId = 0; aId < aFeaturesCount; aId++) {
        ObjectPtr anObject = aDocument->object(ModelAPI_Feature::group(), aId);
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
        if (!aFeature.get())
          continue;

        aRefs.clear();
        aFeature->data()->referencesToObjects(aRefs);
        std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRef = aRefs.begin();
        bool aHasReferenceToObject = false;
        for(; aRef != aRefs.end() && !aHasReferenceToObject; aRef++) {
          std::list<ObjectPtr>::iterator aRefObj = aRef->second.begin();
          for(; aRefObj != aRef->second.end() && !aHasReferenceToObject; aRefObj++) {
            std::list<ObjectPtr>::const_iterator aObjIt = aObjects.begin();
            for(; aObjIt != aObjects.end() && !aHasReferenceToObject; aObjIt++) {
              aHasReferenceToObject = *aObjIt == *aRefObj;
            }
          }
        }
        if (aHasReferenceToObject && !isSubOfComposite(theSourceObject, aFeature) &&
            !theIgnoreList.contains(aFeature))
          theDirectRefFeatures.insert(aFeature);
      }
    }
  }
}

}
