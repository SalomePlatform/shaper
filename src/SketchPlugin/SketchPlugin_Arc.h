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

#ifndef SketchPlugin_Arc_H_
#define SketchPlugin_Arc_H_

#include "SketchPlugin.h"

#include "SketchPlugin_SketchEntity.h"



/**\class SketchPlugin_Arc
 * \ingroup Plugins
 * \brief Feature for creation of the new arc of circle in PartSet.
 * The visualization of this object is separated in two parts. The first one is an AIS object
 * calculated when there is non-initialized attributes of the arc. The second is a result and
 * it is calculated if all attributes are initialized.
 */
class SketchPlugin_Arc: public SketchPlugin_SketchEntity
{
 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchArc");
    return ID;
  }

  /// Central 2D point of the circle which contains the arc
  inline static const std::string& CENTER_ID()
  {
    static const std::string ID = "center_point";
    return ID;
  }

  /// Start 2D point of the arc
  inline static const std::string& START_ID()
  {
    static const std::string ID = "start_point";
    return ID;
  }

  /// End 2D point of the arc
  inline static const std::string& END_ID()
  {
    static const std::string ID = "end_point";
    return ID;
  }

  /// Arc radius.
  static const std::string& RADIUS_ID()
  {
    static const std::string ID("radius");
    return ID;
  }

  /// Arc angle.
  static const std::string& ANGLE_ID()
  {
    static const std::string ID("angle");
    return ID;
  }

  /// Reversed flag
  inline static const std::string& REVERSED_ID()
  {
    static const std::string ID("reversed");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Arc::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates an arc-shape
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Updates the "reversed" flag
  /// \param isReversed  whether the arc will be reversed
  void setReversed(bool isReversed);

  /// Returns \c true is the arc is reversed
  bool isReversed();

  /// Use plugin manager for features creation
  SketchPlugin_Arc();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// to define in which direction draw arc
  double myParamBefore;
};

#endif
