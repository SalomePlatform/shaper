

#ifndef SalomeModule_Module_H
#define SalomeModule_Module_H

#include <LightApp_Module.h>

class SalomeModule_Module: public LightApp_Module
{
  Q_OBJECT
public:
  SalomeModule_Module();
  virtual ~SalomeModule_Module();

  virtual void initialize( CAM_Application* theApp);
  virtual void windows( QMap<int, int>& theWndMap) const;
  virtual void viewManagers( QStringList& theList) const;

public slots:
  bool activateModule( SUIT_Study* theStudy);
  bool deactivateModule( SUIT_Study* theStudy);

private:

};

#endif
