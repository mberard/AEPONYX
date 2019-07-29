__all__ = ["VIA_M1_M2", "BondPad", "BONDPAD_5050"]

from ipkiss3 import all as i3
from ..via import VIA_M1_M2

from ipkiss3.pcell.via import ElectricalVia


class BondPad(i3.PCell):

    """Base bondpad class used in our aeponyx PDK.

    A bondpad is a large rectangular area on the top metal layer for connecting external electrical wires, flip-chip bumps or probe needles.
    """

    via = i3.ChildCellProperty()

    def _default_via(self):
        return VIA_M1_M2()

    class Layout(i3.LayoutView):
        metal1_size = i3.Size2Property(default=(50.0, 50.0))
        metal2_size = i3.Size2Property(default=(50.0, 50.0))
        via_pitch = i3.Size2Property(default=(2.0, 2.0))
        metal1_layer = i3.LayerProperty(default=i3.TECH.PPLAYER.M1.LINE, locked=True)
        metal2_layer = i3.LayerProperty(default=i3.TECH.PPLAYER.M2.LINE, locked=True)

        def _generate_instances(self, insts):
            from numpy import floor
            via_span_x = min(self.metal1_size[0], self.metal2_size[0])
            via_span_y = min(self.metal1_size[1], self.metal2_size[1])
            periods_x = int(floor(via_span_x / self.via_pitch[0]))
            periods_y = int(floor(via_span_y / self.via_pitch[1]))
            insts += i3.ARef(reference=self.via,
                             origin=(-0.5 * via_span_x + 0.5 * self.via_pitch[0], -0.5 * via_span_y + 0.5 * self.via_pitch[1]),
                             period=self.via_pitch,
                             n_o_periods=(periods_x, periods_y))
            return insts

        def _generate_elements(self, elems):
            elems += i3.Rectangle(layer=self.metal1_layer, box_size=self.metal1_size)
            elems += i3.Rectangle(layer=self.metal2_layer, box_size=self.metal2_size)
            return elems

        def _generate_ports(self, ports):
            ports += i3.ElectricalPort(name="m1", position=(0.0, 0.0), shape=self.metal1_size, process=self.metal1_layer.process)
            ports += i3.ElectricalPort(name="m2", position=(0.0, 0.0), shape=self.metal2_size, process=self.metal2_layer.process)
            return ports

    class Netlist(i3.NetlistFromLayout):
        pass


@i3.lock_properties()
class BONDPAD_5050(BondPad):

    """Fixed default bondpad 50 x 50 um^2"""

    def _default_name(self):
        return "BONDPAD_5050"
