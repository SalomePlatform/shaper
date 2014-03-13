
#ifndef XGUI_Interfaces_H
#define XGUI_Interfaces_H

#include <QString>
#include <QIcon>
#include <QKeySequence>


class IFeatureMenu
{
public:
    virtual bool enabled() const = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual QString getId() const = 0;
    virtual void connectTo(const QObject* theResiver, const char* theSlot) = 0;
};


class IMenuGroup
{
public:
    virtual IFeatureMenu* addFeature(const QString& theId, 
                                     const QString& theTitle, 
                                     const QString& theTip, 
                                     const QIcon& theIcon, 
                                     const QKeySequence& theKeys = QKeySequence()) = 0;
};


class IWorkbench
{
public:
    virtual IMenuGroup* addGroup() = 0;
};


class IWorkshop
{
public:
    virtual IWorkbench* addWorkbench(const QString& theName) = 0;
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