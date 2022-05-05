// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef MODULEBASE_WIDGETCONCEALEDOBJECTS_H_
#define MODULEBASE_WIDGETCONCEALEDOBJECTS_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <string>
#include <map>
#include <memory>

class ModelAPI_Object;

class QWidget;
class QTableWidget;

/**
* \ingroup GUI
* Implementation of model widget for table of concealed objects.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetConcealedObjects : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration.
  ModuleBase_WidgetConcealedObjects(QWidget* theParent,
                                    const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetConcealedObjects();

  /// Redefinition of virtual method
  virtual QList<QWidget*> getControls() const;

  /// The widget can not accept focus
  virtual bool canAcceptFocus() const { return false; };

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();

private:
  /// Creates a row in view
  /// \param theResult a result for the row
  void addViewRow(const std::shared_ptr<ModelAPI_Result>& theResult);

private slots:
  /// Processes the display button click in the view.
  /// \param theState a new state
  void onItemToggled(bool theState);

private:
  std::string myBaseShapeAttribute; ///< attribute of the base object
  bool myPickConcealedParents; ///< select parent compsolid/compounds of concealed objects
  QTableWidget* myView; ///< table view of visibility states

  std::shared_ptr<ModelAPI_Feature> myBaseFeature; ///< backup of feature of filling the table view
  /// backup of visualized in the view concealed objects, stored to avoid table rebuild by restore
  std::map<int, std::shared_ptr<ModelAPI_Result> > myConcealedResults;
};

#endif /* MODULEBASE_WIDGETCONCEALEDOBJECTS_H_ */
