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

#ifndef ConstructionPlugin_Validators_H_
#define ConstructionPlugin_Validators_H_

#include <ModelAPI_AttributeValidator.h>

/// \class ConstructionPlugin_ValidatorPointLines
/// \ingroup Validators
/// \brief A validator for selection lines for point by intersection.
class ConstructionPlugin_ValidatorPointLines: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorPointEdgeAndPlaneNotParallel
/// \ingroup Validators
/// \brief A validator for selection edge and plane for point by intersection.
class ConstructionPlugin_ValidatorPointEdgeAndPlaneNotParallel: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorPlaneThreePoints
/// \ingroup Validators
/// \brief A validator for selection three points for plane.
class ConstructionPlugin_ValidatorPlaneThreePoints: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorPlaneLinePoint
/// \ingroup Validators
/// \brief A validator for selection line and point for plane.
class ConstructionPlugin_ValidatorPlaneLinePoint: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorPlaneTwoParallelPlanes
/// \ingroup Validators
/// \brief A validator for selection two parallel planes.
class ConstructionPlugin_ValidatorPlaneTwoParallelPlanes: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes
/// \ingroup Validators
/// \brief A validator for selection two parallel planes.
class ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
   virtual bool isValid(const AttributePtr& theAttribute,
                        const std::list<std::string>& theArguments,
                        Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorPointThreeNonParallelPlanes
/// \ingroup Validators
/// \brief A validator for selection three non parallel planes.
class ConstructionPlugin_ValidatorPointThreeNonParallelPlanes: public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/// \class ConstructionPlugin_ValidatorNotFeature
/// \ingroup Validators
/// \brief A validator for selection of a result but not a feature.
class ConstructionPlugin_ValidatorNotFeature : public ModelAPI_AttributeValidator
{
public:
  //! \return True if the attribute is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

#endif