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

#include <vector>

#include "TestUtils.hxx"
#include "BrepGeometryTest.hxx"
#include "../XAO_XaoUtils.hxx"
#include "../XAO_Xao.hxx"
#include "../XAO_BrepGeometry.hxx"

using namespace XAO;

void BrepGeometryTest::setUp()
{
}

void BrepGeometryTest::tearDown()
{
}

void BrepGeometryTest::cleanUp()
{
}

void readBrep(Geometry* geom, const std::string& fileName)
{
    char* txt = TestUtils::readTextFile(TestUtils::getTestFilePath(fileName));
    geom->setShapeString(txt);
}

void BrepGeometryTest::testGetIDs()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_1.brep");

    CPPUNIT_ASSERT_EQUAL(8, geom->countElements(XAO::VERTEX));
    CPPUNIT_ASSERT_EQUAL(8, geom->countVertices());
    int vertices[8] = { 6,7,9,11,16,17,19,21 };
    for (int i = 0; i < 8; ++i)
        CPPUNIT_ASSERT_EQUAL(vertices[i], geom->getVertexID(i));

    CPPUNIT_ASSERT_EQUAL(12, geom->countElements(XAO::EDGE));
    CPPUNIT_ASSERT_EQUAL(12, geom->countEdges());
    int edges[12] = { 5,8,10,12,15,18,20,22,25,26,29,30 };
    for (int i = 0; i < 12; ++i)
        CPPUNIT_ASSERT_EQUAL(edges[i], geom->getEdgeID(i));

    CPPUNIT_ASSERT_EQUAL(6, geom->countElements(XAO::FACE));
    CPPUNIT_ASSERT_EQUAL(6, geom->countFaces());
    int faces[6] = { 3,13,23,27,31,33 };
    for (int i = 0; i < 6; ++i)
        CPPUNIT_ASSERT_EQUAL(faces[i], geom->getFaceID(i));

    CPPUNIT_ASSERT_EQUAL(1, geom->countElements(XAO::SOLID));
    CPPUNIT_ASSERT_EQUAL(1, geom->countSolids());
    CPPUNIT_ASSERT_EQUAL(1, geom->getSolidID(0));

    delete geom;
}

void BrepGeometryTest::testGetReferences()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_1.brep");

    // vertex of index 1 has id = 7
    CPPUNIT_ASSERT_EQUAL(std::string("7"), geom->getElementReference(XAO::VERTEX, 1));
    CPPUNIT_ASSERT_EQUAL(std::string("7"), geom->getVertexReference(1));
    CPPUNIT_ASSERT_EQUAL(7, geom->getVertexID(1));
    CPPUNIT_ASSERT_EQUAL(1, geom->getElementIndexByReference(XAO::VERTEX, "7"));
    CPPUNIT_ASSERT_EQUAL(1, geom->findVertex(7));

    // edge of index 1 has id = 8
    CPPUNIT_ASSERT_EQUAL(std::string("8"), geom->getElementReference(XAO::EDGE, 1));
    CPPUNIT_ASSERT_EQUAL(std::string("8"), geom->getEdgeReference(1));
    CPPUNIT_ASSERT_EQUAL(8, geom->getEdgeID(1));
    CPPUNIT_ASSERT_EQUAL(1, geom->getElementIndexByReference(XAO::EDGE, "8"));
    CPPUNIT_ASSERT_EQUAL(1, geom->findEdge(8));

    // face of index 1 has id = 13
    CPPUNIT_ASSERT_EQUAL(std::string("13"), geom->getElementReference(XAO::FACE, 1));
    CPPUNIT_ASSERT_EQUAL(std::string("13"), geom->getFaceReference(1));
    CPPUNIT_ASSERT_EQUAL(13, geom->getFaceID(1));
    CPPUNIT_ASSERT_EQUAL(1, geom->getElementIndexByReference(XAO::FACE, "13"));
    CPPUNIT_ASSERT_EQUAL(1, geom->findFace(13));

    // solid of index 0 has id = 1
    CPPUNIT_ASSERT_EQUAL(std::string("1"), geom->getElementReference(XAO::SOLID, 0));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), geom->getSolidReference(0));
    CPPUNIT_ASSERT_EQUAL(1, geom->getSolidID(0));
    CPPUNIT_ASSERT_EQUAL(0, geom->getElementIndexByReference(XAO::SOLID, "1"));
    CPPUNIT_ASSERT_EQUAL(0, geom->findSolid(1));

    delete geom;
}

