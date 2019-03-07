
Python addons
=============

It is possible to create custom features.

The corresponding folder should be created for each feature at *../sources/src/PythonAddons/macros*.

A feature description includes 4 files:

- widget.xml with a description of the property panel,
-  __init__.py,
- feature.py with python commands, 
- icon.png with image of button in the toolbar (the file is located at sub-folder /icons).

Two examples of already created custom features are:

.. toctree::
   :titlesonly:
   :maxdepth: 1

   rectangleFeature.rst
