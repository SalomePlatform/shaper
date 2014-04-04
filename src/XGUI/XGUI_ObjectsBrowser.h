
#ifndef XGUI_ObjectsBrowser_H
#define XGUI_ObjectsBrowser_H

#include <QTreeView>

class XGUI_DocumentDataModel;

class XGUI_ObjectsBrowser : public QTreeView
{
  Q_OBJECT
public:
  XGUI_ObjectsBrowser(QWidget* theParent);
  virtual ~XGUI_ObjectsBrowser();

  XGUI_DocumentDataModel* dataModel() const { return myDocModel; }

private:

  XGUI_DocumentDataModel* myDocModel;
};

#endif