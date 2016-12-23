// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Frederic Pons (OpenCascade)

#include <libxml/parser.h>

#include "XAO_XaoExporter.hxx"
#include "XAO_Xao.hxx"
#include "XAO_Geometry.hxx"
#include "XAO_Group.hxx"
#include "XAO_Field.hxx"
#include "XAO_Step.hxx"
#include "XAO_XaoUtils.hxx"

namespace XAO
{
    const xmlChar* C_TAG_XAO = (xmlChar*)"XAO";
    const xmlChar* C_ATTR_XAO_AUTHOR = (xmlChar*)"author";
    const xmlChar* C_ATTR_XAO_VERSION = (xmlChar*)"version";

    const xmlChar* C_TAG_GEOMETRY = (xmlChar*)"geometry";
    const xmlChar* C_ATTR_GEOMETRY_NAME = (xmlChar*)"name";

    const xmlChar* C_TAG_SHAPE = (xmlChar*)"shape";
    const xmlChar* C_ATTR_SHAPE_FORMAT = (xmlChar*)"format";

    const xmlChar* C_TAG_TOPOLOGY = (xmlChar*)"topology";
    const xmlChar* C_TAG_VERTICES = (xmlChar*)"vertices";
    const xmlChar* C_TAG_VERTEX = (xmlChar*)"vertex";
    const xmlChar* C_TAG_EDGES = (xmlChar*)"edges";
    const xmlChar* C_TAG_EDGE = (xmlChar*)"edge";
    const xmlChar* C_TAG_FACES = (xmlChar*)"faces";
    const xmlChar* C_TAG_FACE = (xmlChar*)"face";
    const xmlChar* C_TAG_SOLIDS = (xmlChar*)"solids";
    const xmlChar* C_TAG_SOLID = (xmlChar*)"solid";
    const xmlChar* C_ATTR_COUNT = (xmlChar*)"count";
    const xmlChar* C_ATTR_ELT_INDEX = (xmlChar*)"index";
    const xmlChar* C_ATTR_ELT_NAME = (xmlChar*)"name";
    const xmlChar* C_ATTR_ELT_REFERENCE = (xmlChar*)"reference";

    const xmlChar* C_TAG_GROUPS = (xmlChar*)"groups";
    const xmlChar* C_TAG_GROUP = (xmlChar*)"group";
    const xmlChar* C_ATTR_GROUP_NAME = (xmlChar*)"name";
    const xmlChar* C_ATTR_GROUP_DIM = (xmlChar*)"dimension";

    const xmlChar* C_TAG_ELEMENT = (xmlChar*)"element";
    const xmlChar* C_ATTR_ELEMENT_INDEX = (xmlChar*)"index";
    const xmlChar* C_TAG_VALUE = (xmlChar*)"value";
    const xmlChar* C_ATTR_VALUE_COMPONENT = (xmlChar*)"component";

    const xmlChar* C_TAG_FIELDS = (xmlChar*)"fields";
    const xmlChar* C_TAG_FIELD = (xmlChar*)"field";
    const xmlChar* C_ATTR_FIELD_NAME = (xmlChar*)"name";
    const xmlChar* C_ATTR_FIELD_TYPE = (xmlChar*)"type";
    const xmlChar* C_ATTR_FIELD_DIMENSION = (xmlChar*)"dimension";
    const xmlChar* C_TAG_COMPONENTS = (xmlChar*)"components";
    const xmlChar* C_TAG_COMPONENT = (xmlChar*)"component";
    const xmlChar* C_ATTR_COMPONENT_COLUMN = (xmlChar*)"column";
    const xmlChar* C_ATTR_COMPONENT_NAME = (xmlChar*)"name";

    const xmlChar* C_TAG_STEPS = (xmlChar*)"steps";
    const xmlChar* C_TAG_STEP = (xmlChar*)"step";
    const xmlChar* C_ATTR_STEP_NUMBER = (xmlChar*)"number";
    const xmlChar* C_ATTR_STEP_STAMP = (xmlChar*)"stamp";
}

using namespace XAO;

