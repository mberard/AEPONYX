from ipkiss.technology import get_technology
from ipkiss.process.layer import PurposeProperty, ProcessProperty
from ipcore.properties.predefined import PositiveNumberProperty, NumberProperty, NonNegativeNumberProperty
from ipkiss.primitives.layer import LayerProperty
from ipcore.properties.lock_properties import lock_properties
from ipkiss3.pcell.photonics.rounded_waveguide import RoundedWaveguide
import numpy as np
from ..generic.trace import GenericWaveGuideTemplate
import os
# Do not import ipkiss3.all here to avoid cyclic imports
# In this case, caused by loading i3, which loads the autotaperdatabase, which (due to a delayed init tree) imports this file.

__all__ = ["MSNWireWaveguideTemplate", "SWG450", "SWG550", "SWG1000", "RoundedWireWaveguide"]

TECH = get_technology()
from ipkiss3.pcell.trace.window.window import PathTraceWindow

class MSNWireWaveguideTemplate(GenericWaveGuideTemplate):

    class Layout(GenericWaveGuideTemplate.Layout):

        cladding_width = PositiveNumberProperty(default=2.0)
        exclusion_width= PositiveNumberProperty(default=20.0)
        cladding_layer = LayerProperty()
        exclusion_layer = LayerProperty()
        
        def _default_exclusion_layer(self):
            return TECH.PPLAYER.MSN_TRENCH
        
        def _default_core_layer(self):
            return TECH.PPLAYER.MSN

        def _default_cladding_layer(self):
            return TECH.PPLAYER.CLADDING


        def _default_core_width(self):
            return 0.45

        def _default_windows(self):
            windows = [PathTraceWindow(layer=self.core_layer,
                                       start_offset=-0.5*self.core_width,
                                       end_offset=0.5*self.core_width),
                       PathTraceWindow(layer=self.cladding_layer,
                                       start_offset=-0.5 * self.cladding_width,
                                       end_offset=0.5 * self.cladding_width),
                       PathTraceWindow(layer=self.exclusion_layer,
                                       start_offset=-0.5 * self.exclusion_width,
                                       end_offset=0.5 * self.exclusion_width)
                       ]

            windows = [PathTraceWindow(layer=layer,
                                       start_offset=-off/2.0,
                                       end_offset=off/2.0) for off ,layer in zip([1,2.,3.0,5.0,9.0],[self.core_layer, self.cladding_layer,self.exclusion_layer])]
            return windows

    class CircuitModel(GenericWaveGuideTemplate.CircuitModel):

        def _default_n_eff_values(self):

            n_eff_values= np.loadtxt(os.path.join(os.path.dirname(__file__),"neff_si_wire.txt"), delimiter=",")
            # The complex part of the refractive index is dominated by scattering
            db_cm = 3.0
            loss_1cm = 10**(-db_cm/20)
            wav = 1.55
            imag_neff = - np.log(loss_1cm) * wav/1e4 * 1/(2*np.pi)
            n_eff_values = n_eff_values + imag_neff*1j # Adding some loss. Here non disperisve scattering loss.

            return n_eff_values

        def _default_phase_error_width_deviation(self):
            return 0.000

        def _default_phase_error_correlation_length(self):
            return 0.0

        def _default_wavelengths(self):
            return np.linspace(1.5, 1.6, 5)

        def _default_widths(self):
            return np.linspace(0.4, 1.2, 6)



@lock_properties()
class SWG450(MSNWireWaveguideTemplate):

    def _default_name(self):
        return "SWG450"

    class Layout(MSNWireWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.45


@lock_properties()
class SWG550(MSNWireWaveguideTemplate):

    def _default_name(self):
        return "SWG550"

    class Layout(MSNWireWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.55



@lock_properties()
class SWG1000(MSNWireWaveguideTemplate):

    def _default_name(self):
        return "SWG1000"

    class Layout(MSNWireWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 1.0


class RoundedWireWaveguide(RoundedWaveguide):

    def _default_trace_template(self):
        return SWG450()

    class Layout(RoundedWaveguide.Layout):

        def _default_bend_radius(self):
            return TECH.WIREWG.BEND_RADIUS

