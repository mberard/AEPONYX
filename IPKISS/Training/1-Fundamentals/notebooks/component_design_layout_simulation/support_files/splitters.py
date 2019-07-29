from ipkiss3 import all as i3
from splined_shape import SplinedShape
from picazzo3.traces.wire_wg import WireWaveguideTemplate
from luceda_cst.model import CapheModelCST
from picazzo3.logical.coupler import Coupler1x2
import numpy as np
import os


class _Splitter(Coupler1x2):

    """
    Abstract splitter class just places 3 waveguides. 
    """
    wg_template = i3.WaveguideTemplateProperty(doc="Waveguide template used.")
    wgs_straights = i3.ChildCellListProperty(doc="Childcells used for the waveguides.")

    def _default_wg_template(self):
        return WireWaveguideTemplate()

    def _default_wgs_straights(self):
        wgs = []
        for cnt in xrange(0, 3):
            wg = i3.RoundedWaveguide(trace_template=self.wg_template)
            wgs.append(wg)
        return wgs

    class Layout(i3.LayoutView):

        length_splitting_shape = i3.PositiveNumberProperty(default=10.0, doc="Length of the splitting section.")
        length_straights = i3.PositiveNumberProperty(default=0.2, doc="Length of the straights at the input and output.")
        spacing = i3.PositiveNumberProperty(default=2.0, doc="Spacing between the two output waveguides.")

        def _default_wg_template(self):
            wgt = self.cell.wg_template.get_default_view(i3.LayoutView)
            return wgt

        def _default_wgs_straights(self):
            wgs_cells = self.cell.wgs_straights
            wgs_layout = []

            for wgc in wgs_cells:
                wg_layout = wgc.get_default_view(i3.LayoutView)
                wg_layout.set(shape=i3.Shape(points=[(0, 0), (self.length_straights, 0)]))
                wgs_layout.append(wg_layout)

            return wgs_layout

        def _generate_instances(self, insts):

            insts += i3.SRef(reference=self.wgs_straights[0], name="in")
            insts += i3.SRef(reference=self.wgs_straights[1], name="out1", position=(self.length_straights
                                                                                     + self.length_splitting_shape, -self.spacing / 2))
            insts += i3.SRef(reference=self.wgs_straights[2], name="out2", position=(self.length_straights +
                                                                                     self.length_splitting_shape, +self.spacing / 2))

            return insts

        def _get_cladding_elem(self):

            shape_cladding = i3.Shape(points=[(self.length_straights, self.wg_template.width / 2),
                                              (self.length_straights + self.length_splitting_shape, self.spacing / 2 + self.wg_template.width / 2)])

            shape_cladding_m = i3.Shape(points=[(self.length_straights, -self.wg_template.width / 2),
                                                (self.length_straights + self.length_splitting_shape, -self.spacing / 2 - self.wg_template.width / 2)])

            shape_elem = shape_cladding + shape_cladding_m.reversed()

            elem = i3.Boundary(layer=self.wg_template.windows[1].layer, shape=shape_elem)
            return elem

        def _generate_ports(self, ports):
            ports += self.instances["in"].ports["in"]
            ports += self.instances["out1"].ports["out"].modified_copy(name="out1")
            ports += self.instances["out2"].ports["out"].modified_copy(name="out2")
            return ports

        def _get_splitting_elem(self):
            raise NotImplementedError("This is an abstract class in which _get_splitting_elem was not implemented.")

        def _generate_elements(self, elems):
            elems += self._get_splitting_elem()
            elems += self._get_cladding_elem()
            return elems

    class CapheModelCST(CapheModelCST):

        """
        CapheModel based on a CST simulation
        """

        def _default_additional_commands_path(self):
            return r"{}\simulation_files\additional_commands.mod".format(os.path.dirname(__file__))

    class CapheModel(Coupler1x2.CapheModel):

        """
        Simple Analytic CapheModel
        """
        pass


class SplitterWaypoint(_Splitter):

    """
    Splitter based on a waypoint. 
    """

    class Layout(_Splitter.Layout):

        waypoint = i3.Coord2Property(doc="Waypoint of the outside limit of the splitting shape.")
        split_base_width = i3.PositiveNumberProperty(default=0.4, doc="Width of the base of the parabole inside the splitting shape.")
        split_base_pos = i3.PositiveNumberProperty(doc="Position of the base of the parabole inside the splitting shape.")
        small_straights = i3.PositiveNumberProperty(default=0.1, doc="Length of the small straight added at each input waveguide in the direction of the waveguide.")

        def _default_split_base_pos(self):
            return self.length_splitting_shape / 2 + self.instances["in"].ports["out"].position.x

        def _default_waypoint(self):
            return i3.Coord2(self.length_straights + self.length_splitting_shape / 2, self.spacing / 2)

        def _get_splitting_elem(self):

            pos_in = self.instances["in"].ports["out"].position
            pos_out_1 = self.instances["out1"].ports["in"].position
            pos_out_2 = self.instances["out2"].ports["in"].position

            wg_width = self.wg_template.core_width

            waypoint_shape_top = i3.Shape(points=[pos_in + i3.Coord2(0, wg_width / 2),
                                                  pos_in + i3.Coord2(self.small_straights, wg_width / 2),
                                                  self.waypoint,
                                                  pos_out_2 + i3.Coord2(-self.small_straights, +wg_width / 2),
                                                  pos_out_2 + i3.Coord2(0, +wg_width / 2)])

            shape_top = SplinedShape(waypoint_shape=waypoint_shape_top, resolution=0.05)
            shape_top_m = shape_top.v_mirror_copy()

            start_parabole = i3.Coord2(self.split_base_pos, self.split_base_width / 2)
            end_parabole = pos_out_2 - i3.Coord2(self.small_straights, wg_width / 2)

            x_parabole = np.linspace(start_parabole[0], end_parabole[0], 50)
            y_parabole = (end_parabole[1] - start_parabole[1]) * 1 / (end_parabole[0] - x_parabole[0])**0.5 * (x_parabole - x_parabole[0])**0.5 + start_parabole[1]

            points_parabole = [i3.Coord2(x, y) for x, y in zip(x_parabole, y_parabole)]
            points_parabole = [i3.Coord2(start_parabole[0], 0)] + points_parabole + [pos_out_2 - i3.Coord2(0, wg_width / 2)]
            shape_parabole = i3.Shape(points=points_parabole)
            shape_parabole_m = shape_parabole.v_mirror_copy()

            shape_parabole_final = shape_parabole.reversed() + shape_parabole_m
            shape_parabole_final.remove_identicals()
            shape_bot = SplinedShape(waypoint_shape=i3.Shape(points=shape_parabole_final), resolution=0.05)

            shape_elem = shape_top + shape_bot + shape_top_m.reversed()
            shape_elem.remove_identicals()
            elem = i3.Boundary(layer=self.wg_template.core_layer, shape=shape_elem)

            return elem


class SplitterWayPoint1500(SplitterWaypoint):

    """
    SplitterWaypoint with somewhat optimized values at 1.5
    """

    class Layout(SplitterWaypoint.Layout):

        def _default_waypoint(self):
            return i3.Coord2(3.25522953, 1.10901057)

        def _default_split_base_pos(self):
            return 12.05368973

        def _default_split_base_width(self):
            return 0.35418294

        def _default_length_splitting_shape(self):
            return 16.49474912


# Set the simple model as default view.
SplitterWayPoint1500.set_default_view(SplitterWayPoint1500.CapheModel)
