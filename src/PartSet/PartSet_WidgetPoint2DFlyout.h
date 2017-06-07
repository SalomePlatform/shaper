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

#ifndef PartSet_WidgetPoint2DFlyout_H
#define PartSet_WidgetPoint2DFlyout_H

#include "PartSet.h"
#include <PartSet_WidgetPoint2d.h>

class XGUI_Workshop;

/**\class PartSet_WidgetPoint2DFlyout
 * \ingroup Modules
 * \brief Implementation of usual point 2d widget with a condition that it can not accept the focus
 * when the AIS presentation is not visualized in the viewer.
 */
class PARTSET_EXPORT PartSet_WidgetPoint2DFlyout : public PartSet_WidgetPoint2D
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetPoint2DFlyout(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                              const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~PartSet_WidgetPoint2DFlyout() {};

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped widget values
  /// \param theToValidate a validation flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Checks if the selection presentation is valid in widget
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Activates the editor control only in case if the mouse over the OCC window, otherwise
  /// set focus to the usual double value control
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// Returns true if the attribute can be changed using the selected shapes in the viewer
  /// and creating a coincidence constraint to them. This control does not use them.
  virtual bool useSelectedShapes() const;

private:
  //! Returns workshop
  XGUI_Workshop* workshop() const;
};

#endif
