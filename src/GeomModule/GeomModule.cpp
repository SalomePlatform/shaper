#include "GeomModule.h"

#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTextStream>

/*!Create and return new instance of XGUI_Module*/
extern "C" GM_EXPORT IModule* createModule(IWorkshop* theWshop)
{
  return new GeomModule(theWshop);
}


GeomModule::GeomModule(IWorkshop* theWshop)
{
    myWorkshop = theWshop;
}


GeomModule::~GeomModule()
{
}


void GeomModule::createFeatures()
{
    int aPageId = myWorkshop->addWorkbench("Primitives");
    int aGroupId = myWorkshop->addGroup(aPageId);
    int aCommand = myWorkshop->addFeature(aPageId, aGroupId, "Box", "Create Box", QIcon(":icons/box.png"), QKeySequence());
    aCommand = myWorkshop->addFeature(aPageId, aGroupId, "Cylinder", "Create Cylinder", QIcon(":icons/cylinder.png"), QKeySequence());
    aCommand = myWorkshop->addFeature(aPageId, aGroupId, "Disk", "Create Disk", QIcon(":icons/disk.png"), QKeySequence());
    aCommand = myWorkshop->addFeature(aPageId, aGroupId, "Torus", "Create Torus", QIcon(":icons/torus.png"), QKeySequence());

    aPageId = myWorkshop->addWorkbench("Operations");
}