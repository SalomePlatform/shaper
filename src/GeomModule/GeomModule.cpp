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
    IWorkbench* aPage = myWorkshop->addWorkbench("Primitives");
    IMenuGroup* aGroup = aPage->addGroup();

    IFeatureMenu* aCommand = aGroup->addFeature("new_part", "Part", "Creates a new part", QIcon(":pictures/part_ico.png"));
    aCommand = aGroup->addFeature("new_point", "Point", "Create a new point", QIcon(":icons/point.png"));
    aCommand = aGroup->addFeature("new_axis", "Axis", "Create a new axis", QIcon(":icons/axis.png"), QKeySequence());
    aCommand = aGroup->addFeature("new_plane", "Plane", "Create a new plane", QIcon(":icons/plane.png"), QKeySequence());

    //aGroup = aPage->addGroup();
    aCommand = aGroup->addFeature("duplicate", "Duplicate", "Duplicate selected object", QIcon(":icons/duplicate.png"));
    aCommand = aGroup->addFeature("remove", "Remove", "Remove selected object", QIcon(":icons/remove.png"));

    aPage = myWorkshop->addWorkbench("Features");
    aGroup = aPage->addGroup();

    aCommand = aGroup->addFeature("extrusion", "Extrusion", "Make extrusion", QIcon(":icons/extrusion.png"));
    aCommand = aGroup->addFeature("cut", "Cut", "Make cut", QIcon(":icons/cut.png"));
    aCommand = aGroup->addFeature("fusion", "Fusion", "Make fusion", QIcon(":icons/fusion.png"));
    aCommand = aGroup->addFeature("revolution", "Revolution", "Make revolution", QIcon(":icons/revol.png"));
    aCommand = aGroup->addFeature("common", "Common", "Make common", QIcon(":icons/common.png"));

    //aGroup = aPage->addGroup();
    aCommand = aGroup->addFeature("import", "Import", "Make import", QIcon(":icons/import.png"));
}