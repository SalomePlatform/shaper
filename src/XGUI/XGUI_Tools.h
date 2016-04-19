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
class XGUI_Workshop;
class ModuleBase_IWorkshop;

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

// The model concerning tools

/*! Unite object names in one string using the separator between values
 \param theObjects a list of objects
 \param theSeparator a separator
 */
QString unionOfObjectNames(const QObjectPtrList& theObjects, const QString& theSeparator);

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
 \param aList a list of object
 \return a boolean value
 */
bool XGUI_EXPORT canRemoveOrRename(QWidget* theParent, const QObjectPtrList& aList);

/*! 
 Check possibility to rename object
 \param theObject an object to rename
 \param theName a name
 */
bool canRename(const ObjectPtr& theObject, const QString& theName);

/*!
 Returns true if there are no parts in the document, which are not activated
 \param theNotActivatedNames out string which contains not activated names
 \return a boolean value
 */
bool XGUI_EXPORT allDocumentsActivated(QString& theNotActivatedNames);

/*!
 Returns converted workshop
 \param theWorkshop an interface workshop
 \return XGUI workshop instance
*/
XGUI_EXPORT XGUI_Workshop* workshop(ModuleBase_IWorkshop* theWorkshop);

};

#endif
