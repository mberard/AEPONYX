if __name__== "__main__":
    from aeponyx import technology
from aeponyx.components.mmi import MMI_12
from aeponyx.components.grating_couplers.cell import FC_TE_1550
from ipkiss3 import all as i3
from routing.apac import APAC
from splitter_tree import SplitterTree
from routing.routing_functions import sbend, wide_manhattan, manhattan
from routing.combine_connectors import combine_connectors


class RouteSplitterTree(APAC):
    dut = i3.ChildCellProperty(doc="splitter used")
    grating = i3.ChildCellProperty(doc="Splitter")

    def _default_grating(self):
        return FC_TE_1550()
    def _default_bend_radius(self):
        return 20.0
    def _default_dut(self):
        return SplitterTree(name = self.name + "_DUT", levels=4)

    def _default_child_cells(self):
        childs = dict()
        childs["DUT"] = self.dut
        for cnt in range(2**self.dut.levels):
            childs["gr_out_{}".format(cnt)] = self.grating
        childs["gr_in"] = self.grating
        return childs


    def _default_connectors(self):
        conn = []
        conn.append(("gr_in:out", "DUT:in", manhattan))
        insts = self.get_child_instances()
        n_gratings = 2**self.dut.levels
        for cnt in range(n_gratings):
            cnt2 = 2**self.dut.levels - cnt
            grating_name = "gr_out_{}".format(cnt)
            dut_port_name = "out{}".format(cnt2)
            grating_port = insts[grating_name].ports["out"]
            dut_port = insts["DUT"].ports[dut_port_name]
            # Case 1 the grating coupler is on the right of the outputs
            if grating_port.position.x > dut_port.position.x + 70.0:
                conn.append(("gr_out_{}:out".format(cnt), "DUT:out{}".format(cnt2), manhattan))
            # Case 2  the grating coupler is on the left - some bending is required.
            elif grating_port.position.x < dut_port.position.x :
                p1 = (dut_port.x + 30+cnt*5, 220+cnt*5)
                from functools import partial
                cr = partial(manhattan, control_points=[p1])
                conn.append(("gr_out_{}:out".format(cnt), "DUT:out{}".format(cnt2), cr))
            # Case 3  the grating coupler is more or less in the middle, use sbends
            else:
                p1 = (dut_port.x + 30+cnt*5, 220+cnt*5)
                T = i3.Rotation(rotation=-90.0) + i3.Translation(translation=p1)
                from functools import partial
                cr = combine_connectors([sbend, manhattan], [T])
                conn.append(("gr_out_{}:out".format(cnt), "DUT:out{}".format(cnt2), cr))

        return conn

    class Layout(APAC.Layout):
        sep = i3.PositiveNumberProperty(default=50.0, doc="Separation between the gratings")
        def _default_child_transformations(self):

            trans = dict()
            trans["DUT"] = i3.IdentityTransform()
            nw = self.dut.size_info().north_west
            for cnt in range(2 ** self.dut.levels):
                trans["gr_out_{}".format(cnt)] = i3.Rotation(rotation=-90.0) + i3.Translation(translation=(nw[0]+cnt*self.sep, nw[1]+180))
            trans["gr_in"]  = i3.Rotation(rotation=-90.0) + i3.Translation(translation=(nw[0] -1*self.sep, nw[1]+180))

            return trans


if __name__ == "__main__":
    cell = RouteSplitterTree(name="Routed_tree")
    cell_lv = cell.Layout()
    cell_lv.visualize(annotate=True)
    cell_lv.write_gdsii("splitter_tree_routed.gds")
    print "done"
