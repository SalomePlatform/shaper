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

#ifndef ModuleBase_WidgetPointInput_H
#define ModuleBase_WidgetPointInput_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetSelector.h"

class ModuleBase_ParamSpinBox;

class MODULEBASE_EXPORT ModuleBase_WidgetPointInput : public ModuleBase_WidgetSelector
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetPointInput(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData);

  /// Destructor
  virtual ~ModuleBase_WidgetPointInput();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return true; }

  /// Fills given container with selection modes if the widget has it
  /// \param [out] theModuleSelectionModes module additional modes, -1 means all default modes
  /// \param [out] theModes a container of modes
  virtual void selectionModes(int& theModuleSelectionModes, QIntList& theModes);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
    const bool theToValidate);

protected:
  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const;

  /// Returns true if the event is processed.
  virtual bool processEnter();

protected:
  ModuleBase_ParamSpinBox* myXSpin;
  ModuleBase_ParamSpinBox* myYSpin;
  ModuleBase_ParamSpinBox* myZSpin;

  FeaturePtr myXParam;
  FeaturePtr myYParam;
  FeaturePtr myZParam;

  double myDefaultValue[3];
};


#endif