"""Example fiber grating coupler.

This component is based on the standard picazzo fiber coupler with curved grating. We define two classes:

- SPGCCurved
  Inherits from FiberCouplerCurvedGrating, and specifies the default trace template of the output port and the correct process/purposes.

- FC_TE_1550:
  Example of a fixed component (using the i3.lock_properties decorator). Several layout parameters and circuit model
  parameters are fixed for this component.

"""
import ipkiss3.all as i3
from picazzo3.fibcoup.curved import FiberCouplerCurvedGrating
from aeponyx.components.waveguides.wire import SWG450
import numpy as np

__all__ = ["SPGCCurved", "FC_TE_1550"]


class SPGCCurved(FiberCouplerCurvedGrating):

    def _default_trace_template(self):
        return SWG450()

    class Layout(FiberCouplerCurvedGrating.Layout):

        def _default_process(self):
            return i3.TECH.PROCESS.SHALLOW

        def _default_purpose(self):
            return i3.TECH.PURPOSE.DRAWING

    class CircuitModel(i3.CircuitModelView):
        center_wavelength = i3.PositiveNumberProperty(doc="center wavelength (um)")
        bandwidth_1dB = i3.PositiveNumberProperty(doc="1dB bandwidth (um)")
        peak_IL_dB = i3.NonNegativeNumberProperty(doc="peak insertion loss (dB)")
        reflection = i3.ComplexFractionProperty(doc="Complex reflection back into the waveguide")
        reflection_vertical_in = i3.ComplexFractionProperty(doc="Complex reflection back into the vertical direction")

        def _generate_model(self):
            from aeponyx.compactmodels import GratingCouplerCompactModel
            return GratingCouplerCompactModel(center_wavelength=self.center_wavelength,
                                              bandwidth_1dB=self.bandwidth_1dB,
                                              peak_IL_dB=self.peak_IL_dB,
                                              reflection=self.reflection,
                                              reflection_vertical_in=self.reflection_vertical_in)


@i3.lock_properties()
class FC_TE_1550(SPGCCurved):

    """Curved grating coupler for 1550.

    Important note: the values used for this layout are not optimized for 1550 nm. They just serve as an example for aeponyx.
    """

    def _default_name(self):
        return "FC_TE_1550"


    class Layout(SPGCCurved.Layout):

        def _default_period_x(self):
            return 0.8

        def _default_focal_distance_x(self):
            return 20.0

        def _default_n_o_lines(self):
            return 30

        def _default_min_x(self):
            return 10.0

    class CircuitModel(SPGCCurved.CircuitModel):

        def _default_center_wavelength(self):
            return 1.55

        def _default_bandwidth_1dB(self):
            return 0.0248

        def _default_peak_IL_dB(self):
            return 3.39

        def _default_reflection(self):
            return 0.

        def _default_reflection_vertical_in(self):
            return 0.

