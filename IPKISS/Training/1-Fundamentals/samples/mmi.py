from aeponyx import technology
from ipkiss3 import all as i3
from aeponyx.components.waveguides.wire import SiWireWaveguideTemplate
from numpy import sqrt, exp, pi

class MMI(i3.PCell):
    """
    MMI with a variable number of inputs and outputs with variable width and length of the taper.
    """
    n_inputs = i3.PositiveIntProperty(default=1, doc="Number of inputs")
    n_outputs = i3.PositiveIntProperty(default=2, doc="Number of outputs")
    trace_template = i3.TraceTemplateProperty(doc="Trace template at the ports")

    def _default_trace_template(self):
        return SiWireWaveguideTemplate(name=self.name + "tt")

    class Layout(i3.LayoutView):

        length = i3.PositiveNumberProperty(default=25.0, doc="Length of the mmi.")
        width = i3.PositiveNumberProperty(default=5.0, doc="Width of core layer at center of the mmi.")
        transistion_length = i3.PositiveNumberProperty(default=5.0, doc="Length of the tapers.")
        wg_spacing_in = i3.NonNegativeNumberProperty(default=2.0,
                                                     doc="Center to center distance between the waveguides at the input.")
        wg_spacing_out = i3.NonNegativeNumberProperty(default=2.0,
                                                      doc="Center to center distance between the waveguides at the output.")
        taper_width = i3.PositiveNumberProperty(default=1.0,
                                                doc="Width of the core of the taper of the access waveguides of the mmi.")

        @i3.cache()
        def _get_input_taper_coords(self):
            # coordinates of the input tapers [(taper1_begin, taper1_end), (taper2_begin, taper2_end), ...]
            base_y = - (self.n_inputs - 1) * self.wg_spacing_in / 2.0
            taper_coords = [
                [(0, base_y + cnt * self.wg_spacing_in), (-self.transistion_length, base_y + cnt * self.wg_spacing_in)]
                for cnt in range(self.n_inputs)]

            return taper_coords

        @i3.cache()
        def _get_output_taper_coords(self):
            # coordinates of the output tapers [(taper1_begin, taper1_end), (taper2_begin, taper2_end), ...]
            base_y = - (self.n_outputs - 1) * self.wg_spacing_out / 2.0
            taper_coords = [[(self.length, base_y + cnt * self.wg_spacing_out),
                             (self.length + self.transistion_length, base_y + cnt * self.wg_spacing_out)]
                            for cnt in range(self.n_outputs)]

            return taper_coords

        def _generate_elements(self, elems):
            layer_core = self.trace_template.core_layer
            # mmi core
            elems += i3.Rectangle(layer=layer_core, center=(+self.length / 2.0, 0.0),
                                  box_size=(self.length, self.width))
            # input wedges
            for bc, ec in self._get_input_taper_coords():
                elems += i3.Wedge(layer_core, begin_coord=bc, end_coord=ec, begin_width=self.taper_width,
                                  end_width=self.trace_template.core_width)
            for bc, ec in self._get_output_taper_coords():
                elems += i3.Wedge(layer_core, begin_coord=bc, end_coord=ec, begin_width=self.taper_width,
                                  end_width=self.trace_template.core_width)
            return elems

        def _generate_ports(self, ports):

            for cnt, coords in enumerate(self._get_input_taper_coords(), 1):
                ports += i3.OpticalPort(name="in{}".format(cnt), position=coords[1], angle=180.0,
                                        trace_template=self.trace_template)

            for cnt, coords in enumerate(self._get_output_taper_coords(), 1):
                ports += i3.OpticalPort(name="out{}".format(cnt), position=coords[1], angle=0.0,
                                        trace_template=self.trace_template)

            return ports

    class Netlist(i3.NetlistFromLayout):
        pass


class MMI_12(MMI):
    n_inputs = i3.LockedProperty()
    n_outputs = i3.LockedProperty()
    def _default_n_inputs(self):
        return 1

    def _default_n_outputs(self):
        return 2


class MMI_21(MMI):

    def _default_n_inputs(self):
        return 2

    def _default_n_outputs(self):
        return 1


class MMI_22(MMI):

    def _default_n_inputs(self):
        return 2

    def _default_n_outputs(self):
        return 2


class SquareMMI(MMI):
    class Layout(MMI.Layout):
        width = i3.LockedProperty()

        def _default_width(self):
            return self.length


class OptimizedMMI(MMI):
    class Layout(MMI.Layout):

        def _default_length(self):
            return #fancy physics at work)


sq_mmi = SquareMMI()
sq_mmi_lv = sq_mmi.Layout(length=20.0)
sq_mmi_lv.visualize()
sq_mmi_lv.length = 10.0
sq_mmi_lv.visualize()


mmi = MMI()
mmi_lv = mmi.Layout()
mmi_lv.visualize(annotate=True)

mmi12 = MMI_12()
mmi12_lv = mmi12.Layout()
mmi12_lv.visualize(annotate=True)

mmi21 = MMI_21()
mmi21_lv = mmi21.Layout()
mmi21_lv.visualize(annotate=True)

mmi22 = MMI_22()
mmi22_lv = mmi22.Layout()
mmi22_lv.visualize(annotate=True)