namespace {
    xmlDocPtr exportXMLDoc(Xao* xaoObject);
    void exportGeometry(Geometry* xaoGeometry, xmlDocPtr doc, xmlNodePtr xao);
    void exportGeometricElements(Geometry* xaoGeometry, xmlNodePtr topology,
                                 XAO::Dimension dim, const xmlChar* colTag, const xmlChar* eltTag);
    void exportGroups(Xao* xaoObject, xmlNodePtr xao);
    void exportFields(Xao* xaoObject, xmlNodePtr xao);
    void exportStep(Step* step, Field* field, xmlNodePtr nodeSteps);

    void parseXMLDoc(xmlDocPtr doc, Xao* xaoObject);
    void parseXaoNode(xmlDocPtr doc, xmlNodePtr xaoNode, Xao* xaoObject);
    void parseGeometryNode(xmlDocPtr doc, xmlNodePtr geometryNode, Xao* xaoObject);
    void parseShapeNode(xmlDocPtr doc, xmlNodePtr shapeNode, Geometry* geometry);
    void parseTopologyNode(xmlNodePtr topologyNode, Geometry* geometry);
    void parseVerticesNode(xmlNodePtr verticesNode, Geometry* geometry);
    void parseEdgesNode(xmlNodePtr edgesNode, Geometry* geometry);
    void parseFacesNode(xmlNodePtr facesNode, Geometry* geometry);
    void parseSolidsNode(xmlNodePtr solidsNode, Geometry* geometry);
    void parseGroupsNode(xmlNodePtr groupsNode, Xao* xaoObject);
    void parseGroupNode(xmlNodePtr groupNode, Xao* xaoObject);

    void parseFieldsNode(xmlNodePtr fieldsNode, Xao* xaoObject);
    void parseFieldNode(xmlNodePtr fieldNode, Xao* xaoObject);
    void parseStepNode(xmlNodePtr stepNode, Field* field);
    void parseStepElementNode(xmlNodePtr eltNode, Step* step);

    std::string readStringProp(xmlNodePtr node, const xmlChar* attribute,
                               const bool& required, const std::string& defaultValue, const std::string& exception = std::string(""));
    int readIntegerProp(xmlNodePtr node, const xmlChar* attribute,
                        const bool& required, const int& defaultValue, const std::string& exception = std::string(""));

  std::string readStringProp(xmlNodePtr node, const xmlChar* attribute,
                             const bool& required, const std::string& defaultValue,
                             const std::string& exception /*= std::string() */)
  {
    xmlChar* strAttr = xmlGetProp(node, attribute);
    if (strAttr == NULL)
    {
        if (required)
        {
            if (exception.size() > 0)
                throw XAO_Exception(exception.c_str());

            throw XAO_Exception(MsgBuilder() << "Line " << node->line << ": "
                                             << "Property " << (char*)attribute << " is required.");
        }

        return defaultValue;
    }

    std::string res = (char*)strAttr;
    xmlFree(strAttr);
    return res;
  }

  int readIntegerProp(xmlNodePtr node, const xmlChar* attribute,
                      const bool& required, const int& defaultValue,
                      const std::string& exception /*= std::string() */)
  {
    xmlChar* strAttr = xmlGetProp(node, attribute);
    if (strAttr == NULL)
    {
        if (required)
        {
            if (exception.size() > 0)
                throw XAO_Exception(exception.c_str());

            throw XAO_Exception(MsgBuilder() << "Line " << node->line << ": "
                                             << "Property " << (char*)attribute << " is required.");
        }

        return defaultValue;
    }

    int res = XaoUtils::stringToInt((char*)strAttr);
    xmlFree(strAttr);
    return res;
  }

  xmlDocPtr exportXMLDoc(Xao* xaoObject)
  {
    // Creating the Xml document
    xmlDocPtr masterDocument = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr xao = xmlNewNode(0, C_TAG_XAO);
    xmlDocSetRootElement(masterDocument, xao);

    xmlNewProp(xao, C_ATTR_XAO_VERSION, BAD_CAST xaoObject->getVersion().c_str());
    xmlNewProp(xao, C_ATTR_XAO_AUTHOR, BAD_CAST xaoObject->getAuthor().c_str());

    if (xaoObject->getGeometry() != NULL)
    {
        exportGeometry(xaoObject->getGeometry(), masterDocument, xao);
    }

    exportGroups(xaoObject, xao);
    exportFields(xaoObject, xao);

    return masterDocument;
  }

