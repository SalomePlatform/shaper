// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef SketchPlugin_ConstraintMiddle_H_
#define SketchPlugin_ConstraintMiddle_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

class ModelAPI_Feature;

/** \class SketchPlugin_ConstraintMiddle
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which places a point in the middle of a line
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintMiddle : public SketchPlugin_ConstraintBase
{
 public:
  /// Parallel constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_MIDDLE_ID("SketchConstraintMiddle");
    return MY_CONSTRAINT_MIDDLE_ID;
  }

  /// Middle constraint kind
  inline static const std::string& MIDDLE_TYPE()
  {
    static const std::string MY_TYPE_ID("middle_type");
    return MY_TYPE_ID;
  }

  /// Middle constraint type by line and point
  inline static const std::string& MIDDLE_TYPE_BY_LINE_AND_POINT()
  {
    static const std::string MY_TYPE_ID("middle_type_by_line_and_point");
    return MY_TYPE_ID;
  }

  /// Middle constraint type by line
  inline static const std::string& MIDDLE_TYPE_BY_LINE()
  {
    static const std::string MY_TYPE_ID("middle_type_by_line");
    return MY_TYPE_ID;
  }

  /// Created points for middle type by line
  inline static const std::string& POINT_REF_ID()
  {
    static const std::string MY_POINT_REF("point");
    return MY_POINT_REF;
  }

  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintMiddle::ID();
    return MY_KIND;
  }

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintMiddle();

private:
  void CreatePoint();

  std::shared_ptr<ModelAPI_Feature> myPoint;
};

#endif
