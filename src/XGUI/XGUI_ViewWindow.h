#ifndef XGUI_ViewWindow_H
#define XGUI_ViewWindow_H

#include <QWidget>

class QLabel;
class QToolBar;

class XGUI_ViewWindow : public QWidget
{
    Q_OBJECT
public:
    XGUI_ViewWindow();
    virtual ~XGUI_ViewWindow();

protected:
    virtual void resizeEvent(QResizeEvent* theEvent);

    virtual void changeEvent(QEvent* theEvent);

    virtual void enterEvent(QEvent* theEvent);
    virtual void leaveEvent(QEvent* theEvent);

private slots:
    void onClose();
    void onMinimize();

private:

    QLabel* myPicture;
    QLabel* myViewPort;
    QToolBar* aViewBar;
    QToolBar* aWindowBar;
};

#endif