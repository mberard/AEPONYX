from ipkiss3 import all as i3
from picazzo3.wg.chain import TraceChain
from picazzo3.routing.place_route import PlaceAndConnect



import numpy as np




class TraceChainWithCenterLine(TraceChain):
    """
    Class that only accepts traces that have a method for the center line.
    """
    class Layout(TraceChain.Layout):

        center_line_shape = i3.ListProperty(locked=True)

        def validate_properties(self):
            for cnt, t in enumerate(self.traces):
                if not hasattr(t, "center_line_shape"):
                    raise i3.PropertyValidationError(self, "Trace {} has not center_line_shape", {"trace": t,
                                                                                                  "type t": type(t),
                                                                                                  "Position in trace": cnt})
            return True

        def _default_center_line_shape(self):
            shape = []
            for t in self.traces:
                shape += t.center_line_shape

            return shape
