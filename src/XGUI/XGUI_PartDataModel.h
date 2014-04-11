
#ifndef XGUI_PartDataModel_H
#define XGUI_PartDataModel_H

#include "XGUI_DataTreeModel.h"

/**\class XGUI_TopDataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It represents only upper part of data tree (non-parts tree items)
 */
class XGUI_TopDataModel : public XGUI_FeaturesModel
{
  Q_OBJECT
public:
  XGUI_TopDataModel(const std::shared_ptr<ModelAPI_Document>& theDocument, QObject* theParent);
  virtual ~XGUI_TopDataModel();

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

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  virtual FeaturePtr feature(const QModelIndex& theIndex) const;

  virtual QModelIndex findParent(const std::shared_ptr<ModelAPI_Feature>& theFeature) const;

private:
  //! Types of QModelIndexes
  enum DataIds {
    ParamsFolder,
    ParamObject,
    ConstructFolder,
    ConstructObject
  };

};


/**\class XGUI_PartDataModel
 * \ingroup GUI
 * \brief This is a data model for Object Browser (QTreeView).
 * It represents data tree only of a one part
 */
class XGUI_PartDataModel : public XGUI_PartModel
{
  Q_OBJECT
public:
  XGUI_PartDataModel(const std::shared_ptr<ModelAPI_Document>& theDocument, QObject* theParent);
  virtual ~XGUI_PartDataModel();

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

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  virtual FeaturePtr feature(const QModelIndex& theIndex) const;

  //! Returns true if the given document is a sub-document of this tree
  virtual bool hasDocument(const std::shared_ptr<ModelAPI_Document>& theDoc) const;

  //! Returns parent index of the given feature
  virtual QModelIndex findParent(const std::shared_ptr<ModelAPI_Feature>& theFeature) const;

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

};

#endif