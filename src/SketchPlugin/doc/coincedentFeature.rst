.. _sketchCoincident:
.. |coincedence.icon|    image:: images/coincedence.png

Coincident constraint
=====================

Coincident constraint forces two points to be coincident, or one point to lie on a line, a circle or an arc.

To create a Coincident constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Coincident* item  or
#. click |coincedence.icon| **Coincident** button in Sketch toolbar:

Property panel:

.. image:: images/Coincident_panel.png
   :align: center

Input fields:

- **First object** is a point, a line, a circle or an arc selected in the view.
- **Second object** is a point, a line, a circle or an arc selected in the view.

Note that one of two objects should be a point (i.e. a point, a line or an arc end point, a center of a circle or an arc).

After the objects are selected, the point is highlighted in yellow in the view.

**TUI Command**:

.. py:function:: Sketch_1.setCoincident(Object1, Object2)

    :param object: First object.
    :param object: Second object.
    :return: Result object.

Result
""""""

Created Coincident constraint appears in the view.

.. image:: images/Coincident_res.png
	   :align: center

.. centered::
   Created coincident constraint

**See Also** a sample TUI Script of :ref:`tui_create_coincident` operation.