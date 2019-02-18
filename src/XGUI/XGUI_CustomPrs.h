// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef XGUI_CustomPrs_H
#define XGUI_CustomPrs_H

#include "XGUI.h"
#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Result.h>

class XGUI_Workshop;

/**
* Interface of a class which can provide specific customization of
* object presentation
*/ 
class XGUI_CustomPrs : public GeomAPI_ICustomPrs
{
public:
  /// Constructor
  /// \param theWorkshop the current workshop instance
  XGUI_EXPORT XGUI_CustomPrs(XGUI_Workshop* theWorkshop);

  XGUI_EXPORT virtual ~XGUI_CustomPrs() {};

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs);

  /// Returns color of a result object
  /// \param theResult a result object
  /// \param theColor a color in form of RGB vector
  static void getResultColor(const ResultPtr& theResult, std::vector<int>& theColor);

  /// Returns deflection of a result object
  /// \param theResult a result object
  /// \return theDeflection a real value
  static double getResultDeflection(const ResultPtr& theResult);

  /// Returns transparency of a result object
  /// \param theResult a result object
  /// \return theTransparency a real value
  static double getResultTransparency(const ResultPtr& theResult);

  /// Returns the default object color. It obtains colorConfigInfo of the object
  /// and find it in preferences. If there are no this color in preference and an empty
  /// color is interpreted as invalid, it shows error message
  /// \param theObject an investigated object
  /// \param isEmptyColorValid boolean state about interpretation of empty color
  /// \param theColor the result color
  static void XGUI_EXPORT getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
                                          std::vector<int>& theColor);

  /// Returns the default deflection value. The value is obtained from the application preferences
  /// \param theObject an investigated object
  /// \return deflection value
  static double XGUI_EXPORT getDefaultDeflection(const ObjectPtr& theObject);

protected:
  XGUI_Workshop* myWorkshop; ///< the current workshop
};

#endif
