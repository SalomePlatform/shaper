// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "BuildAPI_Filling.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

BuildAPI_Filling::BuildAPI_Filling(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

BuildAPI_Filling::BuildAPI_Filling(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theBaseObjects)
  : ModelHighAPI_Interface(theFeature)
{
  if(initialize())
    setBase(theBaseObjects);
}

BuildAPI_Filling::BuildAPI_Filling(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                   const std::string& theOrientCorrection,
                                   const int theMinDegree,
                                   const int theMaxDegree,
                                   const int theNbIter,
                                   const double theTolerance2D,
                                   const double theTolerance3D,
                                   const bool theApproximate)
  : ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setOrientationMethod(theOrientCorrection);
    setMinDegree(theMinDegree);
    setMaxDegree(theMaxDegree);
    setNbIterations(theNbIter);
    setTolerance2d(theTolerance2D);
    setTolerance3d(theTolerance3D);
    setApproximation(theApproximate);
    setAdvancedOptions();
    setBase(theBaseObjects);
  }
}

BuildAPI_Filling::~BuildAPI_Filling()
{
}

void BuildAPI_Filling::execIfBaseNotEmpty()
{
  if (baseObjects()->size() > 0)
    execute();
}

void BuildAPI_Filling::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setOrientationMethod(const std::string& theMethod)
{
  fillAttribute(theMethod, myorientationMethod);
  if (theMethod != BuildPlugin_Filling::METHOD_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setMinDegree(const int theMinDegree)
{
  fillAttribute(theMinDegree, myminDegree);
  if (theMinDegree != BuildPlugin_Filling::MINIMAL_DEGREE_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setMaxDegree(const int theMaxDegree)
{
  fillAttribute(theMaxDegree, mymaxDegree);
  if (theMaxDegree != BuildPlugin_Filling::MAXIMAL_DEGREE_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setNbIterations(const int theNbIter)
{
  fillAttribute(theNbIter, mynbIterations);
  if (theNbIter != BuildPlugin_Filling::NUMBER_OF_ITERATIONS_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setTolerance2d(const double theTol2d)
{
  fillAttribute(theTol2d, mytolerance2d);
  if (theTol2d != BuildPlugin_Filling::TOLERANCE_2D_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setTolerance3d(const double theTol3d)
{
  fillAttribute(theTol3d, mytolerance3d);
  if (theTol3d != BuildPlugin_Filling::TOLERANCE_3D_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setApproximation(const bool theApproximate)
{
  fillAttribute(theApproximate, myapproximate);
  if (theApproximate != BuildPlugin_Filling::APPROXIMATION_DEFAULT())
    setAdvancedOptions();
  execIfBaseNotEmpty();
}

void BuildAPI_Filling::setAdvancedOptions()
{
  feature()->string(BuildPlugin_Filling::ADVANCED_OPTIONS_ID())->setValue("true");
}

void BuildAPI_Filling::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addFilling(" << aPartName << ", " << baseObjects();

  if (!aBase->string(BuildPlugin_Filling::ADVANCED_OPTIONS_ID())->value().empty()) {
    // dump options too,
    theDumper << ", " << orientationMethod()
              << ", " << minDegree() << ", " << maxDegree() << ", " << nbIterations()
              << ", " << tolerance2d() << ", " << tolerance3d()
              << ", " << approximate();
  }
  theDumper << ")" << std::endl;
}

//==================================================================================================

FillingPtr addFilling(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Filling::ID());
  return FillingPtr(new BuildAPI_Filling(aFeature, theBaseObjects));
}

FillingPtr addFilling(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const std::list<ModelHighAPI_Selection>& theBaseObjects,
                      const std::string& theOrientCorrection,
                      const int theMinDegree,
                      const int theMaxDegree,
                      const int theNbIter,
                      const double theTolerance2D,
                      const double theTolerance3D,
                      const bool theApproximate)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Filling::ID());
  return FillingPtr(new BuildAPI_Filling(aFeature, theBaseObjects, theOrientCorrection,
      theMinDegree, theMaxDegree, theNbIter, theTolerance2D, theTolerance3D, theApproximate));
}
