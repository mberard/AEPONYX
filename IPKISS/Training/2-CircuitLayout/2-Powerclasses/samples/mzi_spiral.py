from aeponyx import technology

from ipkiss3 import all as i3
from picazzo3.wg.bend import WgBend90
from aeponyx.all import MMI_12, FC_TE_1550
from picazzo3.wg.spirals import FixedLengthSpiralRounded
from routing.apac import APAC
from routing.routing_functions import sbend, manhattan, wide_manhattan


import numpy as np
import pylab as plt


spiral = FixedLengthSpiralRounded(name="spiral1",
                                  n_o_loops=5)

spiral_lv = spiral.Layout(total_length=3000.0)
# spiral_lv.visualize()

gr = FC_TE_1550()
gr_layout = gr.Layout()
# gr_layout.visualize()


bend1 = WgBend90(name="bend20")
bend1_lv = bend1.Layout(bend_radius=20.0)
# bend1_lv.visualize()


splitter = MMI_12(name="split")
splitter_lv = splitter.Layout()
# splitter_lv.visualize()


circuit = APAC(name="circuit",
               child_cells={"spl1": splitter,
                            "bend1a": bend1,
                            "bend1b": bend1,
                            "delay": spiral,
                            "spl2": splitter,
                            "grin": gr,
                            "grout": gr
                            },
               links=[("spl1:out1", "delay:in"),
                      ("delay:out", "spl2:out1"),
                      ("spl1:out2", "bend1a:in"),
                      ("spl2:out2", "bend1b:in")
                      ],

               connectors=[("bend1a:out", "bend1b:out"),
                           ("grin:out", "spl1:in1", wide_manhattan),
                           ("grout:out", "spl2:in1", wide_manhattan)],


               )
lo = circuit.Layout(mirror_children={"delay": True,
                                     "spl2": True,
                                     "bend1a": False,
                                     "bend1b": True},

                    child_transformations={"spl1": (0.0, 0.0),
                                           "grin": (-100, -100),
                                           "grout": i3.HMirror() + i3.Translation(translation=(300, +150))},
                    )

lo.visualize(annotate=True)
lo.write_gdsii("mzi_spiral_fixed.gds")
