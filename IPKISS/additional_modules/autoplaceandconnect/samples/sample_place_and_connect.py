from technologies import silicon_photonics
from ipkiss3 import all as i3
from picazzo3.wg.bend import WgBend90
from picazzo3.wg.splitters import WgY90Splitter
from picazzo3.wg.spirals import FixedLengthSpiralRounded
from picazzo3.fibcoup.curved import FiberCouplerCurvedGrating
from picazzo3.wg.connector import RoundedWaveguideConnector

from auto_place_and_connect import AutoPlaceAndConnect
import numpy as np
import pylab as plt

wg50 = i3.Waveguide(name="waveguide50")
wg50.Layout(shape=[(0.0, 0.0), (50.0, 0.0)])
wg100 = i3.Waveguide(name="waveguide100")
wg100.Layout(shape=[(0.0, 0.0), (100.0, 0.0)])

gc = FiberCouplerCurvedGrating(name="gc")

spiral = FixedLengthSpiralRounded(name="spiral1",
                                  n_o_loops=2)
spiral.Layout(total_length=800.0)

bend1 = WgBend90(name="bend20")
bend1.Layout(bend_radius=20.0)

bend2 = WgBend90(name="bend10")
bend2.Layout(bend_radius=10.0)

splitter = WgY90Splitter(name="split")

circuit = AutoPlaceAndConnect(name="circuit",
                              child_cells={"gc1": gc,
                                           "gc2": gc,
                                           "gc3": gc,
                                           "gc4": gc,
                                           "wg1": wg100,
                                           "wg2": wg100,
                                           "wg3": wg50,
                                           "spl1": splitter,
                                           "bend1a": bend2,
                                           "bend1b": bend2,
                                           "delay": spiral,
                                           "bend2a": bend1,
                                           "bend2b": bend1,
                                           "spl2": splitter,
                                           "straight": wg50
                                           },
                              links=[("gc1:out", "wg1:in"),
                                     ("wg1:out", "spl1:center"),
                                     ("spl1:arm1", "bend1a:in"),
                                     ("spl1:arm2", "bend2a:in"),
                                     ("bend1a:out", "delay:in"),
                                     ("bend2a:out", "straight:in"),
                                     ("delay:out", "bend1b:in"),
                                     ("straight:out", "bend2b:in"),
                                     ("bend1b:out", "spl2:arm1"),
                                     ("bend2b:out", "spl2:arm2"),
                                     ("spl2:center", "wg2:in"),
                                     ("wg2:out", "gc2:out"),
                                     # 2nd independent circuit
                                     ("gc3:out", "wg3:in"),
                                     ("wg3:out", "gc4:out"),
                                     ],
                              external_port_names={"gc1:vertical_in": "in",
                                                   "gc2:vertical_in": "out",
                                                   "gc3:vertical_in": "ref_in",
                                                   "gc4:vertical_in": "ref_out",
                                                   })
lo = circuit.Layout(mirror_children={"spl2": True,
                                     "bend2a": True,
                                     "bend2b": True,
                                     "delay": True
                                     },
                    child_transformations={"gc1": (0.0, 100.0),
                                           }
                    )

lo.visualize()

i3.Waveguide.set_default_view("CapheModelFromLayout")
i3.RoundedWaveguide.set_default_view("CapheModelFromLayout")
cm = circuit.CapheModel()
wavelengths = np.linspace(1.5, 1.6, 1001)

smatrix = cm.get_smatrix(wavelengths=wavelengths)

plt.plot(wavelengths, np.abs(smatrix["in", "out"])**2)
plt.show()

print "Done"
