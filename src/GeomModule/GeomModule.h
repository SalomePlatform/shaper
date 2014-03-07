
#ifndef GeomModule_H
#define GeomModule_H

#include "GeomModule_Defs.h"

#include <XGUI_Module.h>

class GM_EXPORT GeomModule : public XGUI_Module
{
public:
    GeomModule();
    virtual ~GeomModule();

    virtual QString moduleDescription() const;

private:
    QString myMenuXML;
};

#endif