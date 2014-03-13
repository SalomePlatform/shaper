
#ifndef XGUI_MenuGroupPanel_H
#define XGUI_MenuGroupPanel_H

#include "XGUI_Interfaces.h"

#include <QWidget>
#include <QMap>

class QGridLayout;
class XGUI_Command;

class XGUI_MenuGroupPanel : public QWidget, public IMenuGroup
{
    Q_OBJECT
public:
    explicit XGUI_MenuGroupPanel(QWidget *parent = 0);

    virtual IFeatureMenu* addFeature(const QString& theId, 
                                     const QString& theTitle, 
                                     const QString& theTip, 
                                     const QIcon& theIcon, 
                                     const QKeySequence& theKeys = QKeySequence());

protected:
    virtual void resizeEvent(QResizeEvent *theEvent);

private:
    void addWidget(QWidget* theWgt);
    void placeWidget(QWidget* theWgt);
    void addCommand(XGUI_Command* theAction);

    QMap<XGUI_Command*, QWidget*> myActions;
    QGridLayout* myLayout;
    int myNewRow;
    int myNewCol;
    int myMaxRow;
};

#endif