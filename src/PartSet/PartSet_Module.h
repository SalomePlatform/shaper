
#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <XGUI_Module.h>

class Config_FeatureReader;

class PARTSET_EXPORT PartSet_Module : public XGUI_Module
{
public:
    PartSet_Module(XGUI_Workshop* theWshop);
    virtual ~PartSet_Module();

    virtual void createFeatures();

private:
    QString myMenuXML;

    XGUI_Workshop* myWorkshop;
};

#endif
