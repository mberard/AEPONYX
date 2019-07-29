from picazzo3.wg.dircoup import BendDirectionalCoupler
from aeponyx.components.waveguides.wire import SiWireWaveguideTemplate
from ipkiss3 import all as i3


class DirectionalCoupler(BendDirectionalCoupler):

    def _default_trace_template1(self):
        return SiWireWaveguideTemplate(name=self.name + "tt")

    def _default_trace_template2(self):
        return self.trace_template1

    def _default_coupler_length(self):
        return 6.0

    class Layout(BendDirectionalCoupler.Layout):

        def _default_coupler_spacing(self):
            return 0.7

        def _default_bend_radius(self):
            return 10.0

        def _default_bend_angles1(self):
            return (0.0, 0.0)

        def _default_wg1a_shape(self):
            return i3.Shape(points=[(-1 * self.bend_radius - self.coupler_length / 2, -self.coupler_spacing / 2), (0, -self.coupler_spacing / 2)])

        def _default_wg1b_shape(self):
            return i3.Shape(points=[(0, -self.coupler_spacing / 2), (+1 * self.bend_radius + self.coupler_length / 2, -self.coupler_spacing / 2)])

        def _default_reverse_individual_bends1a(self):
            return []

        def _default_reverse_individual_bends1b(self):
            return []

        def _default_bend_angles2(self):
            return (90.0, 90.0)

        def get_length_ring_section(self):
            """
            Gets the length of the ring section
            """
            return self.wg2a.trace_length() + self.wg2b.trace_length()
