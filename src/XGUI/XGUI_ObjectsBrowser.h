// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_ObjectsBrowser_H
#define XGUI_ObjectsBrowser_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Events.h>

#include <QWidget>
#include <QTreeView>

class ModuleBase_IDocumentDataModel;
class QLineEdit;
class XGUI_DataModel;

/**
* \ingroup GUI
* Implementation of Data Tree object for Object Browser
*/
class XGUI_DataTree : public QTreeView
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent widget
  XGUI_DataTree(QWidget* theParent);

  virtual ~XGUI_DataTree();

  /// Returns current data model
  XGUI_DataModel* dataModel() const;

signals:
  //! Emited on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

public slots:
  /// Clear content of data tree
  virtual void clear();

 protected slots:
  /// Commit modified data (used for renaming of objects)
  virtual void commitData(QWidget* theEditor);

  /// A slot which is called on mouse double click
  void onDoubleClick(const QModelIndex&);

 protected:
   /// Redefinition of virtual method
  virtual void contextMenuEvent(QContextMenuEvent* theEvent);

   /// Redefinition of virtual method
  virtual void resizeEvent(QResizeEvent* theEvent);

};

/**\class XGUI_ObjectsBrowser
 * \ingroup GUI
 * \brief Object browser window object. Represents data tree of current data structure
 */
class XGUI_EXPORT XGUI_ObjectsBrowser : public QWidget
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent widget
  XGUI_ObjectsBrowser(QWidget* theParent);
  virtual ~XGUI_ObjectsBrowser();

  //! Returns Model which provides access to data objects
  XGUI_DataModel* dataModel() const
  {
    return myDocModel;
  }

  //! Returns list of currently selected objects
  //! \param theIndexes - output list of corresponded indexes (can be NULL)
  QObjectPtrList selectedObjects(QModelIndexList* theIndexes = 0) const;

  /// Set selected list of objects
  /// \param theObjects list of objects to select
  void setObjectsSelected(const QObjectPtrList& theObjects);

  //! Returns currently selected indexes
  QModelIndexList selectedIndexes() const
  {
    return myTreeView->selectionModel()->selectedIndexes();
  }

  //! Returns TreeView widget
  XGUI_DataTree* treeView() const
  {
    return myTreeView;
  }

  /// Returns active doc label object
  QLineEdit* activeDocLabel() const { return myActiveDocLbl; }

  /// Rebuild data tree
  void rebuildDataTree();

  /// Resets the object browser into initial state
  void clearContent();

public slots:
  //! Called on Edit command request
  void onEditItem();

signals:
  //! Emited when selection is changed
  void selectionChanged();

  //! Emited on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

  //! Segnal is emitted when user cliks by mouse in header label of object browser
  void headerMouseDblClicked(const QModelIndex&);

 protected:
   /// Redefinition of virtual method
  virtual bool eventFilter(QObject* obj, QEvent* theEvent);

 private slots:
  /// Show context menu
  /// \param theEvent a context menu event
  void onContextMenuRequested(QContextMenuEvent* theEvent);

  /// Show context menu on upper label
  /// \param thePnt a position of context menu
  void onLabelContextMenuRequested(const QPoint& thePnt);

  //! Called when selection in Data Tree is changed
  void onSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

 private:
  void closeDocNameEditing(bool toSave);

  //! Internal model
  XGUI_DataModel* myDocModel;
  QLineEdit* myActiveDocLbl;
  XGUI_DataTree* myTreeView;
};

#endif
