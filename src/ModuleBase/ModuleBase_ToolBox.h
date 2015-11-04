// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ToolBox.h
// Created:     10 August 2015
// Author:      Alexandre SOLOVYOV

#ifndef ModuleBase_ToolBox_H
#define ModuleBase_ToolBox_H

#include <ModuleBase.h>
#include <QFrame>

class QButtonGroup;
class QFrame;
class QHBoxLayout;
class QStackedWidget;

/**
 * \class ModuleBase_ToolBox
 * \ingroup GUI
 * \brief An extension of QFrame object
 */
class MODULEBASE_EXPORT ModuleBase_ToolBox : public QFrame
{
  Q_OBJECT

public:
  /// Constructor
  /// \param theParent a parent widget
  ModuleBase_ToolBox( QWidget* theParent );
  virtual ~ModuleBase_ToolBox();

  ///  Add a new item to the tool box
  /// \param thePage a widget of the new item
  /// \param theName a name of the item
  /// \param theIcon an icon of the item
  void addItem( QWidget* thePage, const QString& theName, const QPixmap& theIcon );

  /// \return number of items
  int count() const;

  /// \return index of current widget
  int currentIndex() const;

  /// Set current item
  /// \param theIdx an index
  void setCurrentIndex( const int theIdx);

signals:
  /// A signal which is emited on current item changed
  void currentChanged( int );

private slots:
  /// A slot called on button press
  void onButton( int );

private:
  QButtonGroup*   myButtonsGroup;
  QFrame*         myButtonsFrame;
  QHBoxLayout*    myButtonsLayout;
  QStackedWidget* myStack;
};
#endif
