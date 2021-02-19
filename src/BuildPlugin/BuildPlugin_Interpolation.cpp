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

#include "BuildPlugin_Interpolation.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Events_InfoMessage.h>

#include <Locale_Convert.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_CurveBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeExplorer.h>

#include <algorithm>
#include <sstream>

//=================================================================================================
BuildPlugin_Interpolation::BuildPlugin_Interpolation()
{
}

//=================================================================================================
void BuildPlugin_Interpolation::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(CLOSED_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(REORDER_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(USE_TANGENTS_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TANGENT_START_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TANGENT_END_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(CREATION_METHOD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(CREATION_METHOD_BY_SELECTION_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(CREATION_METHOD_ANALYTICAL_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EXPRESSION_ERROR_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(VARIABLE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(VALUE_ID(), ModelAPI_AttributeTables::typeId());
  data()->string(EXPRESSION_ERROR_ID())->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXPRESSION_ERROR_ID());

  data()->addAttribute(XT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(YT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ZT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(MINT_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MAXT_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(NUMSTEP_ID(), ModelAPI_AttributeInteger::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
                                                 CREATION_METHOD_ANALYTICAL_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
                                                 CREATION_METHOD_BY_SELECTION_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VARIABLE_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VALUE_ID());
  data()->addAttribute(ARGUMENTS_ID(), ModelAPI_AttributeRefList::typeId());
  data()->reflist(ARGUMENTS_ID())->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ARGUMENTS_ID());

  if (string(XT_ID())->value() == ""
     && string(YT_ID())->value() == ""
     && string(ZT_ID())->value() == "") {
      string(XT_ID())->setValue("t");
      string(YT_ID())->setValue("t");
      string(ZT_ID())->setValue("t");
      real(MINT_ID())->setValue(0);
      real(MAXT_ID())->setValue(100);
      integer(NUMSTEP_ID())->setValue(10);
      updateCoordinates();
  }
}

//=================================================================================================
void BuildPlugin_Interpolation::attributeChanged(const std::string& theID)
{
  if ((theID == XT_ID()
    || theID == YT_ID()
    || theID == ZT_ID()
    || theID == MINT_ID()
    || theID == MAXT_ID()
    || theID == NUMSTEP_ID())
    && string(XT_ID())->value() !=""
    && string(YT_ID())->value() !=""
    && string(ZT_ID())->value() !=""
    && string(CREATION_METHOD_ID())->value() == CREATION_METHOD_ANALYTICAL_ID()) {
    updateCoordinates();
  }
}

//=================================================================================================
void BuildPlugin_Interpolation::updateCoordinates()
{
    std::wstring exp;
    double aMint = real(MINT_ID())->value();
    double aMaxt = real(MAXT_ID())->value();
    int aNbrStep = integer(NUMSTEP_ID())->value();

    if (aMaxt < aMint) {
      setError("The minimum value of the parameter must be less than maximum value !!!");
    }

    double aScale = (aMaxt - aMint)/aNbrStep;
    string(VARIABLE_ID())->setValue("t");

    tables(VALUE_ID())->setSize(aNbrStep+1,4);
    for (int step = 0; step <= aNbrStep; step++) {
      ModelAPI_AttributeTables::Value aVal;
      aVal.myDouble = step * aScale + aMint;
      tables(VALUE_ID())->setValue(aVal,step,0);
    }

    outErrorMessage="";
    evaluate(outErrorMessage);
    data()->string(EXPRESSION_ERROR_ID())->setValue(outErrorMessage);
    if (!outErrorMessage.empty()) {
      setError("Error: Python interpreter ");
      return;
    }
}

//=================================================================================================
static GeomDirPtr selectionToDir(const AttributeSelectionPtr& theSelection)
{
  GeomDirPtr aDir;
  GeomEdgePtr anEdge;

  GeomShapePtr aShape = theSelection->value();
  if (!aShape && theSelection->context()) {
    aShape = theSelection->context()->shape();
  }

  if (aShape && aShape->isEdge()) {
    anEdge = GeomEdgePtr(new GeomAPI_Edge(aShape));
  }

  if (anEdge && anEdge->isLine()) {
    aDir = anEdge->line()->direction();
  }

  return aDir;
}

//=================================================================================================
void BuildPlugin_Interpolation::execute()
{
  if (string(CREATION_METHOD_ID())->value() == CREATION_METHOD_BY_SELECTION_ID()) {
    // Get closed flag value
    bool isClosed = boolean(CLOSED_ID())->value();

    // Get reorder flag value
    bool isToReorder = boolean(REORDER_ID())->value();

    // Get use tangents flag value
    bool isToUseTangents = isClosed? false : (!string(USE_TANGENTS_ID())->value().empty());

    // Get tangent for start and end points
    GeomDirPtr aDirStart, aDirEnd;
    if (isToUseTangents) {
      aDirStart = selectionToDir(selection(TANGENT_START_ID()));
      aDirEnd = selectionToDir(selection(TANGENT_END_ID()));
    }

    // Get base objects list.
    AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());

    // Collect points.
    std::list<GeomPointPtr> aPoints;
    std::set<GeomShapePtr> aContexts;
    for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
      AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);

      GeomShapePtr aContextShape = aSelection->context()->shape();
      aContexts.insert(aContextShape);

      GeomShapePtr aShape = aSelection->value();
      if (!aShape.get()) {
        aShape = aContextShape;
      }

      GeomPointPtr aPoint = GeomAlgoAPI_PointBuilder::point(aShape);
      aPoints.push_back(aPoint);
    }

    // Create curve from points
    GeomEdgePtr anEdge =
      GeomAlgoAPI_CurveBuilder::edge(aPoints, isClosed, isToReorder, aDirStart, aDirEnd);
    if (!anEdge.get()) {
      setError("Error: Result curve is empty.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data());
    std::set<GeomShapePtr>::const_iterator aContextIt = aContexts.begin();
    for (; aContextIt != aContexts.end(); aContextIt++) {
      aResultBody->storeModified(*aContextIt, anEdge, aContextIt == aContexts.begin());
    }
    int aVertexIndex = 1;
    for (GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
      std::string aVertexName = "Vertex_" + std::to_string((long long)aVertexIndex);
      aResultBody->generated(anExp.current(), aVertexName);
    }

    setResult(aResultBody);

  } else {
    if (string(XT_ID())->value() == ""
        ||string(YT_ID())->value() == ""
        ||string(ZT_ID())->value() == ""
        ||tables(VALUE_ID())->rows()== 0)
      return;

    bool aWasBlocked = data()->blockSendAttributeUpdated(true);
    updateCoordinates();
    data()->blockSendAttributeUpdated(aWasBlocked, false);

    AttributeTablesPtr aTable = tables(VALUE_ID());
    std::list<std::vector<double>> aCoordPoints;
    for (int step = 0; step < aTable->rows(); step++) {
      std::vector<double> aCoordPoint;
      ModelAPI_AttributeTables::Value aValue;
      //x
      aValue = aTable->value(step, 1);
      aCoordPoint.push_back(aValue.myDouble);
      //y
      aValue = aTable->value(step, 2);
      aCoordPoint.push_back(aValue.myDouble);
      //
      aValue = aTable->value(step, 3);
      aCoordPoint.push_back(aValue.myDouble);

      aCoordPoints.push_back(aCoordPoint);
    }

    std::list<GeomPointPtr> aPoints;
    std::list<std::vector<double>>::const_iterator anItCoordPoints = aCoordPoints.begin();

    for (; anItCoordPoints!=aCoordPoints.end(); ++anItCoordPoints) {

      GeomVertexPtr aVertex = GeomAlgoAPI_PointBuilder::vertex((*anItCoordPoints)[0],
                                                               (*anItCoordPoints)[1],
                                                               (*anItCoordPoints)[2]);
      aPoints.push_back(aVertex->point());
    }

    // test if some points are identical
    std::list<GeomPointPtr>::const_iterator anItPoint1 = aPoints.begin();
    std::list<GeomPointPtr>::const_iterator anItPoint2;
    for(; anItPoint1 != aPoints.end(); ++ anItPoint1) {
      anItPoint2 = anItPoint1;
      ++anItPoint2;
      for(; anItPoint2 != aPoints.end(); ++ anItPoint2)
          if ((*anItPoint2)->isEqual(*anItPoint1)) {
            setError("Error: Several points are identical");
            return;
          }
    }

    // Create curve from points
    GeomEdgePtr anEdge =
      GeomAlgoAPI_CurveBuilder::edge(aPoints, false, false,GeomDirPtr(),GeomDirPtr());
    if (!anEdge.get()) {
      setError("Error: Result curve is empty.");
      return;
    }

    ResultBodyPtr aResultBody = document()->createBody(data());
    // Load the result
    aResultBody->store(anEdge);
    int aVertexIndex = 1;
    for (GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
      std::string aVertexName = "Vertex_" + std::to_string((long long)aVertexIndex);
      aResultBody->generated(anExp.current(), aVertexName);
      aVertexIndex++;
    }
    setResult(aResultBody);
  }
}

//=================================================================================================
void BuildPlugin_Interpolation::evaluate(std::string& theError)
{
  FeaturePtr aMyPtr = std::dynamic_pointer_cast<ModelAPI_Feature>(data()->owner());
  std::shared_ptr<ModelAPI_BuildEvalMessage> aProcessMessage =
    ModelAPI_BuildEvalMessage::send(aMyPtr, this);

  if (aProcessMessage->isProcessed()) {
    theError = aProcessMessage->error();

    const std::list<ResultParameterPtr>& aParamsList = aProcessMessage->params();
    //store the list of parameters to store if changed
    AttributeRefListPtr aParams = reflist(ARGUMENTS_ID());
    aParams->clear();
    std::list<ResultParameterPtr>::const_iterator aNewIter = aParamsList.begin();
    for (; aNewIter != aParamsList.end(); aNewIter++) {
      aParams->append(*aNewIter);
    }
  } else { // error: python interpreter is not active
    theError = "Python interpreter is not available";
  }
}