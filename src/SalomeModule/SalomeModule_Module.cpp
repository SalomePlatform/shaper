

#include "SalomeModule_Module.h"

#include <LightApp_Application.h>
#include <OCCViewer_ViewModel.h>



SalomeModule_Module::SalomeModule_Module()
: LightApp_Module( "NewGeom" )
{
}


SalomeModule_Module::~SalomeModule_Module()
{
}

void SalomeModule_Module::initialize(CAM_Application* theApp)
{
}

void SalomeModule_Module::windows(QMap<int, int>& theWndMap) const
{
  theWndMap.insert( LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
}

void SalomeModule_Module::viewManagers(QStringList& theList) const
{
  theList.append( OCCViewer_Viewer::Type() );
}

bool SalomeModule_Module::activateModule(SUIT_Study* theStudy)
{
  return LightApp_Module::activateModule(theStudy);
}

bool SalomeModule_Module::deactivateModule(SUIT_Study* theStudy)
{
  return LightApp_Module::deactivateModule(theStudy);
}