  void exportGeometricElements(Geometry* xaoGeometry,
                               xmlNodePtr topology, XAO::Dimension dim, const xmlChar* colTag, const xmlChar* eltTag)
  {
    xmlNodePtr vertices = xmlNewChild(topology, 0, colTag, 0);
    xmlNewProp(vertices, C_ATTR_COUNT, BAD_CAST XaoUtils::intToString(xaoGeometry->countElements(dim)).c_str());
    GeometricElementList::iterator it = xaoGeometry->begin(dim);
    for (; it != xaoGeometry->end(dim); it++)
    {
        int index = it->first;
        GeometricElement elt = it->second;
        xmlNodePtr vertex = xmlNewChild(vertices, 0, eltTag, 0);
        xmlNewProp(vertex, C_ATTR_ELT_INDEX, BAD_CAST XaoUtils::intToString(index).c_str());
        xmlNewProp(vertex, C_ATTR_ELT_NAME, BAD_CAST elt.getName().c_str());
        xmlNewProp(vertex, C_ATTR_ELT_REFERENCE, BAD_CAST elt.getReference().c_str());
    }
  }

  void exportGeometry(Geometry* xaoGeometry, xmlDocPtr doc, xmlNodePtr xao)
  {
    // Geometric part
    xmlNodePtr geometry = xmlNewChild(xao, 0, C_TAG_GEOMETRY, 0);
    xmlNewProp(geometry, C_ATTR_GEOMETRY_NAME, BAD_CAST xaoGeometry->getName().c_str());

    xmlNodePtr shape = xmlNewChild(geometry, 0, C_TAG_SHAPE, 0);
    xmlNewProp(shape, C_ATTR_SHAPE_FORMAT, BAD_CAST XaoUtils::shapeFormatToString(xaoGeometry->getFormat()).c_str());
    std::string txtShape = xaoGeometry->getShapeString();
    xmlNodePtr cdata = xmlNewCDataBlock(doc, BAD_CAST txtShape.c_str(), txtShape.size());
    xmlAddChild(shape, cdata);

    xmlNodePtr topology = xmlNewChild(geometry, 0, C_TAG_TOPOLOGY, 0);

    exportGeometricElements(xaoGeometry, topology, XAO::VERTEX, C_TAG_VERTICES, C_TAG_VERTEX);
    exportGeometricElements(xaoGeometry, topology, XAO::EDGE, C_TAG_EDGES, C_TAG_EDGE);
    exportGeometricElements(xaoGeometry, topology, XAO::FACE, C_TAG_FACES, C_TAG_FACE);
    exportGeometricElements(xaoGeometry, topology, XAO::SOLID, C_TAG_SOLIDS, C_TAG_SOLID);
  }

  void exportGroups(Xao* xaoObject, xmlNodePtr xao)
  {
    xmlNodePtr groups = xmlNewChild(xao, 0, C_TAG_GROUPS, 0);
    xmlNewProp(groups, C_ATTR_COUNT, BAD_CAST XaoUtils::intToString(xaoObject->countGroups()).c_str());

    for (int i = 0; i < xaoObject->countGroups(); i++)
    {
        //Group* grp = (*it);
        Group* grp = xaoObject->getGroup(i);
        xmlNodePtr group = xmlNewChild(groups, 0, C_TAG_GROUP, 0);
        xmlNewProp(group, C_ATTR_GROUP_NAME, BAD_CAST grp->getName().c_str());
        xmlNewProp(group, C_ATTR_GROUP_DIM, BAD_CAST XaoUtils::dimensionToString(grp->getDimension()).c_str());
        xmlNewProp(group, C_ATTR_COUNT, BAD_CAST XaoUtils::intToString(grp->count()).c_str());

        for (std::set<int>::iterator it = grp->begin(); it != grp->end(); ++it)
        {
            int grpElt = (*it);
            xmlNodePtr elt = xmlNewChild(group, 0, C_TAG_ELEMENT, 0);
            xmlNewProp(elt, C_ATTR_ELEMENT_INDEX, BAD_CAST XaoUtils::intToString(grpElt).c_str());
        }
    }
  }

