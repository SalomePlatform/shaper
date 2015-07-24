// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_Tools_H
#define XGUI_Tools_H

#include "XGUI.h"
#include <QString>
#include <QRect>

#include <ModelAPI_Feature.h>

#include <ModuleBase_Definitions.h>

#include <memory>

class QWidget;

/*!
 \ingroup GUI
 \brief Return directory part of the file path.

 If the file path does not include directory part (the file is in the
 current directory), null string is returned.

 \param path file path
 \param abs if true (default) \a path parameter is treated as absolute file path
 \return directory part of the file path
 */
namespace XGUI_Tools {

/**
* Returns directory name from name of file
* \param path a path to a file
* \param isAbs is absolute or relative path
*/
QString XGUI_EXPORT dir(const QString& path, bool isAbs = true);

/*!
 \brief Return file name part of the file path.

 \param path file path
 \param withExt if true (default) complete file name (with all
 extension except the last) is returned, otherwise only base name
 is returned
 \return file name part of the file path
 */
QString XGUI_EXPORT file(const QString& path, bool withExt = true);

/*!
 \brief Add a slash (platform-specific) to the end of \a path
 if it is not already there.
 \param path directory path
 \return modified path (with slash added to the end)
 */
QString XGUI_EXPORT addSlash(const QString& path);

/// The model concerning tools

/*!
 Returns true if the feature is a model object
 \param theFeature a feature
 */
bool XGUI_EXPORT isModelObject(FeaturePtr theFeature);

/*!
 Returns the string presentation of the given feature
 \param theFeature a feature
 */
std::string XGUI_EXPORT featureInfo(FeaturePtr theFeature);

/*!
 Returns true if there are no parts in the document, which are not activated or
 all objects in the list are not PartSet document.
 It shows the warning control if the result is false.
 \param theParent a parent for the warning control
 \param theList a list of object
 \return a boolean value
 */
bool XGUI_EXPORT canRemoveOrRename(QWidget* theParent, const QObjectPtrList& aList);

/*! 
 Returns true if theObject can be renamed in theName
 */
bool canRename(QWidget* theParent, const ObjectPtr& theObject, const QString& theName);

/*!
 Returns true if there are no parts in the document, which are not activated
 \return a boolean value
 */
bool XGUI_EXPORT allDocumentsActivated(QString& theNotActivatedNames);

/*!
  Returns a container of referenced feature to the current object in the object document.
  \param theObject an object, which will be casted to a feature type
  \param theRefFeatures an output container
 */
void XGUI_EXPORT refsToFeatureInFeatureDocument(const ObjectPtr& theObject,
                                                std::set<FeaturePtr>& theRefFeatures);

/*!
 Returns true if the object if a sub child of the feature. The feature is casted to the
 composite one. If it is possible, the sub object check happens. The method is applyed
 recursively to the feature subs.
 \param theObject a candidate to be a sub object
 \param theFeature a candidate to be a composite feature
 \return a boolean value
 */
bool XGUI_EXPORT isSubOfComposite(const ObjectPtr& theObject, const FeaturePtr& theFeature);

/*!
 Returns a container of references feature to the source object. The search happens in the object
 document and in other Part documents if the object belongs to the PartSet. The search is recursive,
 in other words it is applyed to set of the found objects until it is possible.
 It do not returns the referenced features to the object if this references is a composite feature
 which has the object as a sub object.
 \param theSourceObject an object, which references are searched
 \param theObject an intermediate recursive object, should be set in the source object
 \return a boolean value
 */
void XGUI_EXPORT refsToFeatureInAllDocuments(const ObjectPtr& theSourceObject,
                                             const ObjectPtr& theObject,
                                             std::set<FeaturePtr>& theDirectRefFeatures,
                                             std::set<FeaturePtr>& theIndirectRefFeatures);
};

#endif
