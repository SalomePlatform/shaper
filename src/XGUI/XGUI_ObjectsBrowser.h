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
#include <QLabel>

class ModuleBase_IDocumentDataModel;
class XGUI_DataModel;

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


#if (!defined HAVE_SALOME) && (defined WIN32)
#include <QWindowsVistaStyle>
/**
* \ingroup GUI
* Implementation of XGUI_DataTree custom style
*/
class XGUI_TreeViewStyle : public QWindowsVistaStyle
{
  Q_OBJECT
public:
  XGUI_TreeViewStyle() : QWindowsVistaStyle() {}

  void drawPrimitive(PrimitiveElement theElement, const QStyleOption* theOption,
                     QPainter* thePainter, const QWidget* theWidget = 0) const;

  void setIndex(const QModelIndex& theIndex) { myIndex = theIndex; }
  QModelIndex index() const { return myIndex; }

private:
  QModelIndex myIndex;
};
#endif

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

#if (!defined HAVE_SALOME) && (defined WIN32)
  virtual void drawRow(QPainter* thePainter,
                        const QStyleOptionViewItem& theOptions,
                        const QModelIndex& theIndex) const;
private:
  XGUI_TreeViewStyle* myStyle;
#endif
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
  QLabel* activeDocLabel() const { return myActiveDocLbl; }

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
  QModelIndexList expandedItems(const QModelIndex& theParent = QModelIndex()) const;

  //! Internal model
  XGUI_DataModel* myDocModel;
  XGUI_ActiveDocLbl* myActiveDocLbl;
  XGUI_DataTree* myTreeView;
};

#endif
