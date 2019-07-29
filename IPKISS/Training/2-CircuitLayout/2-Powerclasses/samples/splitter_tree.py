if __name__== "__main__":
    from aeponyx import technology
from aeponyx.components.mmi import MMI_12
from ipkiss3 import all as i3
from routing.apac import APAC
from routing.routing_functions import sbend


class SplitterTree(APAC):
    splitter = i3.ChildCellProperty(doc="splitter used")
    levels = i3.IntProperty(default=3, doc="Number of levels")
    spacing_x = i3.PositiveNumberProperty(default=100.0)
    spacing_y = i3.PositiveNumberProperty(default=50.0)
    bend_radius = i3.PositiveNumberProperty()

    def _default_bend_radius(self):
        return 20.0

    def _default_splitter(self):
        return MMI_12(name=self.name + "_MMI")

    def _default_child_cells(self):

        childs = dict()
        for l in range(self.levels):
            for sp in range(int(2 ** l)):
                childs["sp_{}_{}".format(l, sp)] = self.splitter

        return childs

    def _default_connectors(self):
        conn = []
        for l in range(1, self.levels):
            for sp in range(int(2 ** l)):
                if sp % 2 == 0:
                    in_port = "sp_{}_{}:out1".format(l - 1, int(sp / 2.0))
                else:
                    in_port = "sp_{}_{}:out2".format(l - 1, int(sp / 2.0))

                out_port = "sp_{}_{}:in1".format(l, sp)

                conn.append((in_port, out_port, sbend, {"bend_radius": self.bend_radius}))

        return conn


    def _default_external_port_names(self):
        epn = dict()
        cnt = 1
        l = self.levels-1
        for sp in range(int(2 ** l)):
            epn["sp_{}_{}:out1".format(l, sp)] = "out{}".format(cnt)
            cnt = cnt + 1
            epn["sp_{}_{}:out2".format(l, sp)] = "out{}".format(cnt)
            cnt = cnt + 1

        epn["sp_{}_{}:in1".format(0, 0)] = "in"
        return epn



    class Layout(APAC.Layout):

        def _default_child_transformations(self):

            trans = dict()
            for l in range(self.levels):
                for sp in range(int(2 ** l)):
                    sp_y = self.spacing_y * 2**(self.levels - l - 1)
                    trans["sp_{}_{}".format(l, sp)] = i3.Translation(
                        translation=(l * self.spacing_x, - 0.5 * (2 ** l - 1) * sp_y + sp * sp_y))

            return trans




if __name__ == "__main__":
    cell = SplitterTree(levels=3)
    cell_lv = cell.Layout()
    cell_lv.visualize(annotate=True)
    cell_lv.write_gdsii("splitter_tree.gds")
    print "done"
