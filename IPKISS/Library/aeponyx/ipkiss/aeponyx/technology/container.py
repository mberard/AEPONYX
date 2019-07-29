from ipkiss.technology import get_technology
TECH = get_technology()
from ipkiss.technology.technology import TechnologyTree, DelayedInitTechnologyTree

TECH.CONTAINER = TechnologyTree()


class TerminatePortsTechnologyTree(DelayedInitTechnologyTree):

    def initialize(self):
        self.CHILD_SUFFIX = "termination"
        self.TERMINATION_INSTANCE_PREFIX = "termination"

TECH.CONTAINER.TERMINATE_PORTS = TerminatePortsTechnologyTree()
