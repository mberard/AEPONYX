# Collections of default PCells
#
# The technology keys below are required keys for Ipkiss
# You can change the values or refer them to your own technology keys, but their name should not be changed

from ipkiss.technology import get_technology
from ipkiss.technology.technology import DelayedInitTechnologyTree, TechnologyTree
from ipkiss.process.layer import PPLayer
TECH = get_technology()

# Default waveguide template used in routing


class WGTechTree(DelayedInitTechnologyTree):

    def initialize(self):
        from aeponyx.components.waveguides.wire import SWG450
        self.DEFAULT = SWG450()

TECH.PCELLS.WG = WGTechTree()

# Default metal wire used in metal routing


class MetalTree(DelayedInitTechnologyTree):

    def initialize(self):
        from aeponyx.components.metal.wire import M1WireTemplate
        self.DEFAULT = M1WireTemplate()


TECH.PCELLS.METAL = MetalTree()

# Auto-transition database


class TransitionTree(DelayedInitTechnologyTree):

    def initialize(self):

        # Note: we only create an empty transition database here. It will be filled elsewhere
        # (for example, please check aeponyx/components/waveguides/wire/transition.py)
        from ipkiss3.pcell.trace.transitions.auto_transition.auto_transition_db import AutoTransitionDatabase
        db = AutoTransitionDatabase()
        self.AUTO_TRANSITION_DATABASE = db


TECH.PCELLS.TRANSITION = TransitionTree()
