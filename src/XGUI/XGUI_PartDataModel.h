
#ifndef XGUI_PartDataModel_H
#define XGUI_PartDataModel_H

#include <QAbstractItemModel>

class ModelAPI_Feature;
class ModelAPI_Document; 

class XGUI_TopDataModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  XGUI_TopDataModel(QObject* theParent);
  virtual ~XGUI_TopDataModel();
 
  virtual void setDocument(const std::shared_ptr<ModelAPI_Document>& theDoc)
  {
    myDocument = theDoc;
  }

  // Reimplementation from QAbstractItemModel
  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex index(int theRow, int theColumn, 
                            const QModelIndex& theParent = QModelIndex()) const;

  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

private:
  enum DataIds {
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject
  };

  std::shared_ptr<ModelAPI_Document> myDocument;
};



class XGUI_PartDataModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  XGUI_PartDataModel(QObject* theParent);
  virtual ~XGUI_PartDataModel();

  virtual void setDocument(const std::shared_ptr<ModelAPI_Document>& theDoc, int theId)
  {
    myDocument = theDoc;
    myId = theId;
  }

  // Reimplementation from QAbstractItemModel
  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex index(int theRow, int theColumn, 
                            const QModelIndex& theParent = QModelIndex()) const;

  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

private: 
  std::shared_ptr<ModelAPI_Document> featureDocument() const;


  enum DataIds {
    MyRoot,
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject
  };

  std::shared_ptr<ModelAPI_Document> myDocument;
  int myId;
};

#endif