void BrepGeometryTest::testGetNames()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_1.brep");

    int id;

    // vertex of index 1 has id = 7
    id = 7;
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->findVertexName(id));
    geom->changeVertexName(id, std::string("va"));
    CPPUNIT_ASSERT_EQUAL(std::string("va"), geom->findVertexName(id));
    CPPUNIT_ASSERT_THROW(geom->changeVertexName(100, "a"), XAO_Exception);
    CPPUNIT_ASSERT_THROW(geom->findVertexName(100), XAO_Exception);

    // edge of index 1 has id = 8
    id = 8;
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->findEdgeName(id));
    geom->changeEdgeName(id, std::string("ea"));
    CPPUNIT_ASSERT_EQUAL(std::string("ea"), geom->findEdgeName(id));
    CPPUNIT_ASSERT_THROW(geom->changeEdgeName(100, "a"), XAO_Exception);
    CPPUNIT_ASSERT_THROW(geom->findEdgeName(100), XAO_Exception);

    // face of index 1 has id = 13
    id = 13;
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->findFaceName(id));
    geom->changeFaceName(id, std::string("fa"));
    CPPUNIT_ASSERT_EQUAL(std::string("fa"), geom->findFaceName(id));
    CPPUNIT_ASSERT_THROW(geom->changeFaceName(100, "a"), XAO_Exception);
    CPPUNIT_ASSERT_THROW(geom->findFaceName(100), XAO_Exception);

    // solid of index 0 has id = 1
    id = 1;
    CPPUNIT_ASSERT_EQUAL(std::string(""), geom->findSolidName(id));
    geom->changeSolidName(id, std::string("sa"));
    CPPUNIT_ASSERT_EQUAL(std::string("sa"), geom->findSolidName(id));
    CPPUNIT_ASSERT_THROW(geom->changeSolidName(100, "a"), XAO_Exception);
    CPPUNIT_ASSERT_THROW(geom->findSolidName(100), XAO_Exception);

    delete geom;
}

void BrepGeometryTest::testGetEdgeVertices()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");

    // edge of index 23, id = #63
    // vertex are 47 (#12), 59 (#15)
    int v1, v2;
    geom->getEdgeVertices(23, v1, v2);
    CPPUNIT_ASSERT_EQUAL(12, v1);
    CPPUNIT_ASSERT_EQUAL(15, v2);

    delete geom;
}

void printVector(std::vector<int>& v)
{
    std::cout << "# ";
    for (unsigned int i = 0; i < v.size(); i++)
        std::cout << v[i] << ", ";
    std::cout << std::endl;
}

void BrepGeometryTest::testGetFaceEdges()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");

    CPPUNIT_ASSERT_EQUAL(2, geom->countFaceWires(1)); // face 13
    CPPUNIT_ASSERT_EQUAL(1, geom->countFaceWires(2)); // face 29

    // wire 0 of face 1 (#13) => edge 4 (#15), 5 (#17), 0 (#5), 6 (#19)
    std::vector<int> edges = geom->getFaceEdges(1, 0);
    CPPUNIT_ASSERT_EQUAL(4, (int)edges.size());
    int ids1[4] = { 4,5,0,6 };
    for (int i = 0; i < 4; ++i)
        CPPUNIT_ASSERT_EQUAL(ids1[i], edges[i]);

    // wire 1 of face 13 => edge 7 (#21) ,8 (#24), 9 (#26), 10 (#28)
    edges = geom->getFaceEdges(1, 1);
    CPPUNIT_ASSERT_EQUAL(4, (int)edges.size());
    int ids2[4] = { 7,8,9,10 };
    for (int i = 0; i < 4; ++i)
        CPPUNIT_ASSERT_EQUAL(ids2[i], edges[i]);

    delete geom;
}

