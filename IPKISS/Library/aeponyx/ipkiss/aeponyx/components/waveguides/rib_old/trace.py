from ipkiss3.pcell.photonics.waveguide import WindowWaveguideTemplate
from ipkiss3.pcell.trace.window.window import PathTraceWindow
from ipkiss.process import PPLayer, ProcessProperty, PurposeProperty
from ipkiss.technology import get_technology
from ipcore.properties.lock_properties import lock_properties
from ipcore.properties.predefined import PositiveNumberProperty
from ipkiss3.pcell.photonics.rounded_waveguide import RoundedWaveguide
from picazzo3.traces import rib_wg

__all__ = ["MSNRibWaveguideTemplate", "RWG450", "RWG850", "RoundedRibWaveguide"]

TECH = get_technology()


class MSNRibWaveguideTemplate(rib_wg.RibWaveguideTemplate):
    _name_prefix = "aeponyx_RIB_WGTEMPLATE"

    class Layout(rib_wg.RibWaveguideTemplate.Layout):
        core_process = ProcessProperty(locked=True, doc="Process used for the core layer of the MSN rib_old waveguide.")
        core_purpose = PurposeProperty(locked=True, doc="Purpose used for the core layer of the MSN rib_old waveguide.")
        exclusion_process = ProcessProperty(locked=True, doc="Process used for the exclusion layer of the silicon wire waveguide.")
        exclusion_purpose = PurposeProperty(locked=True, doc="Purpose used for the exclusion layer of the silicon wire waveguide.")
        cladding_process = ProcessProperty(locked=True, doc="Process used for the cladding layer of the MSN rib_old waveguide.")
        cladding_purpose = PurposeProperty(locked=True, doc="Purpose used for the cladding layer of the MSN rib_old waveguide.")
        core_width = PositiveNumberProperty(doc="Width of the core of the rib_old waveguide.")
        cladding_width = PositiveNumberProperty(doc="Cladding width of the rib_old waveguide.")

        def _default_core_process(self):
            return TECH.PROCESS.MSN

        def _default_core_purpose(self):
            return TECH.PURPOSE.DRAWING

        def _default_exclusion_process(self):
            return TECH.PROCESS.MSN

        def _default_exclusion_purpose(self):
            return TECH.PURPOSE.DRAWING

        def _default_cladding_process(self):
            return TECH.PROCESS.SHALLOW

        def _default_cladding_purpose(self):
            return TECH.PURPOSE.DRAWING

        def _default_core_width(self):
            return TECH.RIBWG.RIB_WIDTH

        def _default_cladding_width(self):
            return TECH.WIREWG.CLADDING_WIDTH


@lock_properties()
class RWG450(MSNRibWaveguideTemplate):

    def _default_name(self):
        return "RWG450"

    class Layout(MSNRibWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.45

        def _default_exclusion_width(self):
            return 10

@lock_properties()
class RWG850(MSNRibWaveguideTemplate):

    def _default_name(self):
        return "RWG850"

    class Layout(MSNRibWaveguideTemplate.Layout):

        def _default_core_width(self):
            return 0.85

        def _default_exclusion_width(self):
            return 10

class RoundedRibWaveguide(RoundedWaveguide):

    def _default_trace_template(self):
        return RWG850()

    class Layout(RoundedWaveguide.Layout):

        def _default_bend_radius(self):
            return TECH.RIBWG.BEND_RADIUS


