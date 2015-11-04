# Direct Geometry API not yet swigged
# Author: Daniel Brunier-Coulin
# -----------------------------

from GeomAPI import *


class Ax3:
  """A class to represent a Coordinate system object"""

  def __init__(self, origin, normal, dirx):
    """Constructor"""
    ### Create an origin point
    self.o  = origin
    ### Create a normal vector
    self.n  = normal
    ### Create X axis direction
    self.dx = dirx

  def location (self):
    """Returns origin point"""
    return self.o

  def direction (self):
    """Returns normal direction"""
    return self.n

  def xDirection (self):
    """Returns direction of X axis"""
    return self.dx

  def yDirection (self):
    """Returns direction of Y axis"""
    return self.n.cross(self.dx)
