from ipkiss3.pcell.photonics.waveguide import WindowWaveguideTemplate
from ipkiss3.pcell.trace.window.window import PathTraceWindow
from ipkiss.process import PPLayer, ProcessProperty, PurposeProperty
from ipkiss.technology import get_technology
from ipcore.properties.lock_properties import lock_properties
from ipcore.properties.predefined import PositiveNumberProperty
from ipkiss3.pcell.photonics.rounded_waveguide import RoundedWaveguide
from ..generic.trace import GenericWaveGuideTemplate
from ipkiss.primitives.layer import LayerProperty
import numpy as np
import os


__all__ = ["MSNRibWaveguideTemplate", "RWG450", "RWG850"]

TECH = get_technology()


class MSNRibWaveguideTemplate(GenericWaveGuideTemplate):
    _name_prefix = "aeponyx_RIB_WGTEMPLATE"

    class Layout(GenericWaveGuideTemplate.Layout):

        cladding_width = PositiveNumberProperty()
        cladding_layer = LayerProperty()

        def _default_cladding_width(self):
            return self.core_width + 4.0

        def _default_core_layer(self):
            return TECH.PPLAYER.SI

        def _default_cladding_layer(self):
            return TECH.PPLAYER.SHALLOW

        def _default_windows(self):
            windows = [PathTraceWindow(layer=self.core_layer,
                                       start_offset=-0.5*self.core_width,
                                       end_offset=0.5*self.core_width),
                       PathTraceWindow(layer=self.cladding_layer,
                                       start_offset=-0.5 * self.cladding_width,
                                       end_offset=0.5 * self.cladding_width)
                       ]
            return windows

    class CircuitModel(GenericWaveGuideTemplate.CircuitModel):

        def _default_n_eff_values(self):
            n_eff_values = np.loadtxt(os.path.join(os.path.dirname(__file__), "neff_si_rib.txt"), delimiter=",")
            # The complex part of the refractive index is dominated by scattering
            db_cm = 1.0
            loss_1cm = 10 ** (-db_cm / 20)
            wav = 1.55
            imag_neff = - np.log(loss_1cm) * wav / 1e4 * 1 / (2 * np.pi)
            n_eff_values = n_eff_values + imag_neff * 1j  # Adding some loss. Here non disperisve material loss.

            return n_eff_values

        def _default_phase_error_width_deviation(self):
            return 0.005

        def _default_phase_error_correlation_length(self):
            return 0.5

        def _default_wavelengths(self):
            return np.linspace(1.5, 1.6, 5)

        def _default_widths(self):
            return np.linspace(0.4, 1.2, 6)


@lock_properties()
class RWG450(MSNRibWaveguideTemplate):

    def _default_name(self):
        return "RWG450"

    class Layout(MSNRibWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.45


@lock_properties()
class RWG850(MSNRibWaveguideTemplate):

    def _default_name(self):
        return "RWG850"

    class Layout(MSNRibWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.85


class RoundedRibWaveguide(RoundedWaveguide):

    def _default_trace_template(self):
        return RWG850()

    class Layout(RoundedWaveguide.Layout):

        def _default_bend_radius(self):
            return TECH.RIBWG.BEND_RADIUS


