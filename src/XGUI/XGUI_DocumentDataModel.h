
#ifndef XGUI_DocumentDataModel_H
#define XGUI_DocumentDataModel_H

#include "XGUI_Constants.h"

#include <QAbstractItemModel>
#include <Event_Listener.h>

#include <QList>

class ModelAPI_Document;
class XGUI_PartModel;
class XGUI_TopDataModel;

/**\class XGUI_DocumentDataModel
 * \ingroup GUI
 * \brief This is a proxy data model for Object Browser (QTreeView).
 * It contains several sub-models for generation of each sub-part of data tree.
 */
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

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  FeaturePtr feature(const QModelIndex& theIndex) const;

private:

  //! Converts QModelIndex of this model to QModelIndex of a one of sub-models.
  QModelIndex toSourceModel(const QModelIndex& theProxy) const;

  //! Finds a pointer on QModelIndex which is equal to the given one
  QModelIndex* findModelIndex(const QModelIndex& theIndex) const;

  //! Returns pointer on QModelIndex which is equal to the given one.
  QModelIndex* getModelIndex(const QModelIndex& theIndex) const;

  //! Deletes all saved pointers on QModelIndex objects.
  void clearModelIndexes();

  //! Document
  std::shared_ptr<ModelAPI_Document> myDocument;

  //! Data model of top part of data tree (not parts object)
  XGUI_TopDataModel* myModel;

  //! Data models for Parts data tree representation (one data model per a one part)
  QList<XGUI_PartModel*> myPartModels;

  //! List of saved QModelIndexes created by sub-models
  QList<QModelIndex*> myIndexes;
};

#endif