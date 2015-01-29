// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_DataTreeModel_H
#define XGUI_DataTreeModel_H

#include "XGUI.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultPart.h>

#include <QAbstractItemModel>
#include <QColor>

/**\class XGUI_FeaturesModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with features data.
 */
class XGUI_EXPORT XGUI_FeaturesModel : public QAbstractItemModel
{
 public:
   /// Constructor
   /// \param theParent a parent object
  XGUI_FeaturesModel(QObject* theParent)
      : QAbstractItemModel(theParent),
        myItemsColor(Qt::black)
  {
  }

  //! Returns Feature object by the given Model index.
  //! Returns 0 if the given index is not index of a feature
  /// \param theIndex a model index
  virtual ObjectPtr object(const QModelIndex& theIndex) const = 0;

  //! Returns QModelIndex which corresponds to the given feature
  //! If the feature is not found then index is not valid
  virtual QModelIndex objectIndex(const ObjectPtr& theFeature) const = 0;

  //! Returns parent index of the given feature
  virtual QModelIndex findParent(const ObjectPtr& theObject) const = 0;

  //! Returns index corresponded to the group
  //! \param theGroup a group name
  virtual QModelIndex findGroup(const std::string& theGroup) const = 0;

  //! Set color of items
  void setItemsColor(const QColor& theColor)
  {
    myItemsColor = theColor;
  }

  //! Returns color of items
  QColor itemsColor() const
  {
    return myItemsColor;
  }

 protected:
   /// Color of items
  QColor myItemsColor;
};

/**\class XGUI_PartModel
 * \ingroup GUI
 * \brief Abstaract class of model object which operates with parts data.
 */
class XGUI_PartModel : public XGUI_FeaturesModel
{
 public:
   /// Constructor
   /// \param theParent a parent object
  XGUI_PartModel(QObject* theParent)
      : XGUI_FeaturesModel(theParent)
  {
  }

  /// Set part id
  /// \param theId a new id
  void setPartId(int theId)
  {
    myId = theId;
  }

  //! Returns true if the given document is a sub-document of this tree
  //! \param theDoc a document to check
  virtual bool hasDocument(const DocumentPtr& theDoc) const = 0;

  //! Return a Part object
  virtual ResultPartPtr part() const = 0;

 protected:
  //! Id of the current part object in the document
  int myId;
};

#endif
