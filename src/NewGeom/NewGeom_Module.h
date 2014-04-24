

#ifndef NewGeom_Module_H
#define NewGeom_Module_H

#include "NewGeom.h"
#include <LightApp_Module.h>

class NewGeom_EXPORT NewGeom_Module: public LightApp_Module
{
  Q_OBJECT
public:
  NewGeom_Module();
  virtual ~NewGeom_Module();

  virtual void initialize( CAM_Application* theApp);
  virtual void windows( QMap<int, int>& theWndMap) const;
  virtual void viewManagers( QStringList& theList) const;

public slots:
  bool activateModule( SUIT_Study* theStudy);
  bool deactivateModule( SUIT_Study* theStudy);

protected:
  CAM_DataModel* createDataModel();


private:

};

#endif
