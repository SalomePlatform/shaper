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

#include <cassert>

#include <Standard_TypeMismatch.hxx>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopAbs.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

#include "XAO_BrepGeometry.hxx"
#include "XAO_XaoUtils.hxx"

using namespace XAO;

BrepGeometry::BrepGeometry() : Geometry("")
{
}

BrepGeometry::BrepGeometry(const std::string& name) : Geometry(name)
{
}

const std::string BrepGeometry::getShapeString()
{
    std::ostringstream streamShape;
    BRepTools::Write(m_shape, streamShape);
    return streamShape.str();
}

void BrepGeometry::setShapeString(const std::string& shape)
{
    std::istringstream streamBrep(shape.c_str());
    BRep_Builder builder;
    BRepTools::Read(m_shape, streamBrep, builder);

    initIds();
}

TopoDS_Shape BrepGeometry::getTopoDS_Shape()
{
    return m_shape;
}

void BrepGeometry::setTopoDS_Shape(const TopoDS_Shape& shape)
{
    m_shape = shape;
    initIds();
}

void BrepGeometry::initIds()
{
    // intialization of Ids
    initListIds(TopAbs_VERTEX, m_vertices);
    initListIds(TopAbs_EDGE, m_edges);
    initListIds(TopAbs_FACE, m_faces);
    initListIds(TopAbs_SOLID, m_solids);
}

void BrepGeometry::initListIds(const TopAbs_ShapeEnum& shapeType, GeometricElementList& eltList)
{
    TopTools_MapOfShape mapShape;
    TopTools_ListOfShape listShape;

    int nbElt = 0;
    TopExp_Explorer exp(m_shape, shapeType);
    for (; exp.More(); exp.Next())
    {
        if (mapShape.Add(exp.Current()))
        {
            listShape.Append(exp.Current());
            nbElt++;
        }
    }

    if (listShape.IsEmpty())
        return;

    TopTools_IndexedMapOfShape indices;
    TopExp::MapShapes(m_shape, indices);

    eltList.setSize(nbElt);
    TopTools_ListIteratorOfListOfShape itSub(listShape);
    for (int index = 0; itSub.More(); itSub.Next(), ++index)
    {
        TopoDS_Shape value = itSub.Value();
        int ref = indices.FindIndex(value);
        eltList.setReference(index, XaoUtils::intToString(ref));
    }
}

TopoDS_Shape BrepGeometry::getSubShape(const TopoDS_Shape& mainShape, const TopAbs_ShapeEnum& shapeType, const int& shapeIndex)
throw (XAO_Exception)
{
    TopTools_MapOfShape mapShape;
    TopTools_ListOfShape listShape;

    TopExp_Explorer exp(mainShape, shapeType);
    for (; exp.More(); exp.Next())
    {
        if (mapShape.Add(exp.Current()))
            listShape.Append(exp.Current());
    }

    if (!listShape.IsEmpty())
    {
        TopTools_ListIteratorOfListOfShape itSub(listShape);
        for (int index = 0; itSub.More(); itSub.Next(), ++index)
        {
            if (shapeIndex == index)
            {
                TopoDS_Shape value = itSub.Value();
                return value;
            }
        }
    }

    throw XAO_Exception(MsgBuilder() << "Shape with reference [" << shapeIndex << "]  not found.");
}

// -----------------------------
const int BrepGeometry::countGeometricalElements(const TopoDS_Shape& shape, const TopAbs_ShapeEnum& shapeType)
{
    int res = 0;
    TopExp_Explorer exp(shape, shapeType);
    for (; exp.More(); exp.Next())
        res++;

    return res;
}

std::vector<int> BrepGeometry::getGeometricalElements(const TopoDS_Shape& shape, const TopAbs_ShapeEnum& shapeType, const XAO::Dimension& dim)
{
    std::vector<int> indexList;

    TopTools_MapOfShape mapShape;
    TopTools_ListOfShape listShape;

    TopExp_Explorer exp(shape, shapeType);
    for (; exp.More(); exp.Next())
    {
        if (mapShape.Add(exp.Current()))
            listShape.Append(exp.Current());
    }

    if (!listShape.IsEmpty())
    {
        // use the shape of the geometry for the indices
        TopTools_IndexedMapOfShape indices;
        TopExp::MapShapes(m_shape, indices);

        TopTools_ListIteratorOfListOfShape itSub(listShape);
        for (int index = 0; itSub.More(); itSub.Next(), ++index)
        {
            TopoDS_Shape value = itSub.Value();
            int id = indices.FindIndex(value);
            indexList.push_back(findElement(dim, id));
        }
    }

    return indexList;
}

void BrepGeometry::getEdgeVertices(const int& edgeIndex, int& vertexA, int& vertexB)
{
    TopoDS_Shape edge = getSubShape(m_shape, TopAbs_EDGE, edgeIndex);
    std::vector<int> vertices = getGeometricalElements(edge, TopAbs_VERTEX, XAO::VERTEX);
    assert(vertices.size() == 2);

    vertexA = vertices[0];
    vertexB = vertices[1];
}

const int BrepGeometry::countFaceWires(const int& faceIndex)
{
    TopoDS_Shape face = getSubShape(m_shape, TopAbs_FACE, faceIndex);
    return countGeometricalElements(face, TopAbs_WIRE);
}

std::vector<int> BrepGeometry::getFaceEdges(const int& faceIndex, const int& wireIndex)
{
    // get the face
    TopoDS_Shape face = getSubShape(m_shape, TopAbs_FACE, faceIndex);
    // get the wire
    TopoDS_Shape wire = getSubShape(face, TopAbs_WIRE, wireIndex);
    return getGeometricalElements(wire, TopAbs_EDGE, XAO::EDGE);
}

