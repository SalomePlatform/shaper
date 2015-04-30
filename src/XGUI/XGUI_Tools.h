// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_Tools_H
#define XGUI_Tools_H

#include "XGUI.h"
#include <QString>
#include <QRect>

#include <ModelAPI_Feature.h>

#include <memory>

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


};

#endif
