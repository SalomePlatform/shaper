// Copyright (C) 2013-2023  CEA/DEN, EDF R&D
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

#ifndef __XAO_BREPGEOMETRY_HXX__
#define __XAO_BREPGEOMETRY_HXX__

#include <string>
#include <vector>

#include <TopoDS_Shape.hxx>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Geometry.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif


namespace XAO
{
    /**
     * @class BrepGeometry
     * Representation of a BRep Geometry.
     */
    class XAO_EXPORT BrepGeometry : public Geometry
    {
    public:
        /**
         * Default Constructor.
         */
        BrepGeometry();

        /**
         * Constructor.
         * @param name the name of the geometry.
         */
        BrepGeometry(const std::string& name);

        virtual ~BrepGeometry() {}

        /**
         * Gets the format of the geometry.
         * @return the format of the geometry.
         */
        virtual XAO::Format getFormat() { return XAO::BREP; }

        /**
         * Gets the shape as a string.
         * @return the shape as a string.
         */
        virtual const std::string getShapeString();

        /**
         * Sets the shape from a string.
         * @param shape the shape as a string.
         */
        virtual void setShapeString(const std::string& shape);

        /**
         * Writes shape to a file
         * @param fileName the path to the file
         */
        virtual void writeShapeFile(const std::string& fileName) ;

        /**
         * Reads shape from a file
         * @param fileName the path to the file
         */
        virtual void readShapeFile(const std::string& fileName) ;

#ifdef SWIG
        %pythoncode %{
        def setShape(self, shape):
            if shape is not None and 'GetShapeStream' in dir(shape):
                self.setShapeString(shape.GetShapeStream())
            else:
                raise XAO_Exception("Cannot set shape")
        %}
#endif

        /**
         * Gets the shape as a TopoDS_Shape.
         * @return the TopoDS_Shape.
         */
        TopoDS_Shape getTopoDS_Shape();

        /**
         * Sets the shape from a TopoDS_Shape.
         * @param shape the TopoDS_Shape to set.
         */
        void setTopoDS_Shape(const TopoDS_Shape& shape);

        /**
         * Gives the two extrimities of an edge.
         * @param edgeIndex the index of the edge.
         * @param vertexA
         * @param vertexB
         */
        void getEdgeVertices(int edgeIndex, int& vertexA, int& vertexB);

        /**
         * Gets the number of wires of a face (including holes).
         * @param faceIndex the index of the face.
         * @return the number of wires.
         */
        int countFaceWires(int faceIndex);

        /**
         * Gets the indices of the wires of the face.
         * @param faceIndex the index of the face.
         * @param wireIndex the index of the wire.
         * @return the list of wires for the given face.
         */
        std::vector<int> getFaceEdges(int faceIndex, int wireIndex);

        /**
         * Gets the number of shells of a solid (including cavities).
         * @param solidIndex the index of the solid.
         * @return the number of shells.
         */
        int countSolidShells(int solidIndex);

        /**
         * Gets the indices of the shells of the solids.
         * @param solidIndex the index of the solid.
         * @param shellIndex the index of the shell (for the given solid).
         * @return the list of shells for the given solid.
         */
        std::vector<int> getSolidFaces(int solidIndex, int shellIndex);

        /**
         * Gets the coordinates of a vertex.
         * @param vertexIndex the index of the vertex.
         * @param xCoord the X coordinate.
         * @param yCoord the Y coordinate.
         * @param zCoord the Z coordinate.
         */
        void getVertexXYZ(int vertexIndex, double& xCoord, double& yCoord, double& zCoord)
        ;

        /**
         * Gets the length of an edge.
         * @param index the index of the edge.
         * @return the length of the edge.
         */
        double getEdgeLength(int index);

        /**
         * Gets the are of a face.
         * @param index the index of a face.
         * @return the area of the face.
         */
        double getFaceArea(int index);

        /**
         * Gets the volume of a solid.
         * @param index the index of the solid.
         * @return the volume of the solid.
         */
        double getSolidVolume(int index);

