// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef XGUI_Tools_H
#define XGUI_Tools_H

#include "XGUI.h"
#include <QString>
#include <QRect>

#include <ModelAPI_Feature.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModuleBase_Definitions.h>

#include <memory>

class QWidget;
class XGUI_Workshop;
class ModuleBase_IWorkshop;

/*!
 \ingroup GUI
 \brief Commonly used methods in XGUI package and higher.
 */
namespace XGUI_Tools {

/**
 \brief Return directory part of the file path.

 If the file path does not include directory part (the file is in the
 current directory), null string is returned.

 \param path file path
 \param isAbs if true (default) \a path parameter is treated as absolute file path
 \return directory part of the file path
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
 \param theFeatures a list of checked features
 \return a boolean value
 */
bool XGUI_EXPORT canRemoveOrRename(QWidget* theParent, const std::set<FeaturePtr>& theFeatures);

/*! 
 Check possibility to rename object
 \param theObject an object to rename
 \param theName a name
 */
bool canRename(const ObjectPtr& theObject, const QString& theName);

/*!
 Checks that the given string contains only ASCII symbols
 \param theStr a string to check
 */
bool isAscii(const QString& theStr);

/*!
 Returns converted workshop
 \param theWorkshop an interface workshop
 \return XGUI workshop instance
*/
XGUI_EXPORT XGUI_Workshop* workshop(ModuleBase_IWorkshop* theWorkshop);


/// Generates a presentation name in form: <object_name>/<face>_<face_index>
/// \param thePrs a presentation
/// \return string value
XGUI_EXPORT QString generateName(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

/// Creates and returns the temporary directory (with random name) using the environment variable
/// path to location of such directories,
std::string getTmpDirByEnv( const char* thePathEnv);

/// Removes files and directory where they are located
void removeTemporaryFiles(const std::string& theDirectory,
  const std::list<std::string>& theFiles);
};

#endif
