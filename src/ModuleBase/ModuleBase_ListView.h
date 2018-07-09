// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_ListView_H_
#define ModuleBase_ListView_H_

#include "ModuleBase.h"

#include <QModelIndex>
#include <QObject>
#include <QListWidget>
#include <QTimer>

#include <set>

class QAction;
class QListWidget;
class QWidget;


/**
* Customization of a List Widget to make it to be placed on full width of container
*/
class CustomListWidget : public QListWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget
  CustomListWidget(QWidget* theParent)
    : QListWidget(theParent)
  {
  }

  /// Redefinition of virtual method
  virtual QSize	sizeHint() const
  {
    int aHeight = 2 * QFontMetrics(font()).height();
    QSize aSize = QListWidget::sizeHint();
    return QSize(aSize.width(), aHeight);
  }

  /// Redefinition of virtual method
  virtual QSize	minimumSizeHint() const
  {
    int aHeight = 4/*2*/ * QFontMetrics(font()).height();
    QSize aSize = QListWidget::minimumSizeHint();
    return QSize(aSize.width(), aHeight);
  }

signals:
  void activated();

protected:
  virtual void mouseReleaseEvent(QMouseEvent* e) {
    QListWidget::mouseReleaseEvent(e);
    emit activated();
  }

#ifndef WIN32
  // The code is necessary only for Linux because
  //it can not update viewport on widget resize
protected:
  void resizeEvent(QResizeEvent* theEvent)
  {
    QListWidget::resizeEvent(theEvent);
    QTimer::singleShot(5, viewport(), SLOT(repaint()));
  }
#endif
};


/**
* \ingroup GUI
* An extension of QListWidget to provide Undo/Redo functionality
*/
class MODULEBASE_EXPORT ModuleBase_ListView : public QObject
{
Q_OBJECT

public:
  /// Constructor
  ModuleBase_ListView(QWidget* theParent = 0, const QString& theObjectName = QString(),
    const QString& theToolTip = QString());
  /// Destructor
  virtual ~ModuleBase_ListView() {}

  /// Returns current control
  /// \return list view instance
  QListWidget* getControl() const { return myListControl; }

  /// Adds a new list widget item to the end of the list and connect it to the given index
  /// \param theTextValue value visualized in the view
  /// \param theIndex an item internal index
  void addItem(const QString& theTextValue, const int theIndex);

  /// Returns True if the control already contains an item with the given text
  /// \param theTextValue tex of item to find
  bool hasItem(const QString& theTextValue) const;

  /// Returns list of internal list view item indices
  /// \param theIndices an output container for indices
  void getSelectedIndices(std::set<int>& theIndices);

  /// Removes selected items from the list widget
  void removeSelectedItems();

  /// Remove items contain parameter indices
  /// \param theIndices an indices
  void removeItems(std::set<int>& theIndices);

  /// Set selected items if possible
  /// \param theIndices container of indices to be selected
  void restoreSelection(const QModelIndexList& theIndices);

  /// Update enable/disable state of context menu actions
  void updateActionsStatus();

protected slots:
  /// Slot for copy command in a list pop-up menu
  void onCopyItem();

  /// Slot is called on selection of list of selected items
  void onListSelection();


signals:
  /// Signal about delete action click
  void deleteActionClicked();

  void listActivated();

protected:
  QListWidget* myListControl; ///< List control

  QAction* myCopyAction; ///< A copy action for pop-up menu in a list control
  QAction* myDeleteAction; ///< A delete action for pop-up menu in a list control
};

#endif
