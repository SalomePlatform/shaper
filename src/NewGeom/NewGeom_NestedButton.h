/*
 * NewGeom_NestedButton.h
 *
 *  Created on: Apr 13, 2015
 *      Author: sbh
 */

#ifndef SRC_NEWGEOM_NEWGEOM_NESTEDBUTTON_H_
#define SRC_NEWGEOM_NEWGEOM_NESTEDBUTTON_H_

#include <QWidgetAction>

class QFrame;
class QAction;
class QWidget;
class QToolButton;

/*!
 * \ingroup Salome
 * Custom (nested) button in salome mode.
 */
class NewGeom_NestedButton : public QWidgetAction
{
  Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent objects
   /// \param theNestedActions a list of nested actions
  NewGeom_NestedButton(QObject* theParent, const QList<QAction*>& theNestedActions);
  virtual ~NewGeom_NestedButton();

 private slots:
  /// Shows/hides the additional buttons widget
  void showAdditionalButtons(bool);

  /// Slot called on action state
  void actionStateChanged();

 protected:
  /// Creates the button representation
  /// \param theParent a parent widget
  virtual QWidget * createWidget(QWidget * theParent);
  
  virtual bool event(QEvent* theEvent);

 private:
  QList<QAction*> myNestedActions; ///< list of nested actions
  QWidget* myAdditionalButtonsWidget; ///< widget to precess additional buttons visibility
  QFrame* myButtonFrame; ///< frame arround button representation
  QToolButton* myThisButton; ///< main button
};

#endif /* SRC_NEWGEOM_NEWGEOM_NESTEDBUTTON_H_ */
