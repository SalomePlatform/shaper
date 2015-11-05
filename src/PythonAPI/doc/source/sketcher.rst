:mod:`model.sketcher.sketch` -- Sketch interface
================================================

.. automodule:: model.sketcher.sketch
.. :members:

.. currentmodule:: model.sketcher.sketch

Create a sketch
---------------

.. autofunction:: addSketch

Modify the sketch
-----------------

The sketch object returned by the above described :meth:`~model.sketcher.sketch.addSketch` method
provides an interface for:

  * creating geometries
  * setting and modifying constraints

Create geometries
.................

.. automethod:: Sketch.addPoint
.. automethod:: Sketch.addLine
.. automethod:: Sketch.addArc
.. automethod:: Sketch.addCircle


Set constraints
...............

.. automethod:: Sketch.setRadius
.. automethod:: Sketch.setParallel


