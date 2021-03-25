// Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_ConstraintCoincidenceInternal_H_
#define SketchPlugin_ConstraintCoincidenceInternal_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintCoincidence.h"

/** \class SketchPlugin_ConstraintCoincidenceInternal
 *  \ingroup Plugins
 *  \brief Internal coincidence constraint not applicable for the end user
 */
class SketchPlugin_ConstraintCoincidenceInternal : public SketchPlugin_ConstraintCoincidence
{
  public:
  /// \brief Coincidence constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_COINCIDENCE_ID("SketchConstraintCoincidenceInternal");
    return MY_CONSTRAINT_COINCIDENCE_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintCoincidenceInternal::ID();
    return MY_KIND;
  }

  /// \brief Index of point in the array if the first attribute is an array
  inline static const std::string& INDEX_ENTITY_A()
  {
    static const std::string MY_INDEX("ConstraintEntityA_Index");
    return MY_INDEX;
  }
  /// \brief Index of point in the array if the second attribute is an array
  inline static const std::string& INDEX_ENTITY_B()
  {
    static const std::string MY_INDEX("ConstraintEntityB_Index");
    return MY_INDEX;
  }

  /// \brief Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintCoincidenceInternal();
};

#endif
