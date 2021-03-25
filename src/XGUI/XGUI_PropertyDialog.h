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

#ifndef XGUI_PropertyDialog_H
#define XGUI_PropertyDialog_H

#include "XGUI.h"

#include <QDialog>

class QGridLayout;
/**
* \ingroup GUI
* A dialog that contains property widget and accept/reject buttons.
*/
class XGUI_PropertyDialog : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a parent widget for the dialog
  XGUI_EXPORT XGUI_PropertyDialog(QWidget* theParent);

  XGUI_EXPORT virtual ~XGUI_PropertyDialog() {};

  /// Set content of the dialog
  /// \param theWidget a content widget
  void setContent(QWidget* theWidget);

private:
  QGridLayout* myLayout; /// grid layout where the first row is reserved for content widget,
  /// the second row contains dialog buttons
  QWidget* myContentWidget; /// content widget of the dialog
};

#endif
