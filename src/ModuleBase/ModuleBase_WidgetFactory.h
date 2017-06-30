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
  /// \param alignToTop align to top flag (true by default)
  void createWidget(ModuleBase_PageBase* thePage, bool alignToTop = true);

  /// Creates property panel content for the feature
  /// \param thePage a parent page
  /// \param theFeature a feature to fill the panel
  void createPanel(ModuleBase_PageBase* thePage, const FeaturePtr& theFeature);

  /// Creates one widget for property panel for the widget with given index
  /// \param thePage a parent widget
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

  /// Returns widget API from XML
  const Config_WidgetAPI* widgetAPI() const { return myWidgetApi; }

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
