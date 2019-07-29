from aeponyx import technology
from aeponyx.components.ring.cell import DropRing
from ipkiss3 import all as i3

class DropRingAtWavelength(DropRing):
    """
    Drop ring that implements a simple heuristic to set the resonance wavelength.
    """

    res_wavelength = i3.PositiveNumberProperty(default=1.55, doc="Resonance wavelength")
    order = i3.PositiveIntProperty(default=200, doc="Order of the resonance")
    total_ring_length = i3.LockedProperty()

    def _default_total_ring_length(self):
        cm = self.directional_coupler.trace_template2.get_default_view(i3.CapheModelView)
        dneff = -(cm.n_g - cm.n_eff) / cm.center_wavelength
        neff_total = cm.n_eff + (self.res_wavelength - cm.center_wavelength) * dneff
        return self.order * self.res_wavelength / neff_total


class HeatedDropRingAtWavelength(DropRingAtWavelength):

    class Layout(DropRingAtWavelength.Layout):
        layer_heater = i3.LayerProperty()
        angle_gap = i3.AngleProperty(default=90.0)
        width_m1 = i3.AngleProperty(default=4.0)

        def _default_layer_heater(self):
            return i3.TECH.PPLAYER.M1.LINE

        def _generate_elements(self, elems):

            elems = super(DropRingAtWavelength.Layout, self)._generate_elements(elems)

            r = self.directional_coupler.bend_radius
            dcl = self.directional_coupler.coupler_length

            # Metal 1
            ring_s = i3.RingSegment(layer=self.layer_heater,
                                    center=(-dcl / 2.0, r),
                                    angle_start=180,
                                    angle_end=270,
                                    inner_radius=r - self.width_m1 / 2.0,
                                    outer_radius=r + self.width_m1 / 2.0)

            elems += ring_s
            trans = [i3.HMirror(), i3.VMirror(mirror_plane_y=self._get_distance_between_dc() / 2.0)]
            trans.append(trans[0] + trans[1])
            for t in trans:
                elems += ring_s.transform_copy(t)

            straights = i3.Path(layer=self.layer_heater,
                                shape=[(-r - dcl / 2.0, r), (-r - dcl / 2.0, self._get_distance_between_dc() - r)],
                                line_width=self.width_m1)

            elems += straights
            elems += straights.transform_copy(trans[0])

            top = i3.Path(layer=self.layer_heater,
                          shape=[(-dcl / 2.0, self._get_distance_between_dc()), (dcl / 2.0, self._get_distance_between_dc())],
                          line_width=self.width_m1)

            elems += top
            return elems

        def _generate_ports(self, ports):

            ports = super(DropRingAtWavelength.Layout, self)._generate_ports(ports)
            port_loc = i3.Coord2((-self.directional_coupler.coupler_length / 2.0 - self.width_m1 / 2.0, 0.0))
            pin = i3.ElectricalPort(process=self.layer_heater.process, name="el_in", position=port_loc)
            ports += pin
            ports += pin.transform_copy(transformation=i3.HMirror()).modified_copy(name="el_out")

            return ports


cell = HeatedDropRingAtWavelength(res_wavelength=1.3)
#cell = DropRingAtWavelength()
lv = cell.Layout()
lv.visualize(annotate=True)
lv.write_gdsii("ring.gds")
