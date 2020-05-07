.. |Cone_button.icon|    image:: images/Cone_button.png

Cone
====

Cone feature creates a cone solid.

To create a Cone in the active part:

#. select in the Main Menu *Primitives - > Cone* item  or
#. click |Cone_button.icon| **Cone** button in the toolbar

The following property panel appears.

.. figure:: images/Cone.png
   :align: center

   Cone property panel

Input fields:

- **Point** defines the center of the cone base selected in 3D OCC  viewer or object browser; 
- **Vector** defines the axis of the cone selected in 3D OCC  viewer or object browser;
- Dimensions:      
   - **Base Radius**;
   - **Top radius**;  
   - **Height**.
  
**TUI Command**:

.. py:function:: model.addCone(Part_doc, Point, Axis, Radius1, Radius2, Height)

    :param part: The current part object.
    :param object: Vertex.
    :param object: Axis.
    :param real: Radius 1.
    :param real: Radius 2.
    :param real: Height.
    :return: Result object.

Result
""""""

If both radii are non-zero, then the cone will be truncated.

If the radii are equal, the Cylinder will be created instead of cone.

.. figure:: images/Cone_res.png
   :align: center
		   
   Cone created  

**See Also** a sample TUI Script of :ref:`tui_create_cone` operation.
