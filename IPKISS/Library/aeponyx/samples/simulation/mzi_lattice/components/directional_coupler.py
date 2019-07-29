from ipkiss3 import all as i3
from aeponyx.all import SiWireWaveguideTemplate
import numpy as np


class DirCoupModel(i3.CompactModel):
    """
    Non-dispersive symmetric directional coupler model
    """
    parameters = [
        'delta_n',                 # Difference in effective index between the even and odd mode
        'n_avg',                   # Average effective index
        'coupler_length',          # Length of the coupling section
        'non_coupling_length',     # Length of the directional coupler that is not part of the coupling
        'coupling_at_zero_length', # Coupling for a zero length coupling
        'center_wavelength'        # Reference wavelength for all the parameters above
    ]

    terms = [
        i3.OpticalTerm(name='in1'),
        i3.OpticalTerm(name='in2'),
        i3.OpticalTerm(name='out1'),
        i3.OpticalTerm(name='out2'),
    ]

    def calculate_smatrix(parameters, env, S):
        delta_n = parameters.delta_n
        l0 = parameters.center_wavelength * np.arcsin(parameters.coupling_at_zero_length ) / (np.pi * delta_n)
        phi_0 = np.exp(1j * 2 * np.pi * parameters.n_avg * parameters.non_coupling_length / env.wavelength)
        phi_coupler = np.exp(1j * 2 * np.pi * parameters.n_avg * parameters.coupler_length / env.wavelength)

        delta_beta_x_length = 2 * np.pi / env.wavelength * delta_n * (parameters.coupler_length + l0)

        # Crossing transmission
        cross = 0.5 * (1.0 - np.exp(1j * delta_beta_x_length)) * phi_coupler * phi_0
        S['in1', 'out2'] = S['out2', 'in1'] = cross
        S['in2', 'out1'] = S['out1', 'in2'] = cross

        # Straight transmission
        straight = 0.5 * (1.0 + np.exp(1j * delta_beta_x_length)) * phi_coupler * phi_0
        S['in1', 'out1'] = S['out1', 'in1'] = straight
        S['in2', 'out2'] = S['out2', 'in2'] = straight


class DirectionalCoupler(i3.PCell):
    trace_template = i3.TraceTemplateProperty(doc="Trace template used for directional coupler")

    def _default_trace_template(self):
        return SiWireWaveguideTemplate()

    class Layout(i3.LayoutView):
        spacing = i3.PositiveNumberProperty(default=0.2, doc="Spacing between the waveguides", locked=True)
        bend_radius = i3.PositiveNumberProperty(default=10.0, doc="Bend radius of the directional coupler", locked=True)

        def _generate_instances(self, insts):
            delta = self.trace_template.core_width + self.spacing
            bend_radius = self.bend_radius
            coupler_length = self.cell.get_default_view(i3.CircuitModelView).coupler_length

            shape = i3.Shape([
                (-coupler_length / 2.0 - bend_radius, bend_radius),
                (-coupler_length / 2.0 - bend_radius, 0.0),
                (-coupler_length / 2.0, 0.0),
                (coupler_length / 2.0, 0.0),
                (coupler_length / 2.0 + bend_radius, 0.0),
                (coupler_length / 2.0 + bend_radius, bend_radius)
            ])

            wg = i3.RoundedWaveguide(name=self.name + "_wg",
                                     trace_template=self.trace_template)
            wg_layout = wg.Layout(shape=shape)


            insts += i3.SRef(reference=wg_layout,
                             name="wav_top",
                             position=(0, delta/2.0))

            insts += i3.SRef(reference=wg_layout,
                             name="wav_bot",
                             transformation=i3.VMirror() + i3.Translation(translation=(0,-delta/2.0)))
            return insts

        def _generate_ports(self, ports):
            ports += self.instances["wav_top"].ports["in"].modified_copy(name="in2")
            ports += self.instances["wav_top"].ports["out"].modified_copy(name="out2")
            ports += self.instances["wav_bot"].ports["in"].modified_copy(name="in1")
            ports += self.instances["wav_bot"].ports["out"].modified_copy(name="out1")

            return ports

    class CircuitModel(i3.CircuitModelView):
        coupler_length = i3.PositiveNumberProperty(doc="Length of the straight section of the directional coupler, calculated from the power coupling", locked=True)
        power_coupling_factor = i3.FractionProperty(default=0.5, doc="Fraction of the field coupling")
        delta_n_eff = i3.NumberProperty(doc="Difference between even and odd mode of the dc")
        coupling_at_zero_length = i3.NonNegativeNumberProperty(default=0.03, doc="Field coupling for a zero length coupling", locked=True)
        center_wavelength = i3.PositiveNumberProperty(doc="Reference wavelength for all parameters", locked=True)

        def _default_center_wavelength(self):
            return self.trace_template.center_wavelength

        def _default_delta_n_eff(self):
            return 0.04

        def _default_coupler_length(self):
            L = (self.center_wavelength * np.arcsin(self.power_coupling_factor**0.5) / (np.pi * self.delta_n_eff) -
                 self.center_wavelength * np.arcsin(self.coupling_at_zero_length) / (np.pi * self.delta_n_eff))

            return L

        def _generate_model(self):
            trace_length = self.layout_view.instances['wav_top'].reference.trace_length()
            non_coupling_length = (trace_length - self.coupler_length)

            return DirCoupModel(delta_n=self.delta_n_eff,
                                n_avg=self.trace_template.n_eff,
                                coupler_length=self.coupler_length,
                                non_coupling_length=non_coupling_length,
                                coupling_at_zero_length=self.coupling_at_zero_length,
                                center_wavelength=self.center_wavelength
                                )

    class Netlist(i3.NetlistFromLayout):
        pass