  void exportFields(Xao* xaoObject, xmlNodePtr xao)
  {
    xmlNodePtr fields = xmlNewChild(xao, 0, C_TAG_FIELDS, 0);
    xmlNewProp(fields, C_ATTR_COUNT, BAD_CAST XaoUtils::intToString(xaoObject->countFields()).c_str());

    for (int i = 0; i < xaoObject->countFields(); i++)
    {
        Field* field = xaoObject->getField(i);
        xmlNodePtr nodeField = xmlNewChild(fields, 0, C_TAG_FIELD, 0);
        xmlNewProp(nodeField, C_ATTR_FIELD_NAME, BAD_CAST field->getName().c_str());
        xmlNewProp(nodeField, C_ATTR_FIELD_TYPE, BAD_CAST XaoUtils::fieldTypeToString(field->getType()).c_str());
        xmlNewProp(nodeField, C_ATTR_FIELD_DIMENSION, BAD_CAST XaoUtils::dimensionToString(field->getDimension()).c_str());

        int nbComponents = field->countComponents();
        xmlNodePtr components = xmlNewChild(nodeField, 0, C_TAG_COMPONENTS, 0);
        xmlNewProp(components, C_ATTR_COUNT, BAD_CAST XaoUtils::intToString(nbComponents).c_str());

        for (int j = 0; j < nbComponents; j++)
        {
            xmlNodePtr nodeComponent = xmlNewChild(components, 0, C_TAG_COMPONENT, 0);
            xmlNewProp(nodeComponent, C_ATTR_COMPONENT_COLUMN, BAD_CAST XaoUtils::intToString(j).c_str());
            xmlNewProp(nodeComponent, C_ATTR_COMPONENT_NAME, BAD_CAST field->getComponentName(j).c_str());
        }

        int nbSteps = field->countSteps();
        xmlNodePtr nodeSteps = xmlNewChild(nodeField, 0, C_TAG_STEPS, 0);
        xmlNewProp(nodeSteps, C_ATTR_COUNT, BAD_CAST XaoUtils::intToString(nbSteps).c_str());
        for (stepIterator itStep = field->begin(); itStep != field->end(); itStep++)
        {
            Step* step = *itStep;
            exportStep(step, field, nodeSteps);
        }
    }
  }

  void exportStep(Step* step, Field* field, xmlNodePtr nodeSteps)
  {
    xmlNodePtr nodeStep = xmlNewChild(nodeSteps, 0, C_TAG_STEP, 0);
    xmlNewProp(nodeStep, C_ATTR_STEP_NUMBER, BAD_CAST XaoUtils::intToString(step->getStep()).c_str());
    if (step->getStamp() >= 0)
    {
        xmlNewProp(nodeStep, C_ATTR_STEP_STAMP, BAD_CAST XaoUtils::intToString(step->getStamp()).c_str());
    }

    for(int i = 0; i < step->countElements(); ++i)
    {
        xmlNodePtr nodeElt = xmlNewChild(nodeStep, 0, C_TAG_ELEMENT, 0);
        xmlNewProp(nodeElt, C_ATTR_ELEMENT_INDEX, BAD_CAST XaoUtils::intToString(i).c_str());

        for (int j = 0; j < step->countComponents(); ++j)
        {
            std::string content = step->getStringValue(i, j);
            xmlNodePtr nodeValue = xmlNewChild(nodeElt, NULL, C_TAG_VALUE, BAD_CAST content.c_str());
            xmlNewProp(nodeValue, C_ATTR_VALUE_COMPONENT, BAD_CAST XaoUtils::intToString(j).c_str());
        }
    }
  }

  void parseXMLDoc(xmlDocPtr doc, Xao* xaoObject)
  {
    // Get the root element node
    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (xmlStrcmp(root->name , C_TAG_XAO) != 0)
        throw XAO_Exception("Cannot read XAO file: invalid format XAO node not found");

    parseXaoNode(doc, root, xaoObject);

    xmlFreeDoc(doc);       // free document
    //xmlCleanupParser();    // free globals //vsr: xmlCleanupParser should not be called from the application
  }

