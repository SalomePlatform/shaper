// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_IErrorMgr_H
#define ModuleBase_IErrorMgr_H

#include "ModuleBase.h"

#include <QObject>

#include <memory>

class ModuleBase_IPropertyPanel;
class ModelAPI_Feature;

/**
 * \class ModuleBase_IErrorMgr
 * \ingroup GUI
 * \brief An interface to Errors manager object
 */
class MODULEBASE_EXPORT ModuleBase_IErrorMgr : public QObject
{
  Q_OBJECT
public:
  /// Default constructor
  /// \param theParent a parent object
  ModuleBase_IErrorMgr(QObject* theParent = 0);
  /// Virtual destructor
  virtual ~ModuleBase_IErrorMgr();

  /// \brief Set property pane to the operation
  /// \param theProp a property panel instance
  virtual void setPropertyPanel(ModuleBase_IPropertyPanel* theProp);

  /// \return Currently installed property panel
  ModuleBase_IPropertyPanel* propertyPanel() const { return myPropertyPanel; }

  /// Update actions for the given feature
  /// \param theFeature a feature
  virtual void updateActions(const std::shared_ptr<ModelAPI_Feature>& theFeature) = 0;

protected slots:
  /// Process values changed event for processing feature attribute validation errors.
  virtual void onWidgetChanged() = 0;

protected:
  /// Access to property panel
  ModuleBase_IPropertyPanel* myPropertyPanel;
};

#endif // ModuleBase_IErrorMgr_H