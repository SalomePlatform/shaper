
#ifndef XGUI_ObjectsBrowser_H
#define XGUI_ObjectsBrowser_H

#include "XGUI_Constants.h"

#include <QTreeView>

class XGUI_DocumentDataModel;

class XGUI_ObjectsBrowser : public QTreeView
{
  Q_OBJECT
public:
  XGUI_ObjectsBrowser(QWidget* theParent);
  virtual ~XGUI_ObjectsBrowser();

  XGUI_DocumentDataModel* dataModel() const { return myDocModel; }

  QFeatureList selectedData() const { return mySelectedData; }

signals:
  void selectionChanged();


private slots:
  void onSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

private:
  XGUI_DocumentDataModel* myDocModel;

  QFeatureList mySelectedData;
};

#endif