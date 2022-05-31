// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "BuildAPI_Interpolation.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const FeaturePtr& theFeature,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const ModelHighAPI_Selection& theStartTangent,
                    const ModelHighAPI_Selection& theEndTangent,
                    const bool theIsClosed,
                    const bool theIsToReorder)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(BuildPlugin_Interpolation::CREATION_METHOD_BY_SELECTION_ID(),mycreationmethod);
    setUseTangents(true);
    setTangents(theStartTangent, theEndTangent);
    setClosed(theIsClosed);
    setReorder(theIsToReorder);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const FeaturePtr& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const bool theIsClosed,
  const bool theIsToReorder)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(BuildPlugin_Interpolation::CREATION_METHOD_BY_SELECTION_ID(),mycreationmethod);
    setClosed(theIsClosed);
    setReorder(theIsToReorder);
    setUseTangents(false);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const FeaturePtr& theFeature,
                                               const std::string & theXTexpression,
                                               const std::string & theYTexpression,
                                               const std::string & theZTexpression,
                                               const ModelHighAPI_Double& theMinT,
                                               const ModelHighAPI_Double& theMaxT,
                                               const ModelHighAPI_Integer& theNbStep)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(BuildPlugin_Interpolation::CREATION_METHOD_ANALYTICAL_ID(),mycreationmethod);
    fillAttribute(theXTexpression, myxt);
    fillAttribute(theYTexpression, myyt);
    fillAttribute(theZTexpression, myzt);
    fillAttribute(theMinT, mymint);
    fillAttribute(theMaxT, mymaxt);
    fillAttribute(theNbStep, mynumstep);

    execute();
  }
}

//==================================================================================================
BuildAPI_Interpolation::~BuildAPI_Interpolation()
{
}

//==================================================================================================
void BuildAPI_Interpolation::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void BuildAPI_Interpolation::setClosed(const bool theIsClosed)
{
  fillAttribute(theIsClosed, myclosed);

  execIfBaseNotEmpty();
}

void BuildAPI_Interpolation::setReorder(const bool theIsToReorder)
{
  fillAttribute(theIsToReorder, myreorder);

  execIfBaseNotEmpty();
}

void BuildAPI_Interpolation::setUseTangents(const bool theIsToUseTangents)
{
  fillAttribute(theIsToUseTangents ? "true" : "", myuseTangents);

  execIfBaseNotEmpty();
}

void BuildAPI_Interpolation::setTangents(const ModelHighAPI_Selection& theStartTangent,
                                         const ModelHighAPI_Selection& theEndTangent)
{
  fillAttribute(theStartTangent, mystartTangent);
  fillAttribute(theEndTangent, myendTangent);

  execIfBaseNotEmpty();
}

//==================================================================================================
void BuildAPI_Interpolation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  if (aBase->string(BuildPlugin_Interpolation::CREATION_METHOD_ID())->value() ==
            BuildPlugin_Interpolation::CREATION_METHOD_BY_SELECTION_ID())
  {
    AttributeSelectionListPtr anAttrBaseObjects =
      aBase->selectionList(BuildPlugin_Interpolation::BASE_OBJECTS_ID());

    theDumper << aBase << " = model.addInterpolation(" << aPartName << ", "
              << anAttrBaseObjects << ", ";

    AttributeStringPtr useTangentsAttr = useTangents();
    std::string useTangents = useTangentsAttr->value();
    if (!useTangents.empty()) {
      AttributeSelectionPtr anAttrStartTangent =
        aBase->selection(BuildPlugin_Interpolation::TANGENT_START_ID());
      AttributeSelectionPtr anAttrEndTangent =
        aBase->selection(BuildPlugin_Interpolation::TANGENT_END_ID());

      theDumper << anAttrStartTangent << ", " << anAttrEndTangent << ", ";
    }

    theDumper << closed() << ", " << reorder() << ")" << std::endl;
  } else {
    theDumper << aBase << " = model.addInterpolation(" << aPartName ;
    AttributeStringPtr XtAttr = xt();
    std::string xt = XtAttr->value();
    AttributeStringPtr YtAttr = yt();
    std::string yt = YtAttr->value();
    AttributeStringPtr ZtAttr = zt();
    std::string zt = ZtAttr->value();
    AttributeDoublePtr minTAttr = mint();
    double mint = minTAttr->value();
    AttributeDoublePtr maxTAttr = maxt();
    double maxt = maxTAttr->value();
    AttributeIntegerPtr nbStepAttr = numstep();
    int nbStep = nbStepAttr->value();

    theDumper<< ", \""  << xt << "\", \"" << yt << "\", \""<< zt<< "\", " ;
    theDumper << mint << ", " << maxt << ", "<< nbStep<< ")"<<std::endl;
  }
}

//==================================================================================================
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const bool theIsClosed,
                                  const bool theIsToReorder)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Interpolation::ID());
  return InterpolationPtr(new BuildAPI_Interpolation(aFeature,
                                                     theBaseObjects,
                                                     theIsClosed,
                                                     theIsToReorder));
}

//==================================================================================================
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theStartTangent,
                                  const ModelHighAPI_Selection& theEndTangent,
                                  const bool theIsClosed,
                                  const bool theIsToReorder)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Interpolation::ID());
  return InterpolationPtr(new BuildAPI_Interpolation(aFeature,
                                                     theBaseObjects,
                                                     theStartTangent,
                                                     theEndTangent,
                                                     theIsClosed,
                                                     theIsToReorder));
}

InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::string & theXTexpression,
                                  const std::string & theYTexpression,
                                  const std::string & theZTexpression,
                                  const ModelHighAPI_Double& theMinT,
                                  const ModelHighAPI_Double& theMaxT,
                                  const ModelHighAPI_Integer& theNbStep)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Interpolation::ID());
  return InterpolationPtr(new BuildAPI_Interpolation(aFeature,
                                                     theXTexpression,
                                                     theYTexpression,
                                                     theZTexpression,
                                                     theMinT,
                                                     theMaxT,
                                                     theNbStep));
}

//==================================================================================================
void BuildAPI_Interpolation::execIfBaseNotEmpty()
{
  if (baseObjects()->size() > 0)
    execute();
}