void BrepGeometryTest::testSolidFaces()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Cut_2.brep");

    CPPUNIT_ASSERT_EQUAL(5, geom->countSolidShells(0));

    std::vector<int> faces = geom->getSolidFaces(0, 0);
    CPPUNIT_ASSERT_EQUAL(6, (int)faces.size());
    int ids[6] = { 0, 1, 2, 3, 4, 5 };
    for (int i = 0; i < 6; ++i)
        CPPUNIT_ASSERT_EQUAL(ids[i], faces[i]);

    faces = geom->getSolidFaces(0, 1);
    CPPUNIT_ASSERT_EQUAL(6, (int)faces.size());
    int ids2[6] = { 6, 7, 8, 9, 10, 11 };
    for (int i = 0; i < 6; ++i)
        CPPUNIT_ASSERT_EQUAL(ids2[i], faces[i]);

    delete geom;
}

void BrepGeometryTest::testGetVertex()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");

    double x, y, z;
    geom->getVertexXYZ(15, x, y, z);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(60., x, 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(80., y, 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(60., z, 1e-6);

    delete geom;
}

void BrepGeometryTest::testGetEdgeLength()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");

    // edges 0 (#5), 7 (#21)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(200., geom->getEdgeLength(0), 0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(80., geom->getEdgeLength(7), 0);

    delete geom;
}

void BrepGeometryTest::testGetFaceArea()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");

    // faces 0 (#3), 1 (#13)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(40000., geom->getFaceArea(0), 1e-9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(33600., geom->getFaceArea(1), 1e-9);

    delete geom;
}

void BrepGeometryTest::testGetSolidVolume()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");

    CPPUNIT_ASSERT_DOUBLES_EQUAL(7488000., geom->getSolidVolume(0), 1e-9);

    delete geom;
}

void BrepGeometryTest::testParse()
{
    BrepGeometry* geom = new BrepGeometry("box");
    readBrep(geom, "Box_2.brep");
    std::cout << std::endl;

    for (int solidIndex = 0; solidIndex < geom->countSolids(); ++solidIndex)
    {
        std::cout << "Solid #" << solidIndex << " : " << geom->getSolidReference(solidIndex) << std::endl;
        int nbShells = geom->countSolidShells(solidIndex);
        for (int shellIndex = 0; shellIndex < nbShells; ++shellIndex)
        {
            std::cout << "  Shell #" << shellIndex << std::endl;
            std::vector<int> faces = geom->getSolidFaces(solidIndex, shellIndex);
            for (unsigned int indf = 0; indf < faces.size(); ++indf)
            {
                int faceIndex = faces[indf];
                std::cout  << "    Face #" << geom->getFaceReference(faceIndex) << std::endl;

                int nbWires = geom->countFaceWires(faceIndex);
                for (int wireIndex = 0; wireIndex < nbWires; ++wireIndex)
                {
                    std::cout << "      Wire #" << wireIndex << std::endl;
                    std::vector<int> edges = geom->getFaceEdges(faceIndex, wireIndex);
                    for (unsigned int inde = 0; inde < edges.size(); ++inde)
                    {
                        int edgeIndex = edges[inde];
                        std::cout << "        Edge #" << geom->getEdgeReference(edgeIndex) << " : ";

                        int va = 0, vb = 0;
                        geom->getEdgeVertices(edgeIndex, va, vb);
                        int vaRef = geom->getVertexID(va);
                        int vbRef = geom->getVertexID(vb);
                        double ax, ay, az, bx, by, bz;
                        geom->getVertexXYZ(va, ax, ay, az);
                        geom->getVertexXYZ(vb, bx, by, bz);

                        std::cout << vaRef << " (" << ax << ", " << ay << ", " << az << ")";
                        std::cout << " - ";
                        std::cout << vbRef << " (" << bx << ", " << by << ", " << bz << ")";
                        std::cout << std::endl;
                    }
                }


            }
        }
    }

    delete geom;
}
