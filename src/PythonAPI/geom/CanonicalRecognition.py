# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

from GeomAlgoAPI import GeomAlgoAPI_CanonicalRecognition

class CanonicalRecognition:
    "The class provides recognition of canonical shapes"

    def isPlane(self, shape, tolerance, normal = [], origin = []):
        """
        Check if shape is planar
        Usage:
        > CR.isPlane(shape, tolerance, normal, origin)
        """
        return GeomAlgoAPI_CanonicalRecognition.isPlane(shape, tolerance, normal, origin)

    def isSphere(self, shape, tolerance, origin = [], radius = 0.0):
        """
        Check if shape is spherical
        Usage:
        > CR.isSphere(shape, tolerance, origin, radius)
        """
        return GeomAlgoAPI_CanonicalRecognition.isSphere(shape, tolerance, origin, radius)

    def isCone(self, shape, tolerance, axis = [], apex = [], halfAngle = 0.0):
        """
        Check if shape is conical
        Usage:
        > CR.isCone(shape, tolerance, axis, apex, halfAngle)
        """
        return GeomAlgoAPI_CanonicalRecognition.isCone(shape, tolerance, axis, apex, halfAngle)

    def isCylinder(self, shape, tolerance, axis = [], origin = [], radius = 0.0):
        """
        Check if shape is cylinder
        Usage:
        > CR.isCylinder(shape, tolerance, axis, origin, radius)
        """
        return GeomAlgoAPI_CanonicalRecognition.isCylinder(shape, tolerance, axis, origin, radius)

    def isLine(self, edge, tolerance, direction = [], origin = []):
        """
        Check if edge/wire is line
        Usage:
        > CR.isLine(edge, tolerance, direction, origin)
        """
        return GeomAlgoAPI_CanonicalRecognition.isLine(edge, tolerance, direction, origin)

    def isCircle(self, edge, tolerance, normal = [], origin = [], radius = 0.0):
        """
        Check if edge/wire is circle
        Usage:
        > CR.isCircle(edge, tolerance, normal, origin, radius)
        """
        return GeomAlgoAPI_CanonicalRecognition.isCircle(edge, tolerance, normal, origin, radius)

    def isEllipse(self, edge, tolerance, normal = [], dirX = [], origin = [], majorRadius = 0.0, minorRadius = 0.0):
        """
        Check if edge/wire is ellipse
        Usage:
        > CR.isEllipse(edge, tolerance, normal, dirX, origin, majorRadius, minorRadius)
        """
        return GeomAlgoAPI_CanonicalRecognition.isEllipse(edge, tolerance, normal, dirX, origin, majorRadius, minorRadius)

    def isImplemented(self):
        """
        Check if the CanonicalRecognition is implemented (built with appropriate OCCT version)
        Usage:
        > CR.isImplemented()
        """
        return GeomAlgoAPI_CanonicalRecognition.isImplemented()
