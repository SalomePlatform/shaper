// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetFactory.h
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETFACTORY_H_
#define MODULEBASE_WIDGETFACTORY_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QString>
#include <QList>

class QObject;
class QWidget;
class Config_WidgetAPI;
class ModuleBase_IWorkshop;
class ModuleBase_PageBase;

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
  /// \param thePage a parent page
  void createWidget(ModuleBase_PageBase* thePage, bool alignToTop = true);

  /// Creates property panel content for the feature
  /// \param thePage a parent page
  /// \param theFeature a feature to fill the panel
  void createPanel(ModuleBase_PageBase* thePage, const FeaturePtr& theFeature);

  /// Creates one widget for property panel for the widget with given index
  /// \param theParent a parent widget
  /// \param theWidgetId a widget index
  void createWidget(ModuleBase_PageBase* thePage,
                    const std::string& theWidgetId);

  /// Returns list of model widgets
  QList<ModuleBase_ModelWidget*> getModelWidgets() const
  {
    return myModelWidgets;
  }

  /// Returns the value of the title key from XML definition of the attribute in the feature
  /// \param theAttributeId a value of a kind of the attribute under the feature
  /// \param theTitle the result title
  void getAttributeTitle(const std::string& theAttributeId,
                         std::string& theTitle);

  /// Returns the identifier of the first found attribute where greed field value is set and true
  /// \param theAttributeId an outpup parameter with  attribute
  void getGreedAttribute(std::string& theAttributeId);

protected:
  /// check if ModuleBase_Widget has expandable widgets in getControls
  bool hasExpandingControls(QWidget* theParent);

  /// Create page by its type
  /// \param theType a type
  /// \param theParent a parent widget
  ModuleBase_PageBase* createPageByType(const std::string& theType,
                                        QWidget* theParent);

  /// Create widget by its type
  /// \param theType a type
  /// \param theParent a parent widget
  ModuleBase_ModelWidget* createWidgetByType(const std::string& theType,
                                             QWidget* theParent = NULL);

  /// Convert STD string to QT string
  /// \param theStdString is STD string
  static QString qs(const std::string& theStdString);

  /// It updates internal config api to point in the structure to given id of widget
  /// The method is recusive and it stops when the found flag is true
  /// \param theWidgetId a widget id key value
  /// \param theFound a flag about found windget and recursive search should be stopped
  void moveToWidgetId(const std::string& theWidgetId, bool& theFound);

 private:
   /// API object for XML reading
  Config_WidgetAPI* myWidgetApi;

  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;

  /// List of created model widgets
  QList<ModuleBase_ModelWidget*> myModelWidgets;
};

#endif /* ModuleBase_WidgetFactory_H_ */
