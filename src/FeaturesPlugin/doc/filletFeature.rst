.. |fillet.icon|    image:: images/fillet.png

.. _featureFillet:

Fillet
======

**Fillet** feature creates fillets on the edges of a shape. 

To create a Fillet in the active part:

#. select in the Main Menu *Feature - > Fillet* item  or
#. click |fillet.icon| **Fillet** button in the toolbar

There are 2 types of fillet:

  .. image:: images/fillet_fixed_radius.png   
    :align: left
  fillet by fixed radius

  .. image:: images/fillet_var_radius.png   
    :align: left
  fillet by variable radius

Fillet by fixed radius
----------------------

The  property panel is shown below.

.. image:: images/FilletFixed.png
  :align: center

.. centered::
  Fillet by fixed radius property panel

Input fields:

- **Faces or/and edges** panel contains filleted faces and edges. All edges of a face are subject to fillet operation. Faces and edges are selected in 3D OCC viewer;
- **Radius** defines fillet radius.

**TUI Command**:

.. py:function:: model.addFillet(Part_doc, [face,edge], radius)

    :param part: The current part object.
    :param list: A list of faces and edges subject to fillet operation in format *model.selection(TYPE, shape)*.
    :param number: Radius value.
    :return: Created object.

Result
""""""

Result of **Fillet by fixed radius** is shown below.

.. image:: images/fillet_fix_rad.png
   :align: center

.. centered::
   Fillet by fixed radius

**See Also** a sample TUI Script of :ref:`tui_create_fillet1` operation.

Fillet by variable radius
-------------------------

Alternatively, there is a possibility to create a fillet with a variable radius.

.. image:: images/FilletVarious.png
  :align: center

.. centered::
  Fillet by variable radius

Input fields:

- **Faces or/and edges** panel contains filleted faces and edges. All edges of a face are subject to fillet operation. Faces and edges are selected in 3D OCC viewer;
- **Start radius** defines  the  fillet radius at the start of the selected edge(s);  
- **End radius** defines  the  fillet radius at the end of the selected edge(s).

**TUI Command**:

.. py:function:: model.addFillet(Part_doc, [face,edge], R1, R2)

    :param part: The current part object.
    :param list: A list of faces and edges subject to fillet operation in format *model.selection(TYPE, shape)*.
    :param number: Start radius value.
    :param number: End radius value.
    :return: Created object.

Result
""""""

Result of **Fillet by variable radius** is shown below.

.. image:: images/fillet_var_rad.png
   :align: center

.. centered::
   Fillet by variable radius

**See Also** a sample TUI Script of :ref:`tui_create_fillet2` operation.