const int BrepGeometry::countSolidShells(const int& solidIndex)
{
    TopoDS_Shape solid = getSubShape(m_shape, TopAbs_SOLID, solidIndex);
    return countGeometricalElements(solid, TopAbs_SHELL);
}

std::vector<int> BrepGeometry::getSolidFaces(const int& solidIndex, const int& shellIndex)
{
    TopoDS_Shape solid = getSubShape(m_shape, TopAbs_SOLID, solidIndex);
    TopoDS_Shape shell = getSubShape(solid, TopAbs_SHELL, shellIndex);
    return getGeometricalElements(shell, TopAbs_FACE, XAO::FACE);
}

void BrepGeometry::getVertexXYZ(const int& vertexIndex, double& xCoord, double& yCoord, double& zCoord)
throw (XAO_Exception)
{
    xCoord = 0.;
    yCoord = 0.;
    zCoord = 0.;

    TopoDS_Shape vertex = getSubShape(m_shape, TopAbs_VERTEX, vertexIndex);
    if (vertex.ShapeType() != TopAbs_VERTEX)
        throw XAO_Exception(MsgBuilder() << "Shape " << vertexIndex<< " is not a point.");

    TopoDS_Vertex point = TopoDS::Vertex(vertex);
    if (!point.IsNull())
    {
        gp_Pnt aPnt = BRep_Tool::Pnt(point);
        xCoord = aPnt.X();
        yCoord = aPnt.Y();
        zCoord = aPnt.Z();
    }
}

// -----------------------------
const double BrepGeometry::getEdgeLength(const int& edgeIndex)
{
    TopoDS_Shape edge = getSubShape(m_shape, TopAbs_EDGE, edgeIndex);
    GProp_GProps system;
    BRepGProp::LinearProperties(edge, system);
    return system.Mass();
}

const double BrepGeometry::getFaceArea(const int& faceIndex)
{
    TopoDS_Shape face = getSubShape(m_shape, TopAbs_FACE, faceIndex);
    GProp_GProps system;
    BRepGProp::SurfaceProperties(face, system);
    return system.Mass();
}

const double BrepGeometry::getSolidVolume(const int& solidIndex)
{
    TopoDS_Shape solid = getSubShape(m_shape, TopAbs_SOLID, solidIndex);
    GProp_GProps system;
    BRepGProp::VolumeProperties(solid, system);
    return system.Mass();
}

// -----------------------------
const int BrepGeometry::getVertexID(const int& index)
{
    return XaoUtils::stringToInt(getVertexReference(index));
}

const int BrepGeometry::getEdgeID(const int& index)
{
    return XaoUtils::stringToInt(getEdgeReference(index));
}

const int BrepGeometry::getFaceID(const int& index)
{
    return XaoUtils::stringToInt(getFaceReference(index));
}

const int BrepGeometry::getSolidID(const int& index)
{
    return XaoUtils::stringToInt(getSolidReference(index));
}

// -----------------------------
void BrepGeometry::setVertexID(const int& index, const int& id)
{
    setVertexReference(index, XaoUtils::intToString(id));
}

void BrepGeometry::setEdgeID(const int& index, const int& id)
{
    setEdgeReference(index, XaoUtils::intToString(id));
}

void BrepGeometry::setFaceID(const int& index, const int& id)
{
    setEdgeReference(index, XaoUtils::intToString(id));
}

void BrepGeometry::setSolidID(const int& index, const int& id)
{
    setEdgeReference(index, XaoUtils::intToString(id));
}

// -----------------------------
const int BrepGeometry::findElement(const XAO::Dimension& dim, const int& id)
throw (XAO_Exception)
{
    if (dim == XAO::VERTEX)
        return findVertex(id);
    if (dim == XAO::EDGE)
        return findEdge(id);
    if (dim == XAO::FACE)
        return findFace(id);
    if (dim == XAO::SOLID)
        return findSolid(id);

    throw XAO_Exception(MsgBuilder() << "Unknown Dimension: " << dim);
}

const int BrepGeometry::findVertex(const int& id)
{
    return getVertexIndexByReference(XaoUtils::intToString(id));
}

const int BrepGeometry::findEdge(const int& id)
{
    return getEdgeIndexByReference(XaoUtils::intToString(id));
}

const int BrepGeometry::findFace(const int& id)
{
    return getFaceIndexByReference(XaoUtils::intToString(id));
}

const int BrepGeometry::findSolid(const int& id)
{
    return getSolidIndexByReference(XaoUtils::intToString(id));
}

// -----------------------------
const std::string BrepGeometry::findVertexName(const int& id)
{
    return getVertexName(findVertex(id));
}

const std::string BrepGeometry::findEdgeName(const int& id)
{
    return getEdgeName(findEdge(id));
}

const std::string BrepGeometry::findFaceName(const int& id)
{
    return getFaceName(findFace(id));
}

const std::string BrepGeometry::findSolidName(const int& id)
{
    return getSolidName(findSolid(id));
}

// -----------------------------
void BrepGeometry::changeVertexName(const int& id, const std::string& name)
throw (XAO_Exception)
{
    setVertexName(findVertex(id), name);
}

void BrepGeometry::changeEdgeName(const int& id, const std::string& name)
throw (XAO_Exception)
{
    setEdgeName(findEdge(id), name);
}

void BrepGeometry::changeFaceName(const int& id, const std::string& name)
throw (XAO_Exception)
{
    setFaceName(findFace(id), name);
}

void BrepGeometry::changeSolidName(const int& id, const std::string& name)
throw (XAO_Exception)
{
    setSolidName(findSolid(id), name);
}
