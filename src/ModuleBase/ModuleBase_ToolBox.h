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

class MODULEBASE_EXPORT ModuleBase_ToolBox : public QFrame
{
  Q_OBJECT

public:
  ModuleBase_ToolBox( QWidget* theParent );
  virtual ~ModuleBase_ToolBox();

  void addItem( QWidget* thePage, const QString& theName, const QIcon& theIcon );
  int count() const;
  int currentIndex() const;
  void setCurrentIndex( const int );

signals:
  void currentChanged( int );

private slots:
  void onButton( int );

private:
  QButtonGroup*   myButtonsGroup;
  QFrame*         myButtonsFrame;
  QHBoxLayout*    myButtonsLayout;
  QStackedWidget* myStack;
};
#endif
