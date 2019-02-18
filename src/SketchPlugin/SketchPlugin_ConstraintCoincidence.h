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

#ifndef SketchPlugin_ConstraintCoincidence_H_
#define SketchPlugin_ConstraintCoincidence_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include <SketchPlugin_Sketch.h>
#include <list>

class GeomDataAPI_Point2D;

/** \class SketchPlugin_ConstraintCoincidence
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines equivalence of two points
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintCoincidence : public SketchPlugin_ConstraintBase
{
 public:
  /// Coincidence constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_COINCIDENCE_ID("SketchConstraintCoincidence");
    return MY_CONSTRAINT_COINCIDENCE_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintCoincidence::ID();
    return MY_KIND;
  }

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a constraint on two points
  /// \param theSketch a sketch feature
  /// \param thePoint1 the first point
  /// \param thePoint2 the second point
  static void createCoincidenceFeature(SketchPlugin_Sketch* theSketch,
                                const std::shared_ptr<GeomDataAPI_Point2D>& thePoint1,
                                const std::shared_ptr<GeomDataAPI_Point2D>& thePoint2);

  /// Returns point of coincidence feature
  /// \param theFeature a coincidence feature
  /// \return point 2d attribute. Coincidence always has at least one point 2d attribute
  static std::shared_ptr<GeomDataAPI_Point2D> getPoint(const FeaturePtr& theFeature);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintCoincidence();
};

#endif
