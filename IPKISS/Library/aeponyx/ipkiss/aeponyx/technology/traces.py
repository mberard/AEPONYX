
# Traces,  waveguides

from ipkiss.technology import get_technology
from ipkiss.technology.technology import DelayedInitTechnologyTree

TECH = get_technology()

# TRACES


class TechTraceTree(DelayedInitTechnologyTree):

    def initialize(self):
        from ipkiss.primitives.layer import Layer
        self.DEFAULT_LAYER = TECH.PPLAYER.MSN
        self.CONTROL_SHAPE_LAYER = TECH.PPLAYER.WIRE_TRACE
        self.BEND_RADIUS = 100.0
        self.DRAW_CONTROL_SHAPE = False

TECH.TRACE = TechTraceTree()

# FIXME - temporary default settings until waveguides have modes

TECH.DEFAULT_WAVELENGTH = 1.55


class TechWgDefaultsTree(DelayedInitTechnologyTree):

    def initialize(self):
        from ipkiss.primitives.layer import Layer
        self.N_EFF = 2.4
        self.N_GROUP = 4.4
        self.LOSS_DB_PERM = 0.0
        self.CORE_LAYER = Layer(0)


TECH.WG_DEFAULTS = TechWgDefaultsTree()
