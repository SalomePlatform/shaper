#ifndef XGUI_Command_H
#define XGUI_Command_H

#include "XGUI_Interfaces.h"

#include <QWidgetAction>

#define MIN_BUTTON_HEIGHT 18
#define MIN_BUTTON_WIDTH 40


class XGUI_Command : public QWidgetAction, public IFeatureMenu
{
    Q_OBJECT
public:
    XGUI_Command(const QString& theId, QObject * parent);
    XGUI_Command(const QString& theId, const QIcon& icon, const QString& text, QObject* parent);
    ~XGUI_Command();

    virtual bool enabled() const;
    virtual void enable();
    virtual void disable();
    virtual QString getId() const { return myId; }

    virtual void connectTo(const QObject* theResiver, const char* theSlot);

protected:
    virtual QWidget* createWidget(QWidget* theParent);

private:
    QString myId;
};

#endif