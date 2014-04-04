
#ifndef XGUI_DocumentDataModel_H
#define XGUI_DocumentDataModel_H

#include <QAbstractItemModel>

class ModelAPI_Document;

class XGUI_DocumentDataModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  XGUI_DocumentDataModel(QObject* theParent);
  virtual ~XGUI_DocumentDataModel();


  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex index(int theRow, int theColumn, 
                            const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex parent(const QModelIndex &index) const;

  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

private:
  //std::shared_ptr<ModelAPI_Feature> myRoot;
  short myParamsFolder;
  short myConstructFolder;

  std::shared_ptr<ModelAPI_Document> myDocument;
};

#endif