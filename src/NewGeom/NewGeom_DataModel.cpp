
#include "NewGeom_DataModel.h"


NewGeom_DataModel::NewGeom_DataModel( CAM_Module* theModule )
  : LightApp_DataModel(theModule)
{
}
  
NewGeom_DataModel::~NewGeom_DataModel()
{
}

bool NewGeom_DataModel::open( const QString& thePath, CAM_Study* theStudy, QStringList theFiles )
{
  return true;
}

bool NewGeom_DataModel::save( QStringList& theFiles )
{
  return true;
}

bool NewGeom_DataModel::saveAs( const QString& thePath, CAM_Study* theStudy, QStringList& theFiles )
{
  myStudyPath = thePath;
  return save( theFiles );
}

bool NewGeom_DataModel::close()
{
  return LightApp_DataModel::close();
}

bool NewGeom_DataModel::create( CAM_Study* theStudy )
{
  return true;
}

bool NewGeom_DataModel::isModified() const
{
  return false;
}

bool NewGeom_DataModel::isSaved() const
{
  return true;
}

void NewGeom_DataModel::update( LightApp_DataObject* theObj, LightApp_Study* theStudy )
{
}
