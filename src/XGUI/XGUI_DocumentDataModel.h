
#ifndef XGUI_DocumentDataModel_H
#define XGUI_DocumentDataModel_H

#include "XGUI.h"
#include "XGUI_Constants.h"

#include <QAbstractItemModel>
#include <Events_Listener.h>

#include <QList>

class ModelAPI_Document;
class XGUI_PartModel;
class XGUI_TopDataModel;

/**\class XGUI_DocumentDataModel
 * \ingroup GUI
 * \brief This is a proxy data model for Object Browser (QTreeView).
 * It contains several sub-models for generation of each sub-part of data tree.
 */
class XGUI_EXPORT XGUI_DocumentDataModel : public QAbstractItemModel, public Events_Listener
{
  Q_OBJECT
public:


  XGUI_DocumentDataModel(QObject* theParent);
  virtual ~XGUI_DocumentDataModel();

  // Event Listener method
  virtual void processEvent(const Events_Message* theMessage);


  virtual QVariant data(const QModelIndex& theIndex, int theRole) const;
  virtual QVariant headerData(int theSection, Qt::Orientation theOrient,
                              int theRole = Qt::DisplayRole) const;

  virtual int rowCount(const QModelIndex& theParent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex& theParent = QModelIndex()) const;

  virtual QModelIndex index(int theRow, int theColumn, 
                            const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex parent(const QModelIndex& theIndex) const;

  virtual bool hasChildren(const QModelIndex& theParent = QModelIndex()) const;

  bool insertRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  bool removeRows(int theRow, int theCount, const QModelIndex& theParent = QModelIndex());

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  FeaturePtr feature(const QModelIndex& theIndex) const;

  //! Activates a part data model if the index is a Part node index. 
  //! Returns true if active part changed.
  bool activatedIndex(const QModelIndex& theIndex);

  FeaturePtr activePart() const;

private:

  enum {PartsFolder, HistoryNode};


  //! Converts QModelIndex of this model to QModelIndex of a one of sub-models.
  QModelIndex* toSourceModelIndex(const QModelIndex& theProxy) const;

  //! Finds a pointer on QModelIndex which is equal to the given one
  QModelIndex* findModelIndex(const QModelIndex& theIndex) const;

  //! Returns pointer on QModelIndex which is equal to the given one.
  QModelIndex* getModelIndex(const QModelIndex& theIndex) const;

  //! Deletes all saved pointers on QModelIndex objects.
  void clearModelIndexes();

  //! Removes sub-model on removing a part object. Also it removes QModelIndex-es which refer to this model
  void removeSubModel(int theModelId);

  //! Returns true if the given model is a one of sub-models (of both types)
  bool isSubModel(const QAbstractItemModel* theModel) const;

  //! Returns true if the given model is a one of sub-models of Part type
  bool isPartSubModel(const QAbstractItemModel* theModel) const;

  //! Returns Parts Folder node
  QModelIndex partFolderNode() const;

  int historyOffset() const;


  //! Document
  boost::shared_ptr<ModelAPI_Document> myDocument;

  //! Data model of top part of data tree (not parts object)
  XGUI_TopDataModel* myModel;

  //! Data models for Parts data tree representation (one data model per a one part)
  QList<XGUI_PartModel*> myPartModels;

  //! Active part in part editing mode
  XGUI_PartModel* myActivePart;

  //! List of saved QModelIndexes created by sub-models
  QList<QModelIndex*> myIndexes;

};

#endif