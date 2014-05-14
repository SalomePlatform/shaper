
#ifndef XGUI_DataTreeModel_H
#define XGUI_DataTreeModel_H

#include "XGUI.h"
#include "XGUI_Constants.h"

#include <ModelAPI_Document.h>
#include <QAbstractItemModel>

/**\class XGUI_FeaturesModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with features data.
 */
class XGUI_EXPORT XGUI_FeaturesModel : public QAbstractItemModel
{
public:
  XGUI_FeaturesModel(const boost::shared_ptr<ModelAPI_Document>& theDocument, QObject* theParent):
      QAbstractItemModel(theParent), myDocument(theDocument) {}

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  virtual FeaturePtr feature(const QModelIndex& theIndex) const = 0;

  //! Returns parent index of the given feature
  virtual QModelIndex findParent(const boost::shared_ptr<ModelAPI_Feature>& theFeature) const = 0;

  //! Returns index corresponded to the group
  virtual QModelIndex findGroup(const std::string& theGroup) const = 0;

protected:
  boost::shared_ptr<ModelAPI_Document> myDocument;
};


/**\class XGUI_PartModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with parts data.
 */
class XGUI_PartModel : public XGUI_FeaturesModel
{
public:
  XGUI_PartModel(const boost::shared_ptr<ModelAPI_Document>& theDocument, QObject* theParent):
      XGUI_FeaturesModel(theDocument, theParent) {}

  void setPartId(int theId) { myId = theId; }

  //! Returns true if the given document is a sub-document of this tree
  virtual bool hasDocument(const boost::shared_ptr<ModelAPI_Document>& theDoc) const = 0;

  //! Return a Part object
  virtual FeaturePtr part() const = 0;

protected:
  //! Id of the current part object in the document
  int myId;
};


#endif