// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2dFlyout.h
// Created:     14 Oct 2015
// Author:      Natalia ERMOLAEVA

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
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetPoint2DFlyout(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                        const Config_WidgetAPI* theData, 
                        const std::string& theParentId);
  /// Destructor
  virtual ~PartSet_WidgetPoint2DFlyout() {};

  /// Activates the editor control only in case if the mouse over the OCC window, otherwise
  /// set focus to the usual double value control
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

private:
  //! Returns workshop
  XGUI_Workshop* workshop() const;

private:
  bool myIsInternal; /// an XML internal state
};

#endif
