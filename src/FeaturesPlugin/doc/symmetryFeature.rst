
Symmetry
========

**Symmetry** feature makes a symmetrical copy of the selected shape.

To create a Symmetry in the active part:

#. select in the Main Menu *Part - > Symmetry* item  or
#. click **Symmetry** button in the toolbar

.. image:: images/symmetry.png    
   :align: center

.. centered::
   **Symmetry** button 

Symmetrical copy of an Object can be mirrored in three different ways:

  .. image:: images/symmetry_point_32x32.png    
    :align: left
  through a Point of symmetry 

  .. image:: images/symmetry_axis_32x32.png      
    :align: left
  through an Axis of symmetry  

  .. image:: images/symmetry_plane_32x32.png    
    :align: left
  through a Plane of symmetry 

Symmetry through a Point of symmetry
------------------------------------

.. image:: images/symmetryPoint.png
  :align: center

.. centered::
  Symmetry through a Point of symmetry property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **Point**  defines a point of symmetry selected in 3D OCC viewer or object browser;
- **Create a copy** checkbox preserves/removes the initial object.

**TUI Command**:  *model.addSymmetry(Part_doc, [shape], point, copy)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + point in format *model.selection(TYPE, shape)*+ boolean (Create a copy key).

Result
""""""

The result of operation is a mirrored initial shape with **Create a copy** = *True*.

.. image:: images/symmetryPointres.png
   :align: center

.. centered::
   Symmetry through a Point of symmetry

**See Also** a sample TUI Script of :ref:`tui_symmetry_point` operation.

Symmetry through an Axis of symmetry
------------------------------------

.. image:: images/symmetryAxis.png
  :align: center

.. centered::
  Symmetry through an Axis of symmetry property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **Axis**  defines an axis of symmetry selected in 3D OCC viewer or object browser;
- **Create a copy** checkbox preserves/removes the initial object.

**TUI Command**:  *model.addSymmetry(Part_doc, [shape], axis, copy)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + axis in format *model.selection(TYPE, shape)*+ boolean (Create a copy key).

Result
""""""

The result of operation is a mirrored initial shape with **Create a copy** = *False*.

.. image:: images/symmetryAxisres.png
   :align: center

.. centered::
   Symmetry through an Axis of symmetry

**See Also** a sample TUI Script of :ref:`tui_symmetry_axis` operation.  

Symmetry through a Plane of symmetry
------------------------------------

.. image:: images/symmetryPlane.png
  :align: center

.. centered::
  Symmetry through a Plane of symmetry property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **Plane**  defines a plane of symmetry selected in 3D OCC viewer or object browser;
- **Create a copy** checkbox preserves/removes the initial object.

**TUI Command**:  *model.addSymmetry(Part_doc, [shape], plane, copy)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + plane in format *model.selection(TYPE, shape)*+ boolean (Create a copy key).

Result
""""""

The result of operation is a mirrored initial shape with **Create a copy** = *False*.

.. image:: images/symmetryPlaneres.png
   :align: center

.. centered::
   Symmetry through a Plane of symmetry

**See Also** a sample TUI Script of :ref:`tui_symmetry_plane` operation.   