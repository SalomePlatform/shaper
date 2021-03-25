// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_Fillet_H_
#define FeaturesAPI_Fillet_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Fillet.h>
#include <FeaturesPlugin_Fillet1D.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class FeaturesAPI_Fillet
/// \ingroup CPPHighAPI
/// \brief Interface for Fillet feature.
class FeaturesAPI_Fillet: public ModelHighAPI_Interface
{
public:
  /// Destructor.
  virtual ~FeaturesAPI_Fillet() {}

  virtual std::shared_ptr<ModelAPI_AttributeDouble> radius() const = 0;

  /// Modify base objects of the fillet.
  virtual void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects) = 0;

  /// Modify fillet to have fixed radius
  virtual void setRadius(const ModelHighAPI_Double& theRadius) = 0;

protected:
  FeaturesAPI_Fillet(const std::shared_ptr<ModelAPI_Feature>& theFeature);
};

/// Pointer on the fillet object.
typedef std::shared_ptr<FeaturesAPI_Fillet> FilletPtr;


/// \class FeaturesAPI_Fillet1D
/// \ingroup CPPHighAPI
/// \brief Interface for Fillet1D feature - fillet on vertices of a wire.
class FeaturesAPI_Fillet1D : public FeaturesAPI_Fillet
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Fillet1D(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Fillet1D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theRadius);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Fillet1D();

  INTERFACE_4(FeaturesPlugin_Fillet1D::ID(),
              creationMethod, FeaturesPlugin_Fillet1D::CREATION_METHOD(),
                              ModelAPI_AttributeString,
                              /** Creation method */,
              baseWires, FeaturesPlugin_Fillet1D::WIRE_LIST_ID(),
                           ModelAPI_AttributeSelectionList,
                           /** Base objects */,
              baseVertices, FeaturesPlugin_Fillet1D::VERTEX_LIST_ID(),
                            ModelAPI_AttributeSelectionList,
                            /** Base objects */,
              radius, FeaturesPlugin_Fillet1D::RADIUS_ID(),
                      ModelAPI_AttributeDouble,
                      /** Value of the fixed radius fillet */)

  /// Modify base objects of the fillet.
  FEATURESAPI_EXPORT
  virtual void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify fillet to have fixed radius
  FEATURESAPI_EXPORT
  virtual void setRadius(const ModelHighAPI_Double& theRadius);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
};


/// \class FeaturesAPI_Fillet2D
/// \ingroup CPPHighAPI
/// \brief Interface for Fillet feature - fillet edges on a solid.
class FeaturesAPI_Fillet2D : public FeaturesAPI_Fillet
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theRadius);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                const ModelHighAPI_Double& theRadius1,
                                const ModelHighAPI_Double& theRadius2);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Fillet2D();

  INTERFACE_5(FeaturesPlugin_Fillet::ID(),
              creationMethod, FeaturesPlugin_Fillet::CREATION_METHOD(),
                              ModelAPI_AttributeString,
                              /** Creation method */,
              baseObjects, FeaturesPlugin_Fillet::OBJECT_LIST_ID(),
                           ModelAPI_AttributeSelectionList,
                           /** Base objects */,
              radius, FeaturesPlugin_Fillet::RADIUS_ID(),
                      ModelAPI_AttributeDouble,
                      /** Value of the fixed radius fillet */,
              startRadius, FeaturesPlugin_Fillet::START_RADIUS_ID(),
                           ModelAPI_AttributeDouble,
                           /** Start radius of the varying radius fillet */,
              endRadius, FeaturesPlugin_Fillet::END_RADIUS_ID(),
                         ModelAPI_AttributeDouble,
                         /** End radius of the varying radius fillet */)

  /// Modify base objects of the fillet.
  FEATURESAPI_EXPORT
  virtual void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify fillet to have fixed radius
  FEATURESAPI_EXPORT
  virtual void setRadius(const ModelHighAPI_Double& theRadius);

  /// Modify fillet to have varying radius
  FEATURESAPI_EXPORT
  void setRadius(const ModelHighAPI_Double& theRadius1, const ModelHighAPI_Double& theRadius2);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
};


/// \ingroup CPPHighAPI
/// \brief Create Fillet feature.
FEATURESAPI_EXPORT
FilletPtr addFillet(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const ModelHighAPI_Double& theRadius1,
                    const ModelHighAPI_Double& theRadius2 = ModelHighAPI_Double(-1.0),
                    const bool keepSubResults = false);

#endif // FeaturesAPI_Fillet_H_
