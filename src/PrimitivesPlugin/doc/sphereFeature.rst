
Sphere
======

The feature Sphere creates a sphere solid.

To create a Sphere in the active part:

#. select in the Main Menu *Primitives - > Sphere* item  or
#. click **Sphere** button in the toolbar

.. image:: images/Sphere_button.png
   :align: center

.. centered::
   **Sphere** button 

The following property panel appears.

.. image:: images/Sphere.png
  :align: center

.. centered::
   Sphere property panel
   
Input fields:

- **Point** defines center of the sphere selected in 3D OCC  viewer or object browser; 
- **Radius** defines the radius.
  
**TUI Command**: *model.addSphere(Part_doc, Point, Radius)*.

**Arguments**:  Part + 1 vertex + 1 real value (radius).

Result
""""""

Example is shown below.

.. image:: images/Sphere_res.png
	   :align: center
		   
.. centered::
   Sphere created  

**See Also** a sample TUI Script of a :ref:`tui_create_sphere` operation.
