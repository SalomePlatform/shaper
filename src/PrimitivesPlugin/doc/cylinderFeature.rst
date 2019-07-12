.. |Cylinder_button.icon|    image:: images/Cylinder_button.png

Cylinder
========

Cylinder feature creates a cylinder solid.

To create a Cylinder in the active part:

#. select in the Main Menu *Primitives - > Cylinder* item  or
#. click |Cylinder_button.icon| **Cylinder** button in the toolbar

There are 2 algorithms for creation of a Cylinder:

.. image:: images/cylinder_32x32.png
   :align: left
**Cylinder** creates a cylinder.

.. image:: images/cylinder_portion_32x32.png
   :align: left
**Portion of cylinder** creates one segment of a cylinder.

Cylinder
--------

.. image:: images/Cylinder.png
   :align: center
	
.. centered::
   **Cylinder** property panel

Input fields:

- **Point** defines the center of the cylinder base selected in 3D OCC  viewer or object browser; 
- **Vector** defines the axis of the cylinder selected in 3D OCC  viewer or object browser;
- Dimensions:      
   - **Radius**;
   - **Height**.    

**TUI Commands**:

.. py:function:: model.addCylinder(Part_doc, Point, Axis, Radius, Height)

    :param part: The current part object.
    :param object: Vertex.
    :param object: Axis.
    :param real: Radius.
    :param real: Height.
    :return: Result object.

Result
""""""

An example is shown below.

.. image:: images/Cylinder1.png
	   :align: center
		   
.. centered::
   Cylinder  

**See Also** a sample TUI Script of :ref:`tui_create_cylinder` operation.

Portion of cylinder
-------------------

.. image:: images/Portion_cylinder.png
	   :align: center
		   
.. centered::
   **Portion of cylinder**  property panel 

Input fields for ***Portion of cylinder** tab include **Angle** defining a portion of cylinder.

**TUI Commands**:

.. py:function:: model.addCylinder(Part_doc, Point, Axis, Radius, Height,Angle)
  
    :param part: The current part object.
    :param object: Vertex.
    :param object: Axis.
    :param real: Radius.
    :param real: Height.
    :param real: Angle.
    :return: Result object.

Result
""""""

An example is shown below.

.. image:: images/Cylinder2.png
	   :align: center
		   
.. centered::
   Portion of cylinder  

**See Also** a sample TUI Script of :ref:`tui_create_cylportion` operation.
