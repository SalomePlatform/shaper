Sketch interface
================

.. automodule:: model.sketcher.sketch
.. :members:

.. currentmodule:: model.sketcher.sketch

Create a sketch
---------------

.. autosummary::
   addSketch

Add geometries
--------------

.. autosummary::

   Sketch.addPoint
   Sketch.addLine
   Sketch.addArc
   Sketch.addCircle

Set constraints
---------------

.. autosummary::

   Sketch.setRadius
   Sketch.setParallel

Detailed description
--------------------

Add a sketch to the document
............................

.. autofunction:: addSketch

Sketch object
.............

The sketch object returned by the above described :meth:`~model.sketcher.sketch.addSketch` method
provides an interface for:

  * creating geometries
  * setting and modifying constraints

.. autoclass:: Sketch
   :members:
