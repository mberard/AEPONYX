"""Example of Travelling Wave Mach-Zehnder Modulator, built from picazzo3 components and trace templates from aeponyx.

The modulator is built using PlacAndAutoRoute. This allows to specify the splitter, combiner,
and cells to place within the MZI arms.
"""

__all__ = ['TW_MZM']

from ipkiss3 import all as i3

from ..waveguides.wire.trace import SWG450
from ..mmi.cell import MMI_12
from .traces import PhaseShifterWaveguide
from picazzo3.routing.place_route import PlaceAndAutoRoute


class TW_MZM(PlaceAndAutoRoute):
    """Travelling Wave Mach-Zehnder Modulator made with PlaceAndAutoRoute
    """

    modulator_length = i3.PositiveNumberProperty(default=500., doc='Length of the PN junction [um]')
    modulator = i3.ChildCellProperty(locked=True)
    _name_prefix = 'TW_MZM'

    def _default_trace_template(self):
        return SWG450()

    def _default_modulator(self):
        return PhaseShifterWaveguide(name=self.name + '_phaseshifter')

    def _default_child_cells(self):
        return {
            'splitter': MMI_12(),
            'arm1': self.modulator,
            'arm2': self.modulator,
            'combiner': MMI_12(),
        }

    def _default_links(self):
        return [
            ('splitter:out1', 'arm1:in'),
            ('arm1:out', 'combiner:out1'),
            ('splitter:out2', 'arm2:in'),
            ('arm2:out', 'combiner:out2'),
        ]

    def _default_external_port_names(self):
        return{
            'splitter:in1': 'in',
            'combiner:in1': 'out',
        }

    class Layout(PlaceAndAutoRoute.Layout):
        delta = i3.PositiveNumberProperty(default=60., doc='Offset of the arm with the 0 (@ 0 delay) [um]')
        ground_width = i3.PositiveNumberProperty(default=80., doc="Width of exteriors grounds line [um]")
        signal_width = i3.PositiveNumberProperty(default=10., doc="Width of signal line [um]")
        metal_spacing = i3.PositiveNumberProperty(default=4., doc="Spacing between 2 metal lines [um]")
        delay = i3.NumberProperty(default=0., doc='Delay between the 2 arms (0< in bottom arm, >0 in top arm) [um]')
        additional_length = i3.NonNegativeNumberProperty(default=50., doc="Additional length of the metal lines [um]")
        pad_width = i3.PositiveNumberProperty(default=50., doc='Width of the electrical pads [um]')
        pad_length = i3.PositiveNumberProperty(default=70., doc='Length of the electrical pads [um]')
        taper_length = i3.PositiveNumberProperty(default=60., doc='Length of the tapers between the pads and the lines [um]')
        period_pad = i3.PositiveNumberProperty(default=70., doc='Period of the pads [um]')

        def _default_modulator(self):
            lv = self.cell.modulator.get_default_view(self)
            lv.set(shape=[(0., 0.), (self.modulator_length, 0.)])
            return lv

        def _generate_elements(self, elems):
            super(PlaceAndAutoRoute.Layout, self)._generate_elements(elems)

            total_length = self.modulator_length + self.additional_length
            delta = self.delta
            delay = self.delay

            signal_width = self.signal_width
            metal_spacing = self.metal_spacing
            ground_width = self.ground_width

            period_pad = self.period_pad
            taper_length = self.taper_length
            pad_length = self.pad_length
            pad_width = self.pad_width
            pad_size = (pad_length, pad_width)

            y_ground = -delta + min(delay/2., 0.) - signal_width - 1.5 * metal_spacing - ground_width/2.
            y_arm1_n = -delta + min(delay/2., 0.) - signal_width/2. - metal_spacing/2.
            y_arm1_p = delay/4. - signal_width/2. - metal_spacing/2.
            y_arm2_n = delta + max(delay/2., 0.) - signal_width/2. - metal_spacing/2.
            y_arm2_p = delta + max(delay/2., 0.) + ground_width/2. + metal_spacing/2.

            arm2_n_width = 2*delta + abs(delay/2.) - signal_width - 2*metal_spacing

            # Metal lines
            lines = []
            ground = i3.Rectangle(
                layer=i3.TECH.PPLAYER.M1,
                center=(0., y_ground),
                box_size=(total_length, ground_width)
            )
            arm1_n = i3.Rectangle(
                layer=i3.TECH.PPLAYER.M1,
                center=(0., y_arm1_n),
                box_size=(total_length, signal_width)
            )
            arm1_p = i3.Rectangle(
                layer=i3.TECH.PPLAYER.M1,
                center=(0., y_arm1_p),
                box_size=(total_length, arm2_n_width)
            )
            arm2_n = i3.Rectangle(
                layer=i3.TECH.PPLAYER.M1,
                center=(0., y_arm2_n),
                box_size=(total_length, signal_width)
            )
            arm2_p = i3.Rectangle(
                layer=i3.TECH.PPLAYER.M1,
                center=(0., y_arm2_p),
                box_size=(total_length, ground_width)
            )

            lines += [ground, arm1_n, arm1_p, arm2_n, arm2_p]

            # Metal left pads
            left_pads = []
            taper_ground = i3.Boundary(
                layer=i3.TECH.PPLAYER.M1,
                shape=[
                    (-(total_length/2. + taper_length), y_arm1_p - 2*period_pad + pad_width/2.),
                    (-(total_length/2. + taper_length), y_arm1_p - 2*period_pad - pad_width/2.),
                    (-total_length/2., y_ground - ground_width/2.),
                    (-total_length/2., y_ground + ground_width/2.),
                ]
            )
            taper_arm1_n = i3.Boundary(
                layer=i3.TECH.PPLAYER.M1,
                shape=[
                    (-(total_length/2. + taper_length), y_arm1_p - period_pad + pad_width/2.),
                    (-(total_length/2. + taper_length), y_arm1_p - period_pad - pad_width/2.),
                    (-total_length/2., y_arm1_n - signal_width/2.),
                    (-total_length/2., y_arm1_n + signal_width/2.),
                ]
            )
            taper_arm1_p = i3.Wedge(
                layer=i3.TECH.PPLAYER.M1,
                begin_coord=(-(total_length/2. + taper_length), y_arm1_p),
                end_coord=(-total_length/2., y_arm1_p),
                begin_width=pad_width,
                end_width=arm2_n_width,
            )
            taper_arm2_n = i3.Boundary(
                layer=i3.TECH.PPLAYER.M1,
                shape=[
                    (-(total_length/2. + taper_length), y_arm1_p + period_pad + pad_width/2.),
                    (-(total_length/2. + taper_length), y_arm1_p + period_pad - pad_width/2.),
                    (-total_length/2., y_arm2_n - signal_width/2.),
                    (-total_length/2., y_arm2_n + signal_width/2.),
                ]
            )
            taper_arm2_p = i3.Boundary(
                layer=i3.TECH.PPLAYER.M1,
                shape=[
                    (-(total_length/2. + taper_length), y_arm1_p + 2*period_pad + pad_width/2.),
                    (-(total_length/2. + taper_length), y_arm1_p + 2*period_pad - pad_width/2.),
                    (-total_length/2., y_arm2_p - ground_width/2.),
                    (-total_length/2., y_arm2_p + ground_width/2.),
                ]
            )

            for i in range(-2, 3):
                left_pads += i3.Rectangle(
                    layer=i3.TECH.PPLAYER.M1,
                    center=(-(total_length/2. + taper_length + pad_length/2.), y_arm1_p + i * period_pad),
                    box_size=pad_size
                )

            left_pads += [taper_ground, taper_arm1_n, taper_arm1_p, taper_arm2_n, taper_arm2_p]

            # Metal right pads
            right_pads = [elt.transform_copy(i3.HMirror(0.)) for elt in left_pads]

            elems += lines
            elems += left_pads
            elems += right_pads
            return elems

        def _generate_ports(self, ports):
            # Calling super() to inherit ports from non-connected subblocks (i.e., splitter optical input port and combiner optical output port)
            super(PlaceAndAutoRoute.Layout, self)._generate_ports(ports)

            modulator_length = self.modulator_length
            additional_length = self.additional_length
            pad_length = self.pad_length
            taper_length = self.taper_length
            period_pad = self.period_pad

            y_center_pad = self.delay/4. - 7.

            for i in range(5):
                ports += i3.ElectricalPort(
                    name='elec_left_{}'.format(i + 1),
                    position=(-((modulator_length + additional_length + pad_length)/2. + taper_length), y_center_pad + (i - 2) * period_pad),
                    layer=i3.TECH.PPLAYER.M1
                )
                ports += i3.ElectricalPort(
                    name='elec_right_{}'.format(i + 1),
                    position=((modulator_length + additional_length + pad_length)/2. + taper_length, y_center_pad + (i - 2) * period_pad),
                    layer=i3.TECH.PPLAYER.M1
                )
            return ports

        def _default_child_transformations(self):
            offset_x = self.modulator_length/2.
            delta = self.delta
            delay = self.delay
            
            return{
                'splitter': i3.Translation((-(50. + offset_x), 0.)),
                'arm1': i3.Translation((-offset_x, -delta + (delay/2. if (delay < 0) else 0.))),
                'arm2': i3.Translation((-offset_x, delta + (delay/2. if (delay >= 0) else 0.))),
                'combiner': i3.HMirror(0.) + i3.Translation((50. + offset_x, 0.))
            }

    class Netlist(PlaceAndAutoRoute.Netlist):

        def _generate_terms(self, terms):
            # Calling super() to inherit terms from non-connected subblocks (i.e., splitter optical input term and combiner optical output term)
            super(PlaceAndAutoRoute.Netlist, self)._generate_terms(terms)
            for i in range(5):
                terms['elec_left_{}'.format(i + 1)] = i3.ElectricalTerm(name='elec_left_{}'.format(i + 1))
                terms['elec_right_{}'.format(i + 1)] = i3.ElectricalTerm(name='elec_right_{}'.format(i + 1))
            return terms

    class CircuitModel(i3.CircuitModelView):
        n_eff = i3.PositiveNumberProperty(locked=True)
        VpiLpi = i3.PositiveNumberProperty(default=1.5, doc="Voltage to get pi-shift for 1 cm of phase modulator [V.cm]")
        delay = i3.NumberProperty(locked=True)
        insertion_loss_dB = i3.NonNegativeNumberProperty(default=0., doc='Insertion loss [dB]')

        def _default_n_eff(self):
            tt_cm = self.trace_template
            return tt_cm.n_eff

        def _default_delay(self):
            return self.layout_view.delay

        def _generate_model(self):
            from aeponyx.compactmodels import MZMLumpedCompactModel
            return MZMLumpedCompactModel(n_eff=self.n_eff, delay=self.delay, insertion_loss_dB=self.insertion_loss_dB, VpiLpi=self.VpiLpi, length=self.modulator_length)
