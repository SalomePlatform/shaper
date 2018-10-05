.. _box_feature:

Box
===

Box feature creates a box solid.

To create a Box in the active part:

#. select in the Main Menu *Primitives - > Box* item  or
#. click **Box** button in the toolbar:

.. image:: images/box.png
   :align: center

.. centered::
   **Box**  button 

There are 2 algorithms for creation of a Box:

.. image:: images/box_2pt_32x32.png
   :align: left
**By dimensions** 

.. image:: images/box_dxyz_32x32.png
   :align: left
**By two points** 


By dimensions
-------------

Box is created by dimensions along X, Y, Z axis starting from the origin.

.. image:: images/Box_dimensions.png
   :align: center

Input fields:

- **DX**, **DY**, **DZ** define dimensions of the box along the corresponding coordinate axes. 

**TUI Command**:  *model.addBox(Part_doc, DX, DY, DZ)*
  
**Arguments**:    Part + 3 real values (dimensions at origin).

Result
""""""

A solid box based on the origin of coordinates and with edges parallel to the coordinate axes.

.. image:: images/Box1.png
	   :align: center

**See Also** a sample TUI Script of a :ref:`tui_create_boxdim` operation.

By two points
-------------

Box is created by two points of the box diagonal.

.. image:: images/Box_2points.png
	 :align: center

Input fields:

- **Point 1** and **Point 2**  define diagonal points of the box selected in 3D OCC viewer or object browser.
  
**TUI Command**:  *model.addBox(Part_doc, point1, point2)*

**Arguments**:   Part + 2 selected points (opposite vertices of the box)

Result
""""""

A solid box based on two points and with edges parallel to the coordinate axes.

.. image:: images/Box2.png
	   :align: center
		   
.. centered::
   Created boxes

**See Also** a sample TUI Script of :ref:`tui_create_boxpnt` operation.
