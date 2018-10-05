
Torus
=====

Torus feature creates a torus solid.

To create a Torus in the active part:

#. select in the Main Menu *Primitives - > Torus* item  or
#. click **Torus** button in the toolbar

.. image:: images/Torus_button.png
   :align: center

.. centered::
   **Torus** button 

The following property panel appears.

.. image:: images/Torus.png
  :align: center

.. centered::
   Torus

Input fields:

- **Point** defines the center of the torus base selected in 3D OCC  viewer or object browser; 
- **Vector** defines the axis of the torus selected in 3D OCC  viewer or object browser;
- Dimensions:      
   - **Radius**;
   - **Ring radius**.
  
**TUI Command**: *model.addTorus(Part_doc, Point, Axis, Radius1, Radius2)*

**Arguments**:  Part + 1 vertex + 1 vector + 2 real values (first and second radii).

Result
""""""

An example is shown below.

.. image:: images/Torus_res.png
	   :align: center
		   
.. centered::
   Torus created  

**See Also** a sample TUI Script of :ref:`tui_create_torus` operation.
  
