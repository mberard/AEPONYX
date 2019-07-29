aeponyx
-------

aeponyx is a demonstration library. It contains a realistic layer stack, and a series of generic components: fiber couplers, waveguides, splitters etc.

Using the PDK
-------------
Demonstration of the PDK use is done in the projects folder of the samples.

Contents
--------
- doc
  Documentation of the PDK. It contains this document.
  
- ipkiss
  The base of the library, which contains a technology and a series of components.
  The folder structure should look like this:
  ipkiss/{name_of_lib}/technology
  ipkiss/{name_of_lib}/components
  ipkiss/{name_of_lib}/all.py
  
  Inside the components folder you are free to use the structure you like. 
  aeponyx contains the following components:
  - absorber: an termination component, used to minimize reflections on unused ports.
  - grating_couplers: contains a series of picazzo-based curved fiber grating couplers
  - metal: metal components (via, contact, bondpad, metal trace templates)
  - modulator: a mach-zehnder modulator with a phase modulation section
  - waveguides: rib and strib waveguides, including transitions to taper between different waveguide thicknesses / types
  
- samples
  - tech_layers
    Sample script that generates a GDS file with drawings on all layers defined in the TECH.
  - waveguides
    Sample script for generating a few basic waveguides.
