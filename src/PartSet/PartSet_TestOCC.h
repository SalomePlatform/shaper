// File:        PartSet_TestOCC.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_TestOCC_H
#define PartSet_TestOCC_H

#include "PartSet.h"

class XGUI_Workshop;
class Handle_AIS_InteractiveContext;
class Handle_V3d_View;

/*!
 \class PartSet_TestOCC
 * \brief The operation to test OCC viewer work
*/
class PARTSET_EXPORT PartSet_TestOCC
{
public:
  PartSet_TestOCC() {};

  /// Creates a line feature, select it, modify the line. Check whether the highlight is correct.
  /// \param the workshop to provide an access to the application information
  static void testSelection(XGUI_Workshop* theWorkshop);

  /// Creates row OCC presentation, activate in the local context, modify it and check selection
  /// BUG: The result contains the selection from the first presentation
  /// \param theContext a viewer interactive context
  /// \param theContext a view
  static void testSelection_OCC(Handle_AIS_InteractiveContext theContext,
                                Handle_V3d_View theView);

  /// Creates row OCC presentation, activate in the local context, select it, erase the presentation
  /// BUG: The selected shape are in the viewer even after erase
  /// \param theContext a viewer interactive context
  /// \param theContext a view
  static void testErase_OCC(Handle_AIS_InteractiveContext theContext,
                            Handle_V3d_View theView);

private:

  /// Creates a temporary line
  /// \param the workshop to get the operation manager and the displayer
  static void createTestLine(XGUI_Workshop* theWorkshop);

  /// Changes a temporary line
  /// \param the workshop to get the operation manager and the displayer
  static void changeTestLine(XGUI_Workshop* theWorkshop);

  /// Emulates the mouse moving in the viewer
  static void moveMouse(Handle_AIS_InteractiveContext theContext, Handle_V3d_View theView);
};

#endif
