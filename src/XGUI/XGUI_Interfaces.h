
#ifndef XGUI_Interfaces_H
#define XGUI_Interfaces_H

#include <QString>
#include <QIcon>
#include <QKeySequence>

class IWorkshop
{
public:
    virtual int addWorkbench(const QString& theName) = 0;

    virtual int addGroup(int thePageId) = 0;

    virtual int addFeature(int thePageId, int theGroupId, 
                           const QString& theTitle, const QString& theTip, 
                           const QIcon& theIcon, 
                           const QKeySequence& theKeys = QKeySequence()) = 0;
};


class IModule
{
public:
    virtual void createFeatures() = 0;
};


//! This function must return a new module instance.
extern "C"
{
   typedef IModule* (*CREATE_FUNC)(IWorkshop*);
}

#define CREATE_MODULE "createModule"


#endif