  void parseXaoNode(xmlDocPtr doc, xmlNodePtr xaoNode, Xao* xaoObject)
  {
    std::string version = readStringProp(xaoNode, C_ATTR_XAO_VERSION, false, "");
    if (version != "")
        xaoObject->setVersion(version);

    std::string author = readStringProp(xaoNode, C_ATTR_XAO_AUTHOR, false, "");
    xaoObject->setAuthor(author);

    for (xmlNodePtr node = xaoNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_GEOMETRY) == 0)
            parseGeometryNode(doc, node, xaoObject);
        else if (xmlStrcmp(node->name, C_TAG_GROUPS) == 0)
            parseGroupsNode(node, xaoObject);
        else if (xmlStrcmp(node->name, C_TAG_FIELDS) == 0)
            parseFieldsNode(node, xaoObject);
    }
  }

  void parseGeometryNode(xmlDocPtr doc, xmlNodePtr geometryNode, Xao* xaoObject)
  {
    // get the shape and topo nodes
    xmlNodePtr shapeNode = NULL;
    xmlNodePtr topoNode = NULL;
    for (xmlNodePtr node = geometryNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_SHAPE) == 0)
            shapeNode = node;
        else if (xmlStrcmp(node->name, C_TAG_TOPOLOGY) == 0)
            topoNode = node;
    }

    std::string name = readStringProp(geometryNode, C_ATTR_GEOMETRY_NAME, false, "");
    std::string strFormat = readStringProp(shapeNode, C_ATTR_SHAPE_FORMAT, true, "");
    XAO::Format shapeFormat = XaoUtils::stringToShapeFormat(strFormat);
    Geometry* geometry = Geometry::createGeometry(shapeFormat, name);

    parseShapeNode(doc, shapeNode, geometry);
    parseTopologyNode(topoNode, geometry);

    xaoObject->setGeometry(geometry);
  }

  void parseShapeNode(xmlDocPtr doc, xmlNodePtr shapeNode, Geometry* geometry)
  {
    if (geometry->getFormat() == XAO::BREP)
    {
        xmlChar* data = xmlNodeGetContent(shapeNode->children);
        if (data == NULL)
            throw XAO_Exception("Missing BREP");
        geometry->setShapeString((char*)data);
        xmlFree(data);
    }
    else
    {
        throw XAO_Exception(MsgBuilder() << "Shape format not supported: "
                                         << XaoUtils::shapeFormatToString(geometry->getFormat()));
    }
  }

  void parseTopologyNode(xmlNodePtr topologyNode, Geometry* geometry)
  {
    for (xmlNodePtr node = topologyNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_VERTICES) == 0)
            parseVerticesNode(node, geometry);
        else if (xmlStrcmp(node->name, C_TAG_EDGES) == 0)
            parseEdgesNode(node, geometry);
        else if (xmlStrcmp(node->name, C_TAG_FACES) == 0)
            parseFacesNode(node, geometry);
        else if (xmlStrcmp(node->name, C_TAG_SOLIDS) == 0)
            parseSolidsNode(node, geometry);
    }
  }

  void parseVerticesNode(xmlNodePtr verticesNode, Geometry* geometry)
  {
    int count = readIntegerProp(verticesNode, C_ATTR_COUNT, true, -1);
    geometry->setCountVertices(count);

    for (xmlNodePtr node = verticesNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_VERTEX) == 0)
        {
            int index = readIntegerProp(node, C_ATTR_ELT_INDEX, true, -1);
            std::string name = readStringProp(node, C_ATTR_ELT_NAME, false, "");
            std::string reference = readStringProp(node, C_ATTR_ELT_REFERENCE, true, "");

            geometry->setVertex(index, name, reference);
        }
    }
  }

  void parseEdgesNode(xmlNodePtr edgesNode, Geometry* geometry)
  {
    int count = readIntegerProp(edgesNode, C_ATTR_COUNT, true, -1);
    geometry->setCountEdges(count);

    for (xmlNodePtr node = edgesNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_EDGE) == 0)
        {
            int index = readIntegerProp(node, C_ATTR_ELT_INDEX, true, -1);
            std::string name = readStringProp(node, C_ATTR_ELT_NAME, false, "");
            std::string reference = readStringProp(node, C_ATTR_ELT_REFERENCE, true, "");

            geometry->setEdge(index, name, reference);
        }
    }
  }

  void parseFacesNode(xmlNodePtr facesNode, Geometry* geometry)
  {
    int count = readIntegerProp(facesNode, C_ATTR_COUNT, true, -1);
    geometry->setCountFaces(count);

    for (xmlNodePtr node = facesNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_FACE) == 0)
        {
            int index = readIntegerProp(node, C_ATTR_ELT_INDEX, true, -1);
            std::string name = readStringProp(node, C_ATTR_ELT_NAME, false, "");
            std::string reference = readStringProp(node, C_ATTR_ELT_REFERENCE, true, "");

            geometry->setFace(index, name, reference);
        }
    }
  }

  void parseSolidsNode(xmlNodePtr solidsNode, Geometry* geometry)
  {
    int count = readIntegerProp(solidsNode, C_ATTR_COUNT, true, -1);
    geometry->setCountSolids(count);

    for (xmlNodePtr node = solidsNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_SOLID) == 0)
        {
            int index = readIntegerProp(node, C_ATTR_ELT_INDEX, true, -1);
            std::string name = readStringProp(node, C_ATTR_ELT_NAME, false, "");
            std::string reference = readStringProp(node, C_ATTR_ELT_REFERENCE, true, "");

            geometry->setSolid(index, name, reference);
        }
    }
  }

  void parseGroupsNode(xmlNodePtr groupsNode, Xao* xaoObject)
  {
    for (xmlNodePtr node = groupsNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_GROUP) == 0)
        {
            parseGroupNode(node, xaoObject);
        }
    }
  }

  void parseGroupNode(xmlNodePtr groupNode, Xao* xaoObject)
  {
    std::string strDimension = readStringProp(groupNode, C_ATTR_GROUP_DIM, true, "");
    XAO::Dimension dim = XaoUtils::stringToDimension(strDimension);
    Group* group = xaoObject->addGroup(dim);

    std::string name = readStringProp(groupNode, C_ATTR_GROUP_NAME, false, "");
    group->setName(name);

    for (xmlNodePtr node = groupNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_ELEMENT) == 0)
        {
            int index = readIntegerProp(node, C_ATTR_ELEMENT_INDEX, true, -1);
            group->add(index);
        }
    }
  }

  void parseFieldsNode(xmlNodePtr fieldsNode, Xao* xaoObject)
  {
    for (xmlNodePtr node = fieldsNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_FIELD) == 0)
        {
            parseFieldNode(node, xaoObject);
        }
    }
  }

  void parseFieldNode(xmlNodePtr fieldNode, Xao* xaoObject)
  {
    std::string strDimension = readStringProp(fieldNode, C_ATTR_FIELD_DIMENSION, true, "");
    XAO::Dimension dim = XaoUtils::stringToDimension(strDimension);

    std::string strType = readStringProp(fieldNode, C_ATTR_FIELD_TYPE, true, "");
    XAO::Type type = XaoUtils::stringToFieldType(strType);

    // we need to get the number of components first to create the field
    xmlNodePtr componentsNode = NULL;
    xmlNodePtr stepsNode = NULL;

    for (xmlNodePtr node = fieldNode->children; node; node = node->next)
    {
        if (xmlStrcmp(node->name, C_TAG_COMPONENTS) == 0)
            componentsNode = node;
        else if (xmlStrcmp(node->name, C_TAG_STEPS) == 0)
            stepsNode = node;
    }

    // ensure that the components node is defined
    if (componentsNode == NULL)
    {
        throw XAO_Exception(MsgBuilder() << "Line " << fieldNode->line << ": "
                                         << "No components defined for field.");
    }

    // create the field
    int nbComponents = readIntegerProp(componentsNode, C_ATTR_COUNT, true, -1);
    Field* field = xaoObject->addField(type, dim, nbComponents);

    // parse the components
    for (xmlNodePtr compNode = componentsNode->children; compNode; compNode = compNode->next)
    {
        std::string compName= readStringProp(compNode, C_ATTR_COMPONENT_NAME, false, "");
        if (compName.size() > 0)
        {
            int col = readIntegerProp(compNode, C_ATTR_COMPONENT_COLUMN, true, -1);
            field->setComponentName(col, compName);
        }
    }

    // set the name
    std::string name = readStringProp(fieldNode, C_ATTR_FIELD_NAME, false, "");
    if (name.size() > 0) field->setName(name);

    // read the steps
    if (stepsNode != 0)
    {
        for (xmlNodePtr stepNode = stepsNode->children; stepNode; stepNode = stepNode->next)
        {
            if (xmlStrcmp(stepNode->name, C_TAG_STEP) == 0)
            {
                parseStepNode(stepNode, field);
            }
        }
    }
  }

  void parseStepNode(xmlNodePtr stepNode, Field* field)
  {
    int stepNumber = readIntegerProp(stepNode, C_ATTR_STEP_NUMBER, true, -1);
    Step* step = field->addNewStep(stepNumber);

    int stepStamp = readIntegerProp(stepNode, C_ATTR_STEP_STAMP, false, -1);
    if (stepStamp != -1)
    {
        step->setStamp(stepStamp);
    }

    for (xmlNodePtr eltNode = stepNode->children; eltNode; eltNode = eltNode->next)
    {
        if (xmlStrcmp(eltNode->name, C_TAG_ELEMENT) == 0)
        {
            parseStepElementNode(eltNode, step);
        }
    }
  }

  void parseStepElementNode(xmlNodePtr eltNode, Step* step)
  {
    int index = readIntegerProp(eltNode, C_ATTR_ELT_INDEX, true, -1);

    for (xmlNodePtr valNode = eltNode->children; valNode; valNode = valNode->next)
    {
        if (xmlStrcmp(valNode->name, C_TAG_VALUE) == 0)
        {
            int component = readIntegerProp(valNode, C_ATTR_VALUE_COMPONENT, true, -1);
            xmlChar* data = xmlNodeGetContent(valNode->children);

            std::string value;
            if (data != NULL)
            {
                value = (char*)data;
            }
            else if (step->getType() != XAO::STRING)
            {
                throw XAO_Exception(MsgBuilder() << "Line " << valNode->line << ": no content for value.");
            }

            step->setStringValue(index, component, value);
        }
    }
  }
}

