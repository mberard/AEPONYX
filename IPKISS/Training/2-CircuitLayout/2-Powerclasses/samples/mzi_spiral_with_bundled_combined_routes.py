from aeponyx import technology
from ipkiss3 import all as i3
from picazzo3.wg.bend import WgBend90
from aeponyx.all import MMI_12, FC_TE_1550, MMI_22
from picazzo3.wg.spirals import FixedLengthSpiralRounded
from routing.apac import APAC
from routing.routing_functions import sbend, manhattan
from routing.combine_connectors import combine_connectors
from aeponyx.components.metal.bondpad import BONDPAD_5050


class Circuit(APAC):
    splitter = i3.ChildCellProperty()
    combiner = i3.ChildCellProperty()
    bend = i3.ChildCellProperty()
    spiral = i3.ChildCellProperty()
    grating = i3.ChildCellProperty()
    bp = i3.ChildCellProperty()

    def _default_bp(self):
        return BONDPAD_5050()

    def _default_grating(self):
        gr = FC_TE_1550()
        return gr

    def _default_spiral(self):
        spiral = FixedLengthSpiralRounded(name=self.name + "spiral1",
                                          n_o_loops=2)
        spiral_lv = spiral.Layout(total_length=800.0)
        return spiral

    def _default_bend(self):
        bend = WgBend90(name=self.name + "bend20")
        bend_lv = bend.Layout(bend_radius=20.0)
        return bend

    def _default_splitter(self):
        splitter = MMI_12(name=self.name + "split")
        splitter_lv = splitter.Layout()
        return splitter

    def _default_combiner(self):
        combiner = MMI_22(name=self.name + "comb")
        return combiner

    def _default_child_cells(self):
        return {"spl1": self.splitter,
                "bend1a": self.bend,
                "bend1b": self.bend,
                "delay": self.spiral,
                "spl2": self.combiner,
                "grin": self.grating,
                "grout1": self.grating,
                "grout2": self.grating,
                "bp1": self.bp,
                "bp2": self.bp,
                "bp3": self.bp
                }

    def _default_links(self):
        links = [("spl1:out1", "delay:in"),
                 ("delay:out", "spl2:in2"),
                 ("spl1:out2", "bend1a:in"),
                 ("spl2:in1", "bend1b:in")
                 ]
        return links

    def get_cr1(self, cnt=0):
        d=5.0
        insts = self.get_child_instances()
        t1 = i3.Translation(insts["bp1"].size_info().north_west) + i3.Translation((+cnt*d, 40-cnt*d))
        t2 = i3.Translation(insts["bp2"].size_info().south_east) - i3.Translation((0, 20+cnt*d))
        cr = combine_connectors([manhattan, sbend, manhattan], [t1, t2])
        return cr

    def get_cr2(self):
        insts = self.get_child_instances()
        t1 = i3.Translation((insts["bp3"].size_info().south_west)) - i3.Translation((0, 20))
        cr = combine_connectors([sbend, manhattan], [t1])
        return cr

    def _default_connectors(self):
        connectors = [("bend1a:out", "bend1b:out"),
                      ("spl2:out2", "grout2:out", self.get_cr1(cnt=1)),
                      ("spl2:out1", "grout1:out", self.get_cr1(cnt=0)),
                      ("grin:out", "spl1:in1", self.get_cr2())]

        return connectors

    class Layout(APAC.Layout):

        def _default_child_transformations(self):
            return {"spl1": (0.0, 0.0),
                    "grin": (-200, -200),
                    "grout1": i3.HMirror() + i3.Translation(translation=(600, +150)),
                    "grout2": i3.HMirror() + i3.Translation(translation=(600, -100)),
                    "bp1": (300, 0),
                    "bp2": (500, 150),
                    "bp3": (-100, -100)}

        def _default_mirror_children(self):
            return {"delay": True,
                    "spl2": True,
                    "bend1a": False,
                    "bend1b": True}


circuit = Circuit()
lv = circuit.Layout()
lv.visualize(annotate=True)
lv.write_gdsii("combined_routes.gds")
