// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetFactory.h
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_WidgetFactory_H_
#define ModuleBase_WidgetFactory_H_

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

#include <QString>
#include <QList>

class QObject;
class QWidget;
class Config_WidgetAPI;
class ModuleBase_IWorkshop;

/**
* \ingroup GUI
* A class for creation of widgets instances in for property panel using XML deskription of 
* a feature
*/
class MODULEBASE_EXPORT ModuleBase_WidgetFactory
{
 public:
   /// Constructor
   /// \param theXmlRepresentation content of XML file
   /// \param theWorkshop reference to workshop instance
  ModuleBase_WidgetFactory(const std::string& theXmlRepresentation,
                           ModuleBase_IWorkshop* theWorkshop);
  virtual ~ModuleBase_WidgetFactory();

  /// Creates content widget for property panel
  /// \param theParent a parent widget
  void createWidget(QWidget* theParent);


  /// Returns list of model widgets
  QList<ModuleBase_ModelWidget*> getModelWidgets() const
  {
    return myModelWidgets;
  }

 protected:
  /// check if ModuleBase_Widget has expandable widgets in getControls
  bool hasExpandingControls(QWidget* theParent);
   /// Create widget by its type
   /// \param theType a type
   /// \param theParent a parent widget
  QWidget* createWidgetByType(const std::string& theType, QWidget* theParent = NULL);

   /// Create a widget of container type
   /// \param theType a type
   /// \param theParent a parent widget
  QWidget* createContainer(const std::string& theType, QWidget* theParent);

  /// Create label widget
  /// \param theParent a parent widget
  QWidget* labelControl(QWidget* theParent);

  /// Create double spin box widget
  /// \param theParent a parent widget
  QWidget* doubleSpinBoxControl(QWidget* theParent);

  /// Create double value editor widget
  /// \param theParent a parent widget
  QWidget* doubleValueEditor(QWidget* theParent);

  /// Create shape selector widget
  /// \param theParent a parent widget
  QWidget* shapeSelectorControl(QWidget* theParent);

  /// Create boolean input widget
  /// \param theParent a parent widget
  QWidget* booleanControl(QWidget* theParent);

  /// Create file selector widget
  /// \param theParent a parent widget
  QWidget* fileSelectorControl(QWidget* theParent);

  /// Create choice widget (combo box)
  /// \param theParent a parent widget
  QWidget* choiceControl(QWidget* theParent);

  /// Create line edit widget
  /// \param theParent a parent widget
  QWidget* lineEditControl(QWidget* theParent);

  /// Create multi selector widget
  /// \param theParent a parent widget
  QWidget* multiSelectorControl(QWidget* theParent);

  /// Convert STD string to QT string
  /// \param theStdString is STD string
  static QString qs(const std::string& theStdString);

 private:
   /// API object for XML reading
  Config_WidgetAPI* myWidgetApi;

  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;

  /// List of created model widgets
  QList<ModuleBase_ModelWidget*> myModelWidgets;

  /// Id of current parent
  std::string myParentId;
};

#endif /* ModuleBase_WidgetFactory_H_ */
