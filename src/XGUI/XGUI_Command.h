#ifndef XGUI_Command_H
#define XGUI_Command_H

#include <QWidgetAction>

#define MIN_BUTTON_HEIGHT 18
#define MIN_BUTTON_WIDTH 40


class XGUI_Command : public QWidgetAction
{
    Q_OBJECT
public:
    XGUI_Command(QObject * parent);
    XGUI_Command(const QIcon& icon, const QString& text, QObject* parent);
    ~XGUI_Command();

protected:
    virtual QWidget* createWidget(QWidget* theParent);
};

#endif