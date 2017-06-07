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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_WidgetAction_H
#define ModuleBase_WidgetAction_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class Config_WidgetAPI;
class QWidget;
class QToolButton;

/**
* \ingroup GUI
* Implementation of widget for feature action (tool button)
*/ 
class MODULEBASE_EXPORT ModuleBase_WidgetAction : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetAction(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetAction();

  /// Do not accept focus, returns false
  virtual bool focusTo();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// \return Context for translation
  virtual std::string context() const {
    std::string aContext = myFeatureId;
    if(!aContext.empty() && !myActionID.empty()) {
      aContext += ":";
    }
    aContext += myActionID;

    return aContext;
  }

protected:
  /// Do nothing
  /// \return True in success
  virtual bool storeValueCustom();

  /// Do nothing
  virtual bool restoreValueCustom();

protected slots:
  /// Listens the button click and call the customAction function of the current feature
  void onActionClicked();

private:
  QToolButton* myButton; ///< action button
  std::string myActionID; ///< action index
};

#endif