const bool XaoExporter::saveToFile(Xao* xaoObject, const std::string& fileName)
throw (XAO_Exception)
{
    xmlDocPtr doc = exportXMLDoc(xaoObject);
    xmlSaveFormatFileEnc(fileName.c_str(), doc, "UTF-8", 1); // format = 1 for node indentation
    xmlFreeDoc(doc);

    return true;
}

const std::string XaoExporter::saveToXml(Xao* xaoObject)
throw (XAO_Exception)
{
    xmlDocPtr doc = exportXMLDoc(xaoObject);

    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1); // format = 1 for node indentation
    xmlFreeDoc(doc);
    xmlCleanupGlobals();

    return (char*)xmlbuff;
}

const bool XaoExporter::readFromFile(const std::string& fileName, Xao* xaoObject)
throw (XAO_Exception)
{
    // parse the file and get the DOM
    int options = XML_PARSE_HUGE | XML_PARSE_NOCDATA;
    xmlDocPtr doc = xmlReadFile(fileName.c_str(), NULL, options);
    if (doc == NULL)
    {
        throw XAO_Exception("Cannot read XAO file");
    }

    parseXMLDoc(doc, xaoObject);
    return true;
}

const bool XaoExporter::setXML(const std::string& xml, Xao* xaoObject)
throw (XAO_Exception)
{
    int options = XML_PARSE_HUGE | XML_PARSE_NOCDATA;
    xmlDocPtr doc = xmlReadDoc(BAD_CAST xml.c_str(), "", NULL, options);
    if (doc == NULL)
    {
        throw XAO_Exception("Cannot read XAO stream");
    }

    parseXMLDoc(doc, xaoObject);
    return true;
}
