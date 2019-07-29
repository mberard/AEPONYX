from auto_place_and_connect.auto_place_and_connect import AutoPlaceAndConnect, PortInterfaceFromInstance
import collections
from picazzo3.wg.connector import WaveguideConnector
from .routing_functions import sbend, manhattan
from ipkiss3 import all as i3


def get_port_from_interface(port_id, lv):

    instance_name = port_id.split(":")[0]
    port_name = port_id.split(":")[1]
    if not instance_name in lv.instances:
        raise Exception("Instance {} does not exist - please check your childcells".format(instance_name))
    pnames = [p.name for p in lv.instances[instance_name].ports]
    if port_name not in pnames:
        raise Exception("Port {} does not exist on instance {} - the availble ports are {}".format(port_name, instance_name,pnames))


    return lv.instances[instance_name].ports[port_name]


class APAC(AutoPlaceAndConnect):

    connectors = i3.ListProperty(doc="list of tuples that need to be replaced by connectors. Format is [('inst1:term1','inst2:term2', connector_func=None, connector_kwargs, ...]",
                                 restriction=i3.RestrictTypeList(allowed_types=[collections.Sequence]))
    connector_function = i3.CallableProperty(doc="Connector function used by default if nothing is specified")
    connector_kwargs = i3.DictProperty(doc="Routing kwargs passed if nothing is specified")
    electrical_links = i3.ListProperty(default=[], doc="List of electrical links")
    propagated_electrical_ports = i3.ListProperty(default=[], doc="List of electrical ports that are propagated to the next level. By default None are propagated")

    def connector_less_apac(self):
        """
        generete a connectorless apac to get the location of all the instances.
        """
        lv = self.get_default_view(i3.LayoutView)
        temp_apac = APAC(name="{}_connectorless".format(self.name),
                         child_cells=self.child_cells,
                         links=self.links)
        temp_apac_lv = temp_apac.Layout(child_transformations=lv.child_transformations,
                                        mirror_children=lv.mirror_children)

        return temp_apac_lv

    @i3.cache()
    def get_child_instances(self):
        return self.connector_less_apac().instances

    def _default_connector_function(self):
        return manhattan

    def _default_connector_kwargs(self):
        return {}

    def _default__connector_cells(self):

        connector_cells = []
        if len(self.connectors) > 0:
            clv = self.connector_less_apac()

        for cnt, c in enumerate(self.connectors):
            connector_function = self.connector_function
            connector_kwargs = self.connector_kwargs

            start_port = get_port_from_interface(port_id=c[0], lv=clv)
            end_port = get_port_from_interface(port_id=c[1], lv=clv)

            if len(c) > 2:
                if c[2] is not None:
                    connector_function = c[2]

            if len(c) == 4:
                if c[3] is not None:
                    connector_kwargs = c[3]

            name = self.name + "_connector_{}".format(cnt)
            cell = connector_function(start_port=start_port, end_port=end_port,name=name, connector_kwargs=connector_kwargs)
            connector_cells.append(cell)

        return connector_cells

    class Layout(AutoPlaceAndConnect.Layout):
        metal1_layer = i3.LayerProperty( doc="layer used for the electrical links")
        metal1_width = i3.PositiveNumberProperty(default=5, doc="Width of the metal1 paths")
        electrical_routes = i3.ListProperty(doc="routes used for the electical connections. Is has the same length as electical links. None will use Routemanhatan")

        def _default_metal1_layer(self):
            return i3.TECH.PPLAYER.M1.LINE

        def _default_electrical_routes(self):
            return [None] * len(self.electrical_links)

        @i3.cache()
        def _get_final_electrical_routes(self):
            final_routes = []
            for c, r in zip(self.electrical_links, self.electrical_routes):
                if r is None:
                    inst1, port1 = self._resolve_inst_port(c[0])
                    inst2, port2 = self._resolve_inst_port(c[1])
                    route = i3.RouteManhattan(input_port=port1,
                                              output_port=port2,
                                              angle_out=0.0,
                                              angle_in=0.0,
                                              rounding_algorithm=None)
                    final_routes.append(route)
                else:
                    final_routes.append(r)

            return final_routes

        @i3.cache()
        def _get_electrical_route_elements(self):
            els = []
            for r in self._get_final_electrical_routes():
                els += i3.Path(layer=self.metal1_layer, shape=r, line_width=self.metal1_width)

            return els

        def _generate_elements(self, elems):

            elems = super(APAC.Layout, self)._generate_elements(elems)
            elems.extend(self._get_electrical_route_elements())

            return elems

        def _generate_ports(self, ports):
            connected_links = [p[0].replace(':', '_') for p in self.links] + [p[1].replace(':', '_') for p in self.links]
            connected_links.extend([p[0].replace(':', '_') for p in self.connectors] + [p[1].replace(':', '_') for p in self.connectors])
            for key, val in self._get_child_cell_instances().iteritems():
                if key in self.child_cells.keys():
                    for p in val.ports:
                        name = "{}_{}".format(key, p.name)
                        if name not in connected_links:
                            ext_key = '{}:{}'.format(key, p.name)
                            if ext_key in self.cell.external_port_names:
                                name = self.cell.external_port_names[ext_key]
                            if p.domain is i3.ElectricalDomain and name not in self.propagated_electrical_ports:
                                pass
                            else:
                                ports.append(p.modified_copy(name=name))

            return ports

    class Netlist(AutoPlaceAndConnect.Netlist):

        def _generate_terms(self, terms):

            from ipkiss3.pcell.netlist.term import TermDict
            terms = super(AutoPlaceAndConnect.Netlist, self)._generate_terms(terms)
            new_terms = TermDict()

            for key, val in terms.iteritems():
                if val.domain is i3.ElectricalDomain and val.name not in self.propagated_electrical_ports:
                    pass
                else:
                    new_terms[key] = val

            return new_terms


