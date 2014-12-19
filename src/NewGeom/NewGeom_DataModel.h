// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#ifndef NEWGEOM_DATAMODEL_H
#define NEWGEOM_DATAMODEL_H

#include "NewGeom.h"
#include <LightApp_DataModel.h>

class NewGeom_Module;

class NewGeom_EXPORT NewGeom_DataModel : public LightApp_DataModel
{
  Q_OBJECT
 public:
  NewGeom_DataModel(NewGeom_Module* theModule);
  virtual ~NewGeom_DataModel();

  virtual bool open(const QString& thePath, CAM_Study* theStudy, QStringList theFiles);
  virtual bool save(QStringList& theFiles);
  virtual bool saveAs(const QString& thePath, CAM_Study* theStudy, QStringList& theFiles);
  virtual bool close();
  virtual bool create(CAM_Study* theStudy);

  virtual bool isModified() const;
  virtual bool isSaved() const;

  virtual void update(LightApp_DataObject* theObj = 0, LightApp_Study* theStudy = 0);

protected:
  /**
   * Removes the directory with content if it exists
   * \param theDirectoryName a directory name
   */
  static void removeDirectory(const QString& theDirectoryName);

 private:
  QString myStudyPath;
  QString myTmpDirectory; /// a path to the temporary directory, created by opening a document file
  /// it should be created because the files reading is postponed in the module. The directory
  // should be removed after the model document is closed.
  NewGeom_Module* myModule;
};

#endif
