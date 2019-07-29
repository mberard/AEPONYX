import ipkiss3.all as i3
import numpy as np

__all__ = ["PhotoDiode"]


class PhotoDiode(i3.PCell):
    class Layout(i3.LayoutView):
        length = i3.PositiveNumberProperty(default=20.0)
        width = i3.PositiveNumberProperty(default=4.0)
        taper_width=i3.PositiveNumberProperty(default=1.0)
        contact_pitch=i3.PositiveNumberProperty(default=0.6)
        contact_offset=i3.PositiveNumberProperty(default=1.5)

        def _generate_elements(self, elems):
            from ..metal.contact import CONTACT_STD
            elems += i3.Line(layer=i3.TECH.PPLAYER.GE,
                             begin_coord=(0.0, 0.0), end_coord=(self.length, 0.0),
                             line_width=self.width)
            elems += i3.Line(layer=i3.TECH.PPLAYER.N,
                             begin_coord=(0.0, self.contact_offset), end_coord=(self.length, self.contact_offset),
                             line_width=1.0)
            elems += i3.Line(layer=i3.TECH.PPLAYER.P,
                             begin_coord=(0.0, -self.contact_offset), end_coord=(self.length, -self.contact_offset),
                             line_width=1.0)
            elems += i3.Wedge(layer=i3.TECH.PPLAYER.SI,
                              begin_coord=(-5.0, 0.0), end_coord=(0.0, 0.0),
                              begin_width=0.45,
                              end_width=self.taper_width)
            elems += i3.ARef(reference=CONTACT_STD(), origin=(0.5 * self. contact_pitch, self.contact_offset),
                             period=(self.contact_pitch, 0.0), n_o_periods=(int(np.floor(self.length/self.contact_pitch)), 1)
                             )
            elems += i3.ARef(reference=CONTACT_STD(), origin=(0.5 * self. contact_pitch, -self.contact_offset),
                             period=(self.contact_pitch, 0.0), n_o_periods=(int(np.floor(self.length/self.contact_pitch)), 1)
                             )
            elems += i3.Boundary(layer=i3.TECH.PPLAYER.M1,
                                 shape=[(0.0, self.contact_offset-0.5), (self.length, self.contact_offset-0.5),
                                        (self.length, self.contact_offset+2.0), (0.0, self.contact_offset+2.0)])
            elems += i3.Boundary(layer=i3.TECH.PPLAYER.M1,
                                 shape=[(0.0, -self.contact_offset+0.5), (self.length, -self.contact_offset+0.5),
                                        (self.length, -self.contact_offset-2.0), (0.0, -self.contact_offset-2.0)])

            return elems

        def _generate_ports(self, ports):
            from ..waveguides.wire import SWG450
            ports += i3.OpticalPort(name="in", position=(-5.0, 0.0),
                                    angle=180.0, trace_template=SWG450())
            ports += i3.ElectricalPort(name="cathode", position=(0.5 * self.length, self.contact_offset+1.25))
            ports += i3.ElectricalPort(name="anode", position=(0.5 * self.length, -self.contact_offset-1.25))
            return ports


    class Netlist(i3.NetlistView):

        def _generate_terms(self, terms):
            terms += i3.OpticalTerm(name='in')
            terms += i3.ElectricalTerm(name='anode')
            terms += i3.ElectricalTerm(name='cathode')
            return terms

    class CircuitModel(i3.CircuitModelView):
        R = i3.NumberProperty(doc="Responsivity (A/W)", default=0.8)
        BW = i3.NumberProperty(doc="3 dB bandwidth of the photodetector (GHz)", default=60)
        # Optical parameters
        reflection_dB = i3.NonNegativeNumberProperty(doc="Reflection (dB)", default=100.0)
        # Dark current parameters
        dark_current = i3.NumberProperty(doc='Dark current (nA)', default=50)

        def _generate_model(self):
            from aeponyx.compactmodels import PhotoDetectorCompactModel
            return PhotoDetectorCompactModel(R=self.R, BW=self.BW,
                                             reflection_dB=self.reflection_dB,
                                             dark_current=self.dark_current)

