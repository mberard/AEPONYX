from aeponyx import technology
from aeponyx.components.mmi import MMI_12
from ipkiss3 import all as i3
from routing.apac import APAC
from routing.routing_functions import sbend, wide_manhattan


class Splitter3(APAC):
    splitter = i3.ChildCellProperty(doc="splitter used")
    spacing_x = i3.PositiveNumberProperty(default=100.0, doc="spacing port to port")
    spacing_y = i3.PositiveNumberProperty(default=200.0, doc="spacing port to port")

    def _default_splitter(self):
        return MMI_12(name=self.name+"_MMI")

    def _default_child_cells(self):

        childs = dict()
        childs["sp_0_0"] = self.splitter
        childs["sp_1_0"] = self.splitter
        childs["sp_1_1"] = self.splitter

        return childs

    def _default_connectors(self):
        conn = []
        conn.append(("sp_0_0:out1", "sp_1_0:in1",sbend))
        conn.append(("sp_0_0:out2", "sp_1_1:in1", sbend))

        return conn

    class Layout(APAC.Layout):

        def _default_child_transformations(self):

            trans = dict()

            trans["sp_0_0"] = (0, 0)
            trans["sp_1_0"] = (self.spacing_x, -self.spacing_y / 2.0)
            trans["sp_1_1"] = (self.spacing_x, +self.spacing_y / 2.0)

            return trans


if __name__ == "__main__":
    cell = Splitter3(name="TopCell")
    cell_lv = cell.Layout()
    cell_lv.visualize(annotate=True)
    cell_lv.write_gdsii("splitter_3.gds")
    print "done"
