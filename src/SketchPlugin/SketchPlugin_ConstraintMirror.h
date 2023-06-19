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

#ifndef SketchPlugin_ConstraintMirror_H_
#define SketchPlugin_ConstraintMirror_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintMirror
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint mirroring a list of objects regarding to a given line
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() for mirror line and
 *  SketchPlugin_Constraint::ENTITY_B() for the list of objects
 *
 *  Also the constraint has attribute SketchPlugin_Constraint::ENTITY_C()
 *  which contains list of mirrored objects
 */
class SketchPlugin_ConstraintMirror : public SketchPlugin_ConstraintBase
{
 public:
  /// Mirror constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_MIRROR_ID("SketchConstraintMirror");
    return MY_CONSTRAINT_MIRROR_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintMirror::ID();
    return MY_KIND;
  }

  /// List of objects to be mirrored
  inline static const std::string& MIRROR_LIST_ID()
  {
    static const std::string MY_MIRROR_LIST_ID("ConstraintMirrorList");
    return MY_MIRROR_LIST_ID;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// removes all fields from this feature: results, data, etc
  SKETCHPLUGIN_EXPORT virtual void erase();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintMirror();
};

#endif
