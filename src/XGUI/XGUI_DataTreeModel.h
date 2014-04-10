

#ifndef XGUI_DataTreeModel_H
#define XGUI_DataTreeModel_H

#include <ModelAPI_Document.h>
#include <QAbstractItemModel>

#include "XGUI_Constants.h"

/**\class XGUI_FeaturesModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with features data.
 */
class XGUI_FeaturesModel : public QAbstractItemModel
{
public:
  XGUI_FeaturesModel(const std::shared_ptr<ModelAPI_Document>& theDocument, QObject* theParent):
      QAbstractItemModel(theParent), myDocument(theDocument) {}

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  virtual FeaturePtr feature(const QModelIndex& theIndex) const = 0;

protected:
  std::shared_ptr<ModelAPI_Document> myDocument;
};


/**\class XGUI_PartModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with parts data.
 */
class XGUI_PartModel : public XGUI_FeaturesModel
{
public:
  XGUI_PartModel(const std::shared_ptr<ModelAPI_Document>& theDocument, QObject* theParent):
      XGUI_FeaturesModel(theDocument, theParent) {}

      void setPartId(int theId) { myId = theId; }

protected:
  //! Id of the current part object in the document
  int myId;
};


#endif