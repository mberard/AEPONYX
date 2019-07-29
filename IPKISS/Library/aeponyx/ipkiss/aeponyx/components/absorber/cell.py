"""Sample absorber, based on a GDS file.

To create a fixed cell based on a GDS file, you can use the GDSCell class. A GDSCell needs the following information:
- filename (using _default_filename): the location to the GDS file
- In the Layout, you have to define the position of the ports
- (optional): you can define a netlist and circuit model (see the documentation on how to create a netlist and circuit model)
"""
import ipkiss3.all as i3
from ipkiss3.pcell.gdscell import GDSCell
import os
from numpy import sqrt

__all__ = ["ABSORBER"]


@i3.lock_properties()
class ABSORBER(GDSCell):

    def _default_name(self):
        return "Absorber"

    def _default_cell_name(self):
        return "Absorber"

    def _default_prefix(self):
        return ""

    def _default_filename(self):
        curpath = os.path.dirname(os.path.abspath(__file__))
        return os.path.join(curpath, "absorber.gds")

    class Layout(GDSCell.Layout):

        def _generate_ports(self, ports):
            ports += i3.OpticalPort(name="in", position=(4.0, 0.0), angle=0.0, trace_template=i3.TECH.PCELLS.WG.DEFAULT)
            return ports

    class Netlist(i3.NetlistView):
        def _generate_terms(self, terms):
                terms += i3.OpticalTerm(name='in')
                return terms

    class CircuitModel(i3.CircuitModelView):
        reflection_dB = i3.NonNegativeNumberProperty(doc='Reflection (dB)', default=+40)

        def _generate_model(self):
            from aeponyx.compactmodels import AbsorberCompactModel
            return AbsorberCompactModel(reflection_dB=self.reflection_dB)


