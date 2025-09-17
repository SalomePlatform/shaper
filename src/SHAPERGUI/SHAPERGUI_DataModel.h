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

#ifndef SHAPERGUI_DATAMODEL_H
#define SHAPERGUI_DATAMODEL_H

#include "SHAPER_SHAPERGUI.h"
#include <LightApp_DataModel.h>

class SHAPERGUI;

/**
* \ingroup Salome
* A Data Model class provides a connection of SALOME data structure and OpenParts application data model
*/
class SHAPERGUI_EXPORT SHAPERGUI_DataModel : public LightApp_DataModel
{
  Q_OBJECT
 public:
   /// Constructor
   /// \param theModule a module instance
  SHAPERGUI_DataModel(SHAPERGUI* theModule);
  virtual ~SHAPERGUI_DataModel();

  /// Open a data file
  /// \param thePath a path to the directory
  /// \param theStudy a current study
  /// \param theFiles a list of files to open
  virtual bool open(const QString& thePath, CAM_Study* theStudy, QStringList theFiles);

  /// Save module data to file
  /// \param theFiles list of created files
  virtual bool save(QStringList& theFiles);

  /// Save module data to a file
  /// \param thePath a path to the directory
  /// \param theStudy a current study
  /// \param theFiles a list of files to open
  virtual bool saveAs(const QString& thePath, CAM_Study* theStudy, QStringList& theFiles);

  /// Close data structure
  virtual bool close();

  /// Create data structure
  /// \param theStudy a current study
  virtual bool create(CAM_Study* theStudy);

  /// Returns True if the data structure has been modified
  virtual bool isModified() const;

  /// Returns True if the data structure is already saved
  virtual bool isSaved() const;

  /// Creates a module root object if it has not been created yet
  /// and append it to the active study. It is necessary for correct persistent
  /// of the model.
  virtual void initRootObject() override;

  /// Update data object
  /// \param theObj an data object
  /// \param theStudy a current study
  virtual void update(LightApp_DataObject* theObj = 0, LightApp_Study* theStudy = 0);

  /// Redefinition of virtual method: include the module dump in the common SALOME dump
  virtual bool dumpPython( const QString&, CAM_Study*, bool,  QStringList& );

protected:
  /**
   * Removes the directory with content if it exists
   * \param theDirectoryName a directory name
   */
  static void removeDirectory(const QString& theDirectoryName);

 private:
   /// Pat to a study file
  QString myStudyPath;

  /// a path to the temporary directory, created by opening a document file
  //QString myTmpDirectory;

  /// it should be created because the files reading is postponed in the module. The directory
  // should be removed after the model document is closed.
  SHAPERGUI* myModule;
};

#endif
