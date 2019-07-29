"""Example transition in aeponyx between rib_old and wire waveguide.

You can specify the default length in LinearRibWireTransitionFromPort. In this example, we use a simple formula to calculate
the transition length based on the core widths of the connecting trace templates.

"""
__all__ = ["LinearWireRibTransition", "LinearWireRibTransitionFromPort"]

from ipkiss.technology import get_technology
from ipkiss3.pcell.photonics.transitions.linear import LinearWindowWaveguideTransition, LinearWindowWaveguideTransitionFromPort
from ipkiss3.pcell.layout.view import LayoutView
from ipkiss3.pcell.trace.transition import RestrictedTraceTemplateProperty
from picazzo3.traces.rib_wg.transitions import WireRibWaveguideTransitionLinear, WireRibWaveguideTransitionFromPortLinear
from ..wire.trace import MSNWireWaveguideTemplate
from aeponyx.components.waveguides.wire import MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000
from aeponyx.components.waveguides.rib import MSNRibWaveguideTemplate, RWG450, RWG850

TECH = get_technology()


class LinearWireRibTransition(WireRibWaveguideTransitionLinear):

    """Simple transition from wire to rib_old, based on a LinearWindowTraceTransition.

    This class can be adjusted to the user's needs (currently, it contains only some boilerplate)
    """
    _name_prefix = "T_WIRE_RIB_LIN"

    start_trace_template = RestrictedTraceTemplateProperty((MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000, MSNRibWaveguideTemplate, RWG450, RWG850))
    end_trace_template = RestrictedTraceTemplateProperty((MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000, MSNRibWaveguideTemplate, RWG450, RWG850))


class LinearWireRibTransitionFromPort(LinearWireRibTransition, WireRibWaveguideTransitionFromPortLinear):
    def validate_properties(self):
        return True

    def _default_wire_only_wg_template(self):
        if isinstance(self._start_trace_template, MSNWireWaveguideTemplate):
            return MSNWireWaveguideTemplate(name=self.name+"_wowgt")
        if isinstance(self._start_trace_template, MSNRibWaveguideTemplate):
            return MSNRibWaveguideTemplate(name= self.name + "_wowgt")

    def _default_modified_start_wg_template(self):
        if isinstance(self._end_trace_template, MSNWireWaveguideTemplate):
            return MSNWireWaveguideTemplate(name=self.name+"_mswgt")
        if isinstance(self._end_trace_template, MSNRibWaveguideTemplate):
            return MSNRibWaveguideTemplate(name= self.name + "_mswgt")

    class Layout(LinearWireRibTransition.Layout, WireRibWaveguideTransitionFromPortLinear.Layout):

        def _default_length(self):
            # calculate taper length dynamically based on difference in core width
            w1 = self.start_trace_template.core_width
            w2 = self.end_trace_template.core_width
            a = 20.0
            return max(2.0, abs(a * (w2**2 - w1**2)))

        """Make sure we call the correct _generate_ports. The LinearWindowWaveguideTransitionFromPort
        knows how to position the end port based on the length of the transition."""
        _generate_ports = WireRibWaveguideTransitionFromPortLinear.Layout._generate_ports


from picazzo3.traces.rib_wg import RibWaveguideTemplate
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.add(MSNWireWaveguideTemplate, MSNRibWaveguideTemplate, LinearWireRibTransitionFromPort)
# These two trace templates are identical if you look at the windows, so we can treat them as equal for transitioning purposes
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(RibWaveguideTemplate, MSNRibWaveguideTemplate)
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(RWG450, MSNRibWaveguideTemplate)
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(RWG850, MSNRibWaveguideTemplate)
