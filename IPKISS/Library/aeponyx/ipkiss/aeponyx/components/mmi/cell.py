from ipkiss3 import all as i3
from aeponyx.components.waveguides.wire import MSNWireWaveguideTemplate


class MMI(i3.PCell):

    """
    MMI with a variable number of inputs and outputs with variable width and length of the taper.
    """
    n_inputs = i3.PositiveIntProperty(default=1, doc="Number of inputs")
    n_outputs = i3.PositiveIntProperty(default=2, doc="Number of outputs")
    trace_template = i3.TraceTemplateProperty(doc="Trace template at the ports")

    def _default_trace_template(self):
        return MSNWireWaveguideTemplate(name=self.name + "tt")

    class Layout(i3.LayoutView):

        length = i3.PositiveNumberProperty(default=25.0, doc="Length of the mmi.")
        width = i3.PositiveNumberProperty(default=5.0, doc="Width of core layer at center of the mmi.")
        transition_length = i3.PositiveNumberProperty(default=5.0, doc="Length of the tapers.")
        wg_spacing_in = i3.NonNegativeNumberProperty(default=2.0, doc="Center to center distance between the waveguides at the input.")
        wg_spacing_out = i3.NonNegativeNumberProperty(default=2.0, doc="Center to center distance between the waveguides at the output.")
        taper_width = i3.PositiveNumberProperty(default=1.0, doc="Width of the core of the taper of the access waveguides of the mmi.")
        cladding_width = i3.PositiveNumberProperty(doc="Width of the cladding square on the entire MMI.")

        def _default_cladding_width(self):
            return self.width + self.trace_template.cladding_width - self.trace_template.core_width

        @i3.cache()
        def _get_input_taper_coords(self):
            # coordinates of the input tapers [(taper1_begin, taper1_end), (taper2_begin, taper2_end), ...]
            base_y = - (self.n_inputs - 1) * self.wg_spacing_in / 2.0
            taper_coords = [[(0, base_y + cnt * self.wg_spacing_in), (-self.transition_length, base_y + cnt * self.wg_spacing_in)]
                             for cnt in range(self.n_inputs)]

            return taper_coords

        @i3.cache()
        def _get_output_taper_coords(self):
            # coordinates of the output tapers [(taper1_begin, taper1_end), (taper2_begin, taper2_end), ...]
            base_y = - (self.n_outputs - 1) * self.wg_spacing_out / 2.0
            taper_coords = [[(self.length,  base_y + cnt * self.wg_spacing_out), (self.length + self.transition_length, base_y + cnt * self.wg_spacing_out)]
                             for cnt in range(self.n_outputs)]

            return taper_coords

        def _generate_elements(self, elems):
            layer_core = self.trace_template.core_layer
            layer_cladding = i3.PPLayer(self.trace_template.cladding_process, self.trace_template.cladding_purpose)
            # mmi core
            elems += i3.Rectangle(layer=layer_core, center=(+self.length / 2.0, 0.0), box_size=(self.length, self.width))
            # input wedges
            for bc, ec in self._get_input_taper_coords():
                elems += i3.Wedge(layer_core, begin_coord=bc, end_coord=ec, begin_width=self.taper_width, end_width=self.trace_template.core_width)
            for bc, ec in self._get_output_taper_coords():
                elems += i3.Wedge(layer_core, begin_coord=bc, end_coord=ec, begin_width=self.taper_width, end_width=self.trace_template.core_width)
            # cladding layer
            elems += i3.Rectangle(layer=layer_cladding, center=(+self.length / 2.0, 0.0), box_size=(self.length + 2 * self.transition_length, self.cladding_width))

            return elems

        def _generate_ports(self, ports):

            for cnt, coords in enumerate(self._get_input_taper_coords(), 1):
                ports += i3.OpticalPort(name="in{}".format(cnt), position=coords[1], angle=180.0, trace_template=self.trace_template)

            for cnt, coords in enumerate(self._get_output_taper_coords(), 1):
                ports += i3.OpticalPort(name="out{}".format(cnt), position=coords[1], angle=0.0, trace_template=self.trace_template)

            return ports


class MMI_12(MMI):

    def _default_n_inputs(self):
        return 1

    def _default_n_outputs(self):
        return 2
    class Netlist(i3.NetlistView):

        def _generate_terms(self, terms):
            terms += i3.OpticalTerm(name='in1')
            terms += i3.OpticalTerm(name='out1')
            terms += i3.OpticalTerm(name='out2')
            return terms

    class CircuitModel(i3.CircuitModelView):
        excess_loss_dB = i3.NumberProperty(default=0.01, doc='power loss in the MMI (sum of the two outputs) (dB)')
        power_imbalance_dB = i3.NumberProperty(default=0.0, doc='power difference between the output ports (10 * log10(P_out_1 / P_out_0) (dB)')
        reflection_dB = i3.NonNegativeNumberProperty(default=2000, doc='Reflection (dB)')

        def _generate_model(self):
            from aeponyx.compactmodels import MMI1x2CompactModel
            return MMI1x2CompactModel(excess_loss_dB=self.excess_loss_dB,
                                      power_imbalance_dB=self.power_imbalance_dB,
                                      reflection_dB=self.reflection_dB)

class MMI_22(MMI):

    def _default_n_inputs(self):
        return 2

    def _default_n_outputs(self):
        return 2
    class Netlist(i3.NetlistView):

        def _generate_terms(self, terms):
            terms += i3.OpticalTerm(name='in1')
            terms += i3.OpticalTerm(name='in2')
            terms += i3.OpticalTerm(name='out1')
            terms += i3.OpticalTerm(name='out2')
            return terms

    class CircuitModel(i3.CircuitModelView):
        excess_loss_dB = i3.NumberProperty(doc='power loss in the MMI (sum of the two outputs) (dB)')
        power_imbalance_dB = i3.NumberProperty(doc='power difference between the output ports (10 * log10(P_out_1 / P_out_0) for in_1 (dB)')
        reflection_dB = i3.NonNegativeNumberProperty(doc='Reflection (dB)')

        def _generate_model(self):
            from aeponyx.compactmodels import MMI2x2CompactModel
            return MMI2x2CompactModel(excess_loss_dB=self.excess_loss_dB,
                                      power_imbalance_dB=self.power_imbalance_dB,
                                      reflection_dB=self.reflection_dB)
