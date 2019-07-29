__all__ = ["WireWgCrossing", "CROSSING_450"]

from picazzo3.wg.crossing import WgParabolicCrossing
import ipkiss3.all as i3
from ..waveguides.wire import MSNWireWaveguideTemplate, SWG450

from numpy import pi, exp


class WireWgCrossing(WgParabolicCrossing):

    def _default_trace_template(self):
        return SiWireWaveguideTemplate()

    class Layout(WgParabolicCrossing.Layout):

        def _default_core_process(self):
            return i3.TECH.PROCESS.SI

        def _default_strip_process(self):
            return i3.TECH.PROCESS.SHALLOW

        def _default_cladding_process(self):
            return i3.TECH.PROCESS.NONE

        def _default_core_cladding_process(self):
            return i3.TECH.PROCESS.NONE

        def _default_core_purpose(self):
            return i3.TECH.PURPOSE.DRAWING

        def _default_strip_purpose(self):
            return i3.TECH.PURPOSE.DRAWING

        def _default_cladding_purpose(self):
            return i3.TECH.PURPOSE.UNUSED

        def _default_core_cladding_purpose(self):
            return i3.TECH.PURPOSE.UNUSED

    class Netlist(i3.NetlistView):

        def _generate_terms(self, terms):
            terms += i3.OpticalTerm(name='east')
            terms += i3.OpticalTerm(name='west')
            terms += i3.OpticalTerm(name='south')
            terms += i3.OpticalTerm(name='north')
            return terms

    class CircuitModel(i3.CircuitModelView):
        length = i3.PositiveNumberProperty(doc="Lengths of the crossing waveguides used for simulation")
        neff = i3.PositiveNumberProperty(doc='Effective index (by default, the same as SWG450)')
        crosstalk_dB = i3.NonNegativeNumberProperty(doc='Crosstalk in amplitude (dB)')
        insertion_loss_dB = i3.NonNegativeNumberProperty(doc="Insertion Loss (dB)")
        reflection_dB = i3.NonNegativeNumberProperty(doc='Reflection (dB)')

        def _generate_model(self):
            from aeponyx.compactmodels import CrossingCompactModel
            return CrossingCompactModel(length=self.length, neff=self.neff,
                                        crosstalk_dB=self.crosstalk_dB,
                                        insertion_loss_dB=self.insertion_loss_dB,
                                        reflection_dB=self.reflection_dB)


@i3.lock_properties()
class CROSSING_450(WireWgCrossing):

    def _default_trace_template(self):
        return SWG450()

    class Layout(WireWgCrossing.Layout):
        def _default_length(self):
            return 6.5

    class CircuitModel(WireWgCrossing.CircuitModel):
        def _default_length(self):
            return 6.5

        def _default_neff(self):
            return 2.35

        def _default_crosstalk_dB(self):
            return 33.

        def _default_insertion_loss_dB(self):
            return 0.3

        def _default_reflection_dB(self):
            return 100.


