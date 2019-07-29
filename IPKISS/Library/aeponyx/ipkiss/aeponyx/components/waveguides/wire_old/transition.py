"""Example transition in aeponyx.

In this file we define default transitions (tapers) for aeponyx. We don't override a lot of parameters here.

You can specify the default length in LinearWireTransitionFromPort. In this example, we use a simple formula to calculate
the transition length based on the core widths of the connecting trace templates.

"""
__all__ = ["LinearWireTransition", "LinearWireTransitionFromPort"]

from ipkiss.technology import get_technology
from ipkiss3.pcell.layout.view import LayoutView
from ipkiss3.pcell.trace.transition import RestrictedTraceTemplateProperty
from picazzo3.traces.wire_wg.transitions import LinearWindowWaveguideTransition, LinearWindowWaveguideTransitionFromPort
from aeponyx.components.waveguides.wire import MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000

TECH = get_technology()


class LinearWireTransition(LinearWindowWaveguideTransition):

    """Simple transition based on a LinearWindowTraceTransition.

    This class can be adjusted to the user's needs (currently, it contains only some boilerplate)
    """
    _name_prefix = "T_WIRE_WIRE_LIN"

    start_trace_template = RestrictedTraceTemplateProperty((MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000))
    end_trace_template = RestrictedTraceTemplateProperty((MSNWireWaveguideTemplate, SWG450, SWG550, SWG1000))


class LinearWireTransitionFromPort(LinearWireTransition, LinearWindowWaveguideTransitionFromPort):

    class Layout(LinearWireTransition.Layout, LinearWindowWaveguideTransitionFromPort.Layout):

        def _default_length(self):
            # calculate taper length dynamically based on difference in core width
            w1 = self.start_trace_template.core_width
            w2 = self.end_trace_template.core_width
            a = 10.0
            return max(1.0, abs(a * (w2**2 - w1**2)))

        """Make sure we call the correct _generate_ports. The LinearWindowWaveguideTransitionFromPort
        knows how to position the end port based on the length of the transition."""
        _generate_ports = LinearWindowWaveguideTransitionFromPort.Layout._generate_ports


from picazzo3.traces.wire_wg import WireWaveguideTemplate
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.add(MSNWireWaveguideTemplate, MSNWireWaveguideTemplate, LinearWireTransitionFromPort)
# These two trace templates are identical if you look at the windows, so we can treat them as equal for transitioning purposes
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(WireWaveguideTemplate, MSNWireWaveguideTemplate)
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(SWG450, MSNWireWaveguideTemplate)
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(SWG550, MSNWireWaveguideTemplate)
TECH.PCELLS.TRANSITION.AUTO_TRANSITION_DATABASE.treat_trace_template_as(SWG1000, MSNWireWaveguideTemplate)
