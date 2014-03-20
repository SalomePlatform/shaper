
#ifndef GeomModule_H
#define GeomModule_H

#include "GeomModule_Defs.h"

#include <XGUI_Interfaces.h>

class Config_XMLReader;

class GM_EXPORT GeomModule : public IModule
{
public:
    GeomModule(IWorkshop* theWshop);
    virtual ~GeomModule();

    virtual void createFeatures();

private:
    QString myMenuXML;

    IWorkshop* myWorkshop;
};

#endif
