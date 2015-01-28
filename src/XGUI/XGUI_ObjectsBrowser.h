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

class XGUI_DocumentDataModel;
class QLineEdit;

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

  //! Returns list of currently selected objects
  QObjectPtrList selectedObjects() const
  {
    return mySelectedData;
  }

  /// Returns current data model
  XGUI_DocumentDataModel* dataModel() const;

signals:
  //! Emited when selection is changed
  void selectionChanged();

  //! Emited when active part changed
  void activePartChanged(ObjectPtr thePart);

  //! Emited on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

public slots:
  /// Clear content of data tree
  virtual void clear();

 protected slots:
   /// Commit modified data (used for renaming of objects)
  virtual void commitData(QWidget* theEditor);

 protected:
   /// Redefinition of virtual method
  virtual void mouseDoubleClickEvent(QMouseEvent* theEvent);

   /// Redefinition of virtual method
  virtual void contextMenuEvent(QContextMenuEvent* theEvent);

 private slots:
  //! Called when selection in Data Tree is changed
  void onSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

 private:
  //! List of currently selected data
  QObjectPtrList mySelectedData;
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
  XGUI_DocumentDataModel* dataModel() const
  {
    return myDocModel;
  }

  //! Returns list of currently selected objects
  QObjectPtrList selectedObjects() const
  {
    return myObjectsList;
  }

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

  //! Activates currently selected part. Signal activePartChanged will not be sent
  void activatePart(const ResultPartPtr& thePart);

  /// Rebuild data tree
  void rebuildDataTree();

  /// Process application event
  /// \param theMessage an event message
  void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Resets the object browser into initial state
  void clearContent();

signals:
  //! Emited when selection is changed
  void selectionChanged();

  //! Emited when current active document is changed
  void activePartChanged(ObjectPtr thePart);

  //! Emited on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

 protected:
   /// Redefinition of virtual method
  virtual bool eventFilter(QObject* obj, QEvent* theEvent);

 private slots:
   /// Activate part
   /// \param thePart a part to activate
  void onActivePartChanged(ObjectPtr thePart);

  /// Show context menu
  /// \param theEvent a context menu event
  void onContextMenuRequested(QContextMenuEvent* theEvent);

  /// Show context menu on upper label
  /// \param thePnt a position of context menu
  void onLabelContextMenuRequested(const QPoint& thePnt);

  //! Called on Edit command request
  void onEditItem();

  /// Process selection changed event
  void onSelectionChanged();

 private:
  void closeDocNameEditing(bool toSave);

  //! Internal model
  XGUI_DocumentDataModel* myDocModel;

  QLineEdit* myActiveDocLbl;
  XGUI_DataTree* myTreeView;

  QObjectPtrList myObjectsList;
};

#endif
