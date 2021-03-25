// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef XGUI_ObjectsBrowser_H
#define XGUI_ObjectsBrowser_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Folder.h>

#include <QWidget>
#include <QTreeView>
#include <QLabel>
#include <QMap>

class ModuleBase_IDocumentDataModel;
class XGUI_DataModel;
class Config_DataModelReader;
class XGUI_Workshop;
class ModuleBase_ITreeNode;

//#define DEBUG_INDXES

/**
* \ingroup GUI
* Implementation of root label in Object Browser
*/
class XGUI_ActiveDocLbl: public QLabel
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theText a text
   /// \param theParent a parent widget
   XGUI_ActiveDocLbl(const QString& theText, QWidget* theParent );

   /// Sets tree view
   /// \param theView a view
   void setTreeView(QTreeView* theView);

   /// Returns tree view
   QTreeView* treePalette() const { return myTreeView;}

#if (!defined HAVE_SALOME) && (defined WIN32)
   virtual bool event(QEvent* theEvent);
#endif

public slots:
  /// On unselect
  void unselect();

protected:
  /// On mouse release
  virtual void mouseReleaseEvent( QMouseEvent* e);

  /// Filter event
  bool eventFilter(QObject* theObj, QEvent* theEvent);

private:
  QString myPreSelectionStyle;
  QString myNeutralStyle;
  QString mySelectionStyle;

  QTreeView* myTreeView;
  bool myIsSelected;
};


/**
* \ingroup GUI
* Implementation of Data Tree object for Object Browser
*/
class XGUI_EXPORT XGUI_DataTree : public QTreeView
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

   /// Redefinition of virtual method
  virtual void mouseReleaseEvent(QMouseEvent* theEvent);

#ifdef DEBUG_INDXES
  virtual void mousePressEvent(QMouseEvent* theEvent);
#endif

private:
  /// Process a history change request
  /// \param theIndex a clicked data index
  void processHistoryChange(const QModelIndex& theIndex);

  /// Process a visibility change request
  /// \param theIndex a clicked data index
  void processEyeClick(const QModelIndex& theIndex);
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
  XGUI_ObjectsBrowser(QWidget* theParent, XGUI_Workshop* theWorkshop);
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

  //! Scroll TreeView to make given object visible
  //! \param theObject object to make it visible
  void ensureVisible(const ObjectPtr theObject);

  //! Returns currently selected indexes
  QModelIndexList selectedIndexes() const
  {
    if (myTreeView->selectionModel())
      return myTreeView->selectionModel()->selectedIndexes();
    else
      return QModelIndexList();
  }

  //! Returns TreeView widget
  XGUI_DataTree* treeView() const
  {
    return myTreeView;
  }

  /// Returns active doc label object
  QLabel* activeDocLabel() const { return myActiveDocLbl; }

  /// Rebuild data tree
  void rebuildDataTree();

  /// Resets the object browser into initial state
  void clearContent();

  /// Initialize the Object browser
  void initialize(ModuleBase_ITreeNode* theRoot);

  /// Returns list of folders opened state for the given document
  /// \param theDoc the document
  /// \return list of booleans with state expanded or not
  std::list<bool> getStateForDoc(DocumentPtr theDoc) const;

  /// Set folders opened state for the given document
  /// \param theDoc the document
  /// \param theStates list of booleans with state expanded or not
  void setStateForDoc(DocumentPtr theDoc, const std::list<bool>& theStates);

  /// Returns current workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

  void onSelectionChanged();

  /// Updates all items of object browser
  /// \param theColumn - column of items
  /// \param theParent - a parent item (by default from root)
  void updateAllIndexes(int theColumn = 0, const QModelIndex& theParent = QModelIndex());

  QMap<ObjectPtr, bool> getFoldersState(DocumentPtr theDoc) const;

  void setFoldersState(const QMap<ObjectPtr, bool>& theStates);

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

  //! An signal emitted on resize of the Object Browser
  void sizeChanged();


protected:
  //! redefinition of a virtual method
  void resizeEvent(QResizeEvent* theEvent);


 private slots:
  /// Show context menu
  /// \param theEvent a context menu event
  void onContextMenuRequested(QContextMenuEvent* theEvent);

  /// Show context menu on upper label
  /// \param thePnt a position of context menu
  void onLabelContextMenuRequested(const QPoint& thePnt);

  //! Called when selection in Data Tree is changed
  void onSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

  void onBeforeReset();

  void onAfterModelReset();

 private:
   QList<ModuleBase_ITreeNode*> expandedItems(const QModelIndex& theParent = QModelIndex()) const;

  //! Internal model
  XGUI_DataModel* myDocModel;
  XGUI_ActiveDocLbl* myActiveDocLbl;
  XGUI_DataTree* myTreeView;
  XGUI_Workshop* myWorkshop;

  /// A field to store expanded items before model reset
  //QModelIndexList myExpandedItems;
  QList<ModuleBase_ITreeNode*> myExpandedItems;
};

#endif
