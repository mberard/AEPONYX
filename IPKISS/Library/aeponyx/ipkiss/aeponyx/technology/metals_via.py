# Settings for metallization
#
# The technology keys below are required keys for Ipkiss
# You can change the values or refer them to your own technology keys, but their name should not be changed


from ipkiss.technology.technology import ProcessTechnologyTree, TechnologyTree, DelayedInitTechnologyTree
from ipkiss.technology import get_technology
from ipkiss.process.layer import ProcessLayer, PatternPurpose, PPLayer
from ipkiss.geometry.shapes.basic import ShapeRectangle

TECH = get_technology()


# Metal Tree

TECH.METAL = TechnologyTree()
TECH.METAL.LINE_WIDTH = 0.5                                                          # Default line width of metal wire
TECH.METAL.DEFAULT_PROCESS = TECH.PROCESS.M1                                         # Default process for metal wiring


TECH.VIAS = TechnologyTree()
TECH.VIAS.DEFAULT = TechnologyTree()
TECH.VIAS.DEFAULT.TOP_SHAPE = ShapeRectangle(box_size=(0.6, 0.6))                    # Default bottom shape (M1) for a via
TECH.VIAS.DEFAULT.VIA_SHAPE = ShapeRectangle(box_size=(0.5, 0.5))                    # Default via shape
TECH.VIAS.DEFAULT.BOTTOM_SHAPE = ShapeRectangle(box_size=(0.6, 0.6))                 # Default top shape (M2) for a via
