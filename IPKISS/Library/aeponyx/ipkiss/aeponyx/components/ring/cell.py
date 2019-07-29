from ipkiss3 import all as i3
from picazzo3.routing.place_route import PlaceAndAutoRoute
from aeponyx.components.directional_coupler.cell import DirectionalCoupler
from aeponyx.components.waveguides.wire import SiWireWaveguideTemplate

class DropRing(PlaceAndAutoRoute):
    """
    Drop ring with the total ring length as a property. 
    """

    directional_coupler = i3.ChildCellProperty(doc="Directional couplers used for the ring resonator")
    total_ring_length = i3.PositiveNumberProperty(doc="Total ring length")

    def _default_trace_template(self):
        return SiWireWaveguideTemplate(name=self.name + "tt")

    def _default_total_ring_length(self):
        return 200.0

    def _default_directional_coupler(self):
        dc = DirectionalCoupler(name=self.name + 'dc')
        return dc

    def _default_child_cells(self):
        return {'dc1': self.directional_coupler,
                'dc2': self.directional_coupler
                }

    def _default_links(self):
        return [('dc1:in2', 'dc2:in2'),
                ('dc1:out2', 'dc2:out2')]

    def _default_external_port_names(self):
        return {"dc1:in1": "in1",
                "dc1:out1": "out1",
                "dc2:out1": "in2",
                "dc2:in1": "out2",
                }

    class Layout(PlaceAndAutoRoute.Layout):

        def _get_distance_between_dc(self):

            # Get length of the curved section of the directional coupler.
            dc = self.directional_coupler
            waveguide_length = (self.total_ring_length - 2 * dc.get_length_ring_section()) / 2.0
            height_out_port = dc.ports["out2"].y

            return 2 * height_out_port + waveguide_length

        def _default_child_transformations(self):
            return {"dc1": (0, 0),
                    "dc2": i3.VMirror(0.0) + i3.Translation((0, self._get_distance_between_dc()))}



