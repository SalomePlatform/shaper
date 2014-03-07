
#ifndef XGUI_Module_H
#define XGUI_Module_H

#include <QString>

class XGUI_Module
{
public:
    //! Returns a string in XML format with definition of the module commands
    virtual QString moduleDescription() const = 0;
};


//! This function must return a new module instance.
extern "C"
{
   typedef XGUI_Module* (*CREATE_FUNC)();
}

#define CREATE_MODULE "createModule"


#endif