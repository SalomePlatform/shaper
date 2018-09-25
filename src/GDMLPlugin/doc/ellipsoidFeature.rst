
Ellipsoid
=========

The feature Ellipsoid creates a 3d ellipsoid or its part using GDML language.

To create a Ellipsoid in the active part:

#. select in the Main Menu *GDML - > Ellipsoid* item  or
#. click **Ellipsoid** button in the toolbar.

.. image:: images/ellips_btn.png
   :align: center

.. centered::
   **Ellipsoid**  button 

The following property panel will be opened:

.. image:: images/Ellipsoid.png
   :align: center
	
.. centered::
   **Ellipsoid property panel**

The property panel contains image which explains meaning of input values:

- **ax** is a size of the ellipsoind along X axis.
- **by** is a size of the ellipsoind along Y axis.
- **cz** is a size of the ellipsoind along Z axis.
- **zcut1** is a z coordinate of a lower cut plane.
- **zcut2** is a z coordinate of a upper cut plane.

**TUI Command**:  *model.addEllipsoid(Part_doc, ax, by, cz)*
  
**Arguments**:    Part + ax + by + cz.

Result
""""""

The Result of the operation will be a SOLID.

.. image:: images/CreatedEllipsoid.png
	   :align: center
		   
.. centered::
   Ellipsoid created

**See Also** a sample TUI Script of a :ref:`tui_create_ellipsoid` operation.
