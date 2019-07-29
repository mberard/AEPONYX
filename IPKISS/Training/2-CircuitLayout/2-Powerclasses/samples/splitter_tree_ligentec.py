from ligentec import technology
technology.select_technology_variant(technology.TECH.VARIANTS.AN150)
from ligentec.all import MMI1x2BB
from routing.routing_functions import sbend
from splitter_tree import SplitterTree

class SplitterTree(SplitterTree):


    def _default_bend_radius(self):
        return 50.0

    def _default_spacing_x(self):
        return 350.0

    def _default_spacing_y(self):
        return 50.0

    def _default_splitter(self):
        return MMI1x2BB()


    def _default_connectors(self):
        conn = []
        for l in range(1, self.levels):
            for sp in range(int(2 ** l)):
                if sp % 2 == 0:
                    in_port = "sp_{}_{}:out0".format(l - 1, int(sp / 2.0))
                else:
                    in_port = "sp_{}_{}:out1".format(l - 1, int(sp / 2.0))

                out_port = "sp_{}_{}:in0".format(l, sp)

                conn.append((in_port, out_port, sbend, {"bend_radius": self.bend_radius}))

        return conn




if __name__ == "__main__":
    cell = SplitterTree()
    cell_lv = cell.Layout()
    cell_lv.visualize()
    cell_lv.write_gdsii("splitter_tree_ligentec.gds")
    print "done"
