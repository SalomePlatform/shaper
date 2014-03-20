
#ifndef XGUI_Module_H
#define XGUI_Module_H

#include <QString>
#include <QIcon>
#include <QKeySequence>

#include <XGUI_Workshop.h>

class XGUI_Module
{
public:
    virtual void createFeatures() = 0;
};

//! This function must return a new module instance.
extern "C"
{
   typedef XGUI_Module* (*CREATE_FUNC)(XGUI_Workshop*);
}

#define CREATE_MODULE "createModule"


#endif //XGUI_Module