        /**
         * Gets the ID of a vertex.
         * @param index the index of the vertex.
         * @return the ID of the vertex.
         */
        int getVertexID(int index);

        /**
         * Gets the ID of an edge.
         * @param index the index of the edge.
         * @return the ID of the edge.
         */
        int getEdgeID(int index);

        /**
         * Gets the ID of a face.
         * @param index the index of the face.
         * @return the ID of the face.
         */
        int getFaceID(int index);

        /**
         * Gets the ID of a solid.
         * @param index the index of the solid.
         * @return the ID of the solid.
         */
        int getSolidID(int index);

        /**
         * Sets the ID of a vertex.
         * @param index the index of the vertex to set.
         * @param id the id to set.
         */
        void setVertexID(int index, int id);

        /**
         * Sets the ID of an edge.
         * @param index the index of the edge to set.
         * @param id the id to set.
         */
        void setEdgeID(int index, int id);

        /**
         * Sets the ID of a face.
         * @param index the index of the face to set.
         * @param id the id to set.
         */
        void setFaceID(int index, int id);

        /**
         * Sets the ID of a solid.
         * @param index the index of the solid to set.
         * @param id the id to set.
         */
        void setSolidID(int index, int id);

        /**
         * Finds a vertex with its ID.
         * @param id the ID of the vertex.
         * @return the index of the vertex.
         */
        int findVertex(int id);

        /**
         * Finds an edge with its ID.
         * @param id the ID of the edge.
         * @return the index of the edge.
         */
        int findEdge(int id);

        /**
         * Finds a face with its ID.
         * @param id the ID of the face.
         * @return the index of the face.
         */
        int findFace(int id);

        /**
         * Finds a solid with its ID.
         * @param id the ID of the solid.
         * @return th index of the solid.
         */
        int findSolid(int id);

        /**
         * Finds the name of a vertex with its ID.
         * @param id the ID of the vertex.
         * @return the name of the vertex.
         */
        const std::string findVertexName(int id);

        /**
         * Finds the name of an edge with its ID.
         * @param id the ID of the edge.
         * @return the name of the edge.
         */
        const std::string findEdgeName(int id);

        /**
         * Finds the name of a face with its ID.
         * @param id the ID of the face.
         * @return the name of the face.
         */
        const std::string findFaceName(int id);

        /**
         * Finds the name of a solid with its ID.
         * @param id the ID of the solid.
         * @return the name of the solid.
         */
        const std::string findSolidName(int id);

        /**
         * Changes the name of a vertex.
         * @param id the ID of the vertex.
         * @param name the name to set.
         */
        void changeVertexName(int id, const std::string& name) ;

        /**
         * Changes the name of an edge.
         * @param id the ID of the edge
         * @param name the name to set.
         */
        void changeEdgeName(int id, const std::string& name) ;

        /**
         * Changes the name of a face.
         * @param id the ID of the face.
         * @param name the name to set.
         */
        void changeFaceName(int id, const std::string& name) ;

        /**
         * Changes the name of a solid.
         * @param id the ID of the solid.
         * @param name the name to set.
         */
        void changeSolidName(int id, const std::string& name) ;

    private:
        void initIds();
        void initListIds(const TopAbs_ShapeEnum& shapeType, GeometricElementList& eltList);
        TopoDS_Shape getSubShape(const TopoDS_Shape& mainShape, const TopAbs_ShapeEnum& shapeType, int shapeIndex)
            ;
        int countGeometricalElements(const TopoDS_Shape& shape, const TopAbs_ShapeEnum& shapeType);
        std::vector<int> getGeometricalElements(const TopoDS_Shape& shape, const TopAbs_ShapeEnum& shapeType, XAO::Dimension dim);
        int findElement(XAO::Dimension dim, int id)
            ;

    private:
        TopoDS_Shape m_shape;
    };
}

#endif // __XAO_BREPGEOMETRY_HXX__
