from ipkiss.technology import get_technology
from ipkiss.process.layer import PurposeProperty, ProcessProperty
from ipcore.properties.predefined import PositiveNumberProperty, NumberProperty, NonNegativeNumberProperty
from ipcore.properties.lock_properties import lock_properties
from ipkiss3.pcell.photonics.rounded_waveguide import RoundedWaveguide
from ipkiss3.simulation.engines.caphe_circuit_sim.pcell_views.caphemodel import CircuitModelView
from picazzo3.traces import twoshape, wire_wg
import numpy as np

# Do not import ipkiss3.all here to avoid cyclic imports
# In this case, caused by loading i3, which loads the autotaperdatabase, which (due to a delayed init tree) imports this file.

__all__ = ["MSNWireWaveguideTemplate", "SWG450", "SWG550", "SWG1000", "RoundedWireWaveguide"]

TECH = get_technology()

class MSNWireWaveguide(twoshape.CoreCladdingShapeWaveguide):
    """A waveguide based on picazzo3's waveguide with a core and cladding layer.
    We replace the circuit model with one that takes phase error into account.
    """

    class CircuitModel(twoshape.CoreCladdingShapeWaveguide.CircuitModel):
        phase_error = NumberProperty()

        def _default_phase_error(self):
            if not hasattr(self.template, 'phase_error_sigma'):
                import warnings
                warnings.warn('Trace template has no phase_error_sigma variable, setting it to 0')
                return 0

            phase_error_sigma = self.template.phase_error_sigma

            if phase_error_sigma == 0.0:
                return 0.0

            return np.random.normal(0.0, phase_error_sigma)


        def _generate_model(self):
            from aeponyx.compactmodels import WGPhaseErrorCompactModel
            model = WGPhaseErrorCompactModel(n_g=self.template.n_g,
                                             n_eff=self.template.n_eff,
                                             center_wavelength=self.template.center_wavelength,
                                             length=self.length,
                                             phase_error=self.phase_error)
            return model


class MSNWireWaveguideTemplate(wire_wg.WireWaveguideTemplate):
    _name_prefix = "aeponyx_WIRE_WGTEMPLATE"

    _templated_class = MSNWireWaveguide

    class Layout(wire_wg.WireWaveguideTemplate.Layout):
        core_width = PositiveNumberProperty(doc="Width of the core of the wire waveguide.")
        trench_width = PositiveNumberProperty(doc="Lateral material exclusion.")
        cladding_width = PositiveNumberProperty(doc="Cladding width of the wire waveguide.")

        core_process = ProcessProperty(locked=True, doc="Process used for the core layer of the MSN wire waveguide.")
        core_purpose = PurposeProperty(locked=True, doc="Purpose used for the core layer of the MSN wire waveguide.")

        trench_process = ProcessProperty(locked=True, doc="Process used for the exclusion layer of the MSN wire waveguide.")
        trench_purpose = PurposeProperty(locked=True, doc="Purpose used for the exclusion layer of the MSN wire waveguide.")

        cladding_process = ProcessProperty(locked=True, doc="Process used for the cladding layer of the MSN wire waveguide.")
        cladding_purpose = PurposeProperty(locked=True, doc="Purpose used for the cladding layer of the MSN wire waveguide.")

        def _default_core_process(self):
            return TECH.PROCESS.MSN

        def _default_core_purpose(self):
            return TECH.PURPOSE.DRAWING

        def _default_trench_process(self):
            return TECH.PROCESS.MSN_TRENCH

        def _default_trench_purpose(self):
            return TECH.PURPOSE.DRAWING

        def _default_cladding_process(self):
            return TECH.PROCESS.CLADDING

        def _default_cladding_purpose(self):
            return TECH.PURPOSE.DRAWING

        def _default_core_width(self):
            return TECH.WIREWG.WIRE_WIDTH

        def _default_trench_width(self):
            return TECH.WIREWG.TRENCH_WIDTH

        def _default_cladding_width(self):
            return TECH.WIREWG.CLADDING_WIDTH



    class CircuitModel(wire_wg.WireWaveguideTemplate.CircuitModel):
        phase_error_sigma = NonNegativeNumberProperty(default=0.0)


@lock_properties()
class SWG450(MSNWireWaveguideTemplate):

    def _default_name(self):
        return "SWG450"

    class Layout(MSNWireWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.45

        def _default_trench_width(self):
            return 20

        def _default_cladding_width(self):
            return 10

    class CircuitModel(MSNWireWaveguideTemplate.CircuitModel):

        def _default_n_eff(self):
            return 2.35

        def _default_n_g(self):
            return 4.3

        def _default_loss_dB_m(self):
            return 210


@lock_properties()
class SWG550(MSNWireWaveguideTemplate):

    def _default_name(self):
        return "SWG550"

    class Layout(MSNWireWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.55

        def _default_trench_width(self):
            return 10

    class CircuitModel(MSNWireWaveguideTemplate.CircuitModel):

        def _default_n_eff(self):
            return 2.4

        def _default_n_g(self):
            return 3.8

        def _default_loss_dB_m(self):
            return 150


@lock_properties()
class SWG1000(MSNWireWaveguideTemplate):

    def _default_name(self):
        return "SWG1000"

    class Layout(MSNWireWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 1.0

        def _default_trench_width(self):
            return 10

    class CircuitModel(MSNWireWaveguideTemplate.CircuitModel):

        def _default_n_eff(self):
            return 2.6

        def _default_n_g(self):
            return 3.2

        def _default_loss_dB_m(self):
            return 100


class RoundedWireWaveguide(RoundedWaveguide):

    def _default_trace_template(self):
        return SWG450()

    class Layout(RoundedWaveguide.Layout):

        def _default_bend_radius(self):
            return TECH.WIREWG.BEND_RADIUS

