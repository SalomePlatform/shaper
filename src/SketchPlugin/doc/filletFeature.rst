.. |fillet.icon|    image:: images/fillet.png

Fillet
======

| The Fillet operation rounds the corner formed by intersection of two sketch elements in a coincident point.
| The operation trims the elements at the intersection and creates a tangent arc between them.

To create a Fillet in the active Sketch:

#. select in the Main Menu *Sketch - > Fillet* item  or
#. click |fillet.icon| **Fillet** button in Sketch toolbar:

Property panel:

.. image:: images/Fillet_panel.png
  :align: center

.. centered::
   Fillet

Input fields:

- **Point** is the coincident point of two intersecting lines (or a line and an arc) selected in the view.

After the point is selected, Fillet preview appears in the view.

**TUI Command**:

.. py:function:: Sketch_1.setFillet(Point)

    :param object: Coincident point.
    :return: Result object.

.. py:function:: Sketch_1.setFilletWithRadius(Point, Radius)

    :param object: Coincident point.
    :param real: Radius.
    :return: Result object.

Result
""""""

Created Fillet appears in the view.

.. image:: images/Fillet_res.png
	   :align: center

.. centered::
   Created fillet

**See Also** a sample TUI Script of :ref:`tui_create_fillet` operation.