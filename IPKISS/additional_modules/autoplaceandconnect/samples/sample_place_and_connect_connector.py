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
                              child_cells={"spl1": splitter,
                                           "bend1a": bend2,
                                           "bend1b": bend1,
                                           "bend2a": bend2,
                                           "bend2b": bend1,
                                           "delay": spiral,
                                           "spl2": splitter,
                                           },
                              links=[("spl1:arm1", "bend1a:in"),
                                     ("bend1a:out", "delay:in"),
                                     ("delay:out", "bend2a:in"),
                                     ("bend2a:out", "spl2:arm1"),
                                     ("spl1:arm2", "bend1b:in"),
                                     ("bend2b:out", "spl2:arm2")
                                     ],
                              connectors=[("bend1b:out", "bend2b:in")],
                              external_port_names={"spl1:center": "in",
                                                   "spl2:center": "out"
                                                   }
                              )
lo = circuit.Layout(mirror_children={"delay": True,
                                     "spl2": True,
                                     "bend1b": True,
                                     "bend2b": True},
                    child_transformations={"spl1": (0.0, 0.0)
                                           },
                    )

lo.visualize()

# Fetch the lengths of the connectors, in the same order as they were specified in the "connectors" argument of the AutoPlaceAndConnect
# At the moment this uses a part which is not part of the API (_connector_cells) and subject to change in the future
connector_lengths = [c.trace.trace_length() for c in lo._connector_cells]
print("connector lengths: {}".format(connector_lengths))

# Run a simulation
i3.Waveguide.set_default_view("CapheModelFromLayout")
i3.RoundedWaveguide.set_default_view("CapheModelFromLayout")
cm = circuit.CapheModel()
wavelengths = np.linspace(1.54, 1.56, 1001)

smatrix = cm.get_smatrix(wavelengths=wavelengths)

plt.plot(wavelengths, np.abs(smatrix["in", "out"])**2)
plt.show()

print "Done"
