// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_ConstraintLength_H_
#define SketchPlugin_ConstraintLength_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include "SketchPlugin_Sketch.h"

#include <GeomAPI_ICustomPrs.h>

#include <list>

class GeomAPI_Pnt2d;
class GeomDataAPI_Point2D;

/** \class SketchPlugin_ConstraintLength
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines a length of a line segment
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::VALUE() (length) and SketchPlugin_Constraint::ENTITY_A() (segment),
 *  SketchPlugin_Constraint::FLYOUT_VALUE_PNT() (distance of a constraints handle)
 */
class SketchPlugin_ConstraintLength : public SketchPlugin_ConstraintBase
{
 public:
  /// Length constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_LENGTH_ID("SketchConstraintLength");
    return MY_CONSTRAINT_LENGTH_ID;
  }

  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintLength::ID();
    return MY_KIND;
  }

  /// attribute name of dimension location type
  inline static const std::string& LOCATION_TYPE_ID()
  {
    static const std::string MY_LOCATION_TYPE_ID("LocationType");
    return MY_LOCATION_TYPE_ID;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Computes the attribute value on the base of other attributes if the value can be computed
  /// \param theAttributeId an attribute index to be computed
  /// \return a boolean value about it is computed
  SKETCHPLUGIN_EXPORT virtual bool compute(const std::string& theAttributeId);

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Retuns the parameters of color definition in the resources config manager
  SKETCHPLUGIN_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                                   std::string& theDefault);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintLength();

  /// Computes distance between lenght point attributes
  /// \param theValue [out] distance or 0 if one of point attributes is not initialized
  /// \return boolean value if distance is computed
  bool computeLenghtValue(double& theValue);

private:
  /// retrns the points-base of length, returns false if it is not possible
  bool getPoints(
    std::shared_ptr<GeomAPI_Pnt>& thePoint1, std::shared_ptr<GeomAPI_Pnt>& thePoint2,
    std::shared_ptr<GeomDataAPI_Point2D>& theStartPoint,
    std::shared_ptr<GeomDataAPI_Point2D>& theEndPoint);

private:
  bool myFlyoutUpdate; ///< to avoid cyclic dependencies on automatic updates of flyout point
};

#endif
