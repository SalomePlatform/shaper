
#ifndef XGUI_PartDataModel_H
#define XGUI_PartDataModel_H

#include <QAbstractItemModel>

class ModelAPI_Feature;
class ModelAPI_Document; 

/**\class XGUI_TopDataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It represents only upper part of data tree (non-parts tree items)
 */
class XGUI_TopDataModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  XGUI_TopDataModel(QObject* theParent);
  virtual ~XGUI_TopDataModel();
 
  //! Set a document object
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
  //! Types of QModelIndexes
  enum DataIds {
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject
  };

  //! Document object
  std::shared_ptr<ModelAPI_Document> myDocument;
};


/**\class XGUI_PartDataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It represents data tree only of a one part
 */
class XGUI_PartDataModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  XGUI_PartDataModel(QObject* theParent);
  virtual ~XGUI_PartDataModel();

  //! Set a document object and Id of a part in the document
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

  //! Types of QModelIndexes
  enum DataIds {
    MyRoot,
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject
  };

  //! Document object
  std::shared_ptr<ModelAPI_Document> myDocument;

  //! Id of the current part object in the document
  int myId;
};

#endif