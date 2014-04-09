
#ifndef XGUI_DocumentDataModel_H
#define XGUI_DocumentDataModel_H

#include <QAbstractItemModel>
#include <Event_Listener.h>

#include <QList>

class ModelAPI_Document;
class XGUI_PartDataModel;
class XGUI_TopDataModel;


class XGUI_DocumentDataModel : public QAbstractItemModel, public Event_Listener
{
  Q_OBJECT
public:
  XGUI_DocumentDataModel(QObject* theParent);
  virtual ~XGUI_DocumentDataModel();

  // Event Listener method
  virtual void processEvent(const Event_Message* theMessage);


  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;
  virtual QVariant headerData(int theSection, Qt::Orientation theOrient,
                              int theRole = Qt::DisplayRole) const;

  virtual int rowCount(const QModelIndex& theParent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex& theParent = QModelIndex()) const;

  virtual QModelIndex index(int theRow, int theColumn, 
                            const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

private:

  QModelIndex toSourceModel(const QModelIndex& theProxy) const;
  QModelIndex fromSourceModel(const QModelIndex& theSource) const;


  std::shared_ptr<ModelAPI_Document> myDocument;

  XGUI_TopDataModel* myModel;
  QList<XGUI_PartDataModel*> myPartModels;
};

#endif