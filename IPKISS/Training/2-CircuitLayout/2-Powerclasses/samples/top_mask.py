from aeponyx import technology
from ipkiss3 import all as i3
from routing.apac import APAC
from routing.routing_functions import wide_manhattan
from aeponyx.all import FC_TE_1550, TW_MZM


class Mask(APAC):

    grating_coupler = i3.ChildCellProperty(doc="Grating couplers used in order")
    modulators = i3.ChildCellListProperty(doc="Modulators used")
    m_spacing = i3.ListProperty(default=[3.5, 4.0, 4.5, 5.0], doc="Metal spacings")

    def _default_grating_coupler(self):
        return FC_TE_1550()

    def _default_modulators(self):
        mods = []
        for w in self.m_spacing:
            cell = TW_MZM(name=self.name + "Modulator_{}".format(w))
            cell.Layout(modulator_length=1500,
                        metal_spacing=w)
            mods.append(cell)
        return mods

    def _default_child_cells(self):
        childs = dict()
        for cnt, mod in enumerate(self.modulators):  # Iterates over self.m_spacing and uses cnt as a counter. This is a python construct.

            childs["grating_in_{}".format(cnt)] = self.grating_coupler
            childs["mod_{}".format(cnt)] = mod
            childs["grating_out_{}".format(cnt)] = self.grating_coupler

        return childs

    def _default_connectors(self):

        conn = []
        for cnt, mod in enumerate(self.modulators):
            in_port = "grating_in_{}:out".format(cnt)
            out_port = "mod_{}:in".format(cnt)
            conn.append((in_port, out_port, wide_manhattan))

            in_port = "grating_out_{}:out".format(cnt)
            out_port = "mod_{}:out".format(cnt)
            conn.append((in_port, out_port, wide_manhattan))

        return conn

    def _default_electrical_links(self):
        el_links = []
        #shorting the ground
        for cnt, mod in enumerate(self.modulators):
            for arm_name in ["1", "5"]:
                p1 = "mod_{}:elec_right_{}".format(cnt, arm_name)
                p2 = "mod_{}:elec_right_{}".format(cnt, "3")
                el_links.append((p1, p2))
                p1 = "mod_{}:elec_left_{}".format(cnt, arm_name)
                p2 = "mod_{}:elec_left_{}".format(cnt, "3")
                el_links.append((p1, p2))


        return el_links

    def _default_external_port_names(self):
        epn = dict()

        for cnt, mod in enumerate(self.modulators):
            epn["grating_in_{}:vertical_in".format(cnt)] = "in{}".format(cnt)
            epn["grating_out_{}:vertical_in".format(cnt)] = "out{}".format(cnt)

        return epn


    def _default_propagated_electrical_ports(self):
        pp = []
        for cnt, mod in enumerate(self.modulators):
            for arm_name in [2,3,4]:
                pp.append("mod_{}_elec_right_{}".format(cnt, arm_name))
                pp.append("mod_{}_elec_left_{}".format(cnt, arm_name))

        return pp


    class Layout(APAC.Layout):

        grating_coupler_spacing = i3.PositiveNumberProperty(default=800, doc="Spacing between the grating couplers")
        center_grating_coupler_y = i3.PositiveNumberProperty(doc="Start position of the first grating coupler")

        def _default_metal1_width(self):
            return 20.0

        def _default_center_bp(self):
            return 2500 / 2.0

        def _default_center_grating_coupler_y(self):
            return 5150.0 / 2.0

        def _default_child_transformations(self):
            trans = dict()
            nw = len(self.m_spacing)
            for cnt, mod in enumerate(self.modulators):
                t = (100.0, self.center_grating_coupler_y + (-nw / 2.0 + 0.5 + cnt) * self.grating_coupler_spacing)
                trans["grating_in_{}".format(cnt)] = i3.Translation(translation=t)
                t = (2500/2.0 , self.center_grating_coupler_y + (-nw / 2.0 + 0.5 + cnt) * self.grating_coupler_spacing)
                trans["mod_{}".format(cnt)] = i3.Translation(translation=t)
                trans["grating_out_{}".format(cnt)] = i3.Rotation(rotation=180.0) + i3.Translation(translation=(2500,
                                                                                                               self.center_grating_coupler_y + (-nw / 2.0 + 0.5 + cnt) * self.grating_coupler_spacing))

            return trans

        @i3.cache()
        def _default_electrical_routes(self):
            routes = []

            for c in self.electrical_links:
                inst1, port1 = self._resolve_inst_port(c[0])
                inst2, port2 = self._resolve_inst_port(c[1])

                if "left" in port1.name:
                    r = i3.RouteManhattan(input_port=port1,
                                          output_port=port2,
                                          min_straight=100.0,
                                          angle_out=0.0,
                                          angle_in=180.0,
                                          rounding_algorithm=None)

                else:
                    r = i3.RouteManhattan(input_port=port1,
                                          output_port=port2,
                                          min_straight=100.0,
                                          angle_out=180.0,
                                          angle_in=0.0,
                                          rounding_algorithm=None)

                routes.append(r)


            return routes


cell = Mask(name="TopMask")
lv = cell.Layout()
lv.visualize(annotate=True)
nl = cell.Netlist()
lv.write_gdsii("top_mask.gds")
print "done"
