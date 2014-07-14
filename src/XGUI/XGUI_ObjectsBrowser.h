
#ifndef XGUI_ObjectsBrowser_H
#define XGUI_ObjectsBrowser_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Object.h>

#include <QWidget>
#include <QTreeView>

class XGUI_DocumentDataModel;
class QLineEdit;


class XGUI_DataTree: public QTreeView
{
  Q_OBJECT
public:
  XGUI_DataTree(QWidget* theParent);
  virtual ~XGUI_DataTree();

  //! Returns list of currently selected objects
  QList<ObjectPtr> selectedObjects() const { return mySelectedData; }

  XGUI_DocumentDataModel* dataModel() const;

signals:
  //! Emited when selection is changed
  void selectionChanged();
  void activePartChanged(ObjectPtr thePart); 
 
  //! Emited on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

protected slots:
  virtual void commitData(QWidget* theEditor);

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent* theEvent);
  virtual void contextMenuEvent(QContextMenuEvent* theEvent);

private slots:
  //! Called when selection in Data Tree is changed
  void onSelectionChanged(const QItemSelection& theSelected, const QItemSelection& theDeselected);

private:
  //! List of currently selected data
  QList<ObjectPtr> mySelectedData;
};


/**\class XGUI_ObjectsBrowser
 * \ingroup GUI
 * \brief Object browser window object. Represents data tree of current data structure
 */
 class XGUI_EXPORT XGUI_ObjectsBrowser : public QWidget
{
  Q_OBJECT
public:
  XGUI_ObjectsBrowser(QWidget* theParent);
  virtual ~XGUI_ObjectsBrowser();

  //! Returns Model which provides access to data objects
  XGUI_DocumentDataModel* dataModel() const { return myDocModel; }

  //! Returns list of currently selected objects
  QList<ObjectPtr> selectedObjects() const { return myObjectsList; }

  void setObjectsSelected(const QList<ObjectPtr>& theObjects);

  //! Returns currently selected indexes
  QModelIndexList selectedIndexes() const { return myTreeView->selectionModel()->selectedIndexes(); }

  //! Returns TreeView widget
  XGUI_DataTree* treeView() const { return myTreeView; }

  //! Activates currently selected part. Signal activePartChanged will not be sent
  void activatePart(const ObjectPtr& thePart);

  void rebuildDataTree();

signals:
  //! Emited when selection is changed
  void selectionChanged();

  //! Emited when current active document is changed
  void activePartChanged(ObjectPtr thePart); 
 
  //! Emited on context menu request
  void contextMenuRequested(QContextMenuEvent* theEvent);

protected:
  virtual bool eventFilter(QObject* obj, QEvent* theEvent);

private slots:
  void onActivePartChanged(ObjectPtr thePart);
  void onContextMenuRequested(QContextMenuEvent* theEvent);
  void onLabelContextMenuRequested(const QPoint& thePnt);

  //! Called on Edit command request
  void onEditItem();

  void onSelectionChanged();

private:
  void closeDocNameEditing(bool toSave);

  //! Internal model
  XGUI_DocumentDataModel* myDocModel;

  QLineEdit* myActiveDocLbl;
  XGUI_DataTree* myTreeView;

  QList<ObjectPtr> myObjectsList;
};

#endif