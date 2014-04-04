
#ifndef XGUI_DataModel_H
#define XGUI_DataModel_H

#include <QAbstractItemModel>

class XGUI_DataModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  XGUI_DataModel(QObject* theParent);
  virtual ~XGUI_DataModel();
};

#endif