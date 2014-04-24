

#include "NewGeom_Module.h"

#include <LightApp_Application.h>
#include <OCCViewer_ViewModel.h>



NewGeom_Module::NewGeom_Module()
: LightApp_Module( "NewGeom" )
{
}


NewGeom_Module::~NewGeom_Module()
{
}

void NewGeom_Module::initialize(CAM_Application* theApp)
{
}

void NewGeom_Module::windows(QMap<int, int>& theWndMap) const
{
  theWndMap.insert( LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
}

void NewGeom_Module::viewManagers(QStringList& theList) const
{
  theList.append( OCCViewer_Viewer::Type() );
}

bool NewGeom_Module::activateModule(SUIT_Study* theStudy)
{
  return LightApp_Module::activateModule(theStudy);
}

bool NewGeom_Module::deactivateModule(SUIT_Study* theStudy)
{
  return LightApp_Module::deactivateModule(theStudy);
}


