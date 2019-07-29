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
                                  n_o_loops=2)

spiral_lv = spiral.Layout(total_length=800.0)
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
               links=[("spl1:out2", "delay:in"),
                      ("delay:out", "spl2:out2"),
                      ("spl1:out1", "bend1a:in"),
                      ("spl2:out1", "bend1b:in")
                      ],

               connectors=[("bend1a:out", "bend1b:out"),
                           ("grin:out", "spl1:in1", wide_manhattan),
                           ("grout:out", "spl2:in1", wide_manhattan)],


               )
lo = circuit.Layout(mirror_children={"delay": False,
                                     "spl2": True,
                                     "bend1a": True,
                                     "bend1b": False},

                    child_transformations={"spl1": (0.0, 0.0),
                                           "grin": i3.Rotation(rotation=0) + i3.Translation((-100, 0)),
                                           "grout": i3.Rotation(rotation=180) + i3.Translation((+300, 0))},
                    )

lo.visualize(annotate=True)
