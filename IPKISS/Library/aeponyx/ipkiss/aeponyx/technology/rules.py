from ipkiss.technology import get_technology
from ipkiss.technology.technology import TechnologyTree
from ipkiss.technology.technology import DelayedInitTechnologyTree
from ipkiss.geometry.shapes.basic import ShapeRectangle

TECH = get_technology()

####################################################################
# WG SETTINGS
####################################################################

TECH.WIREWG = TechnologyTree()
TECH.WIREWG.WIRE_WIDTH = 0.45
TECH.WIREWG.TRENCH_WIDTH = 20.0
#TECH.WIREWG.CLADDING_WIDTH = TECH.WIREWG.WIRE_WIDTH + 2 * TECH.WIREWG.TRENCH_WIDTH
TECH.WIREWG.CLADDING_WIDTH = 10
TECH.WIREWG.BEND_RADIUS = 100.0
TECH.WIREWG.SPACING = 10.0
TECH.WIREWG.DC_SPACING = TECH.WIREWG.WIRE_WIDTH + 0.18
TECH.WIREWG.SHORT_STRAIGHT = 2.0
TECH.WIREWG.SHORT_TRANSITION_LENGTH = 5.0
TECH.WIREWG.SHORT_TAPER_LENGTH = TECH.WIREWG.SHORT_TRANSITION_LENGTH
TECH.WIREWG.OVERLAP_EXTENSION = 0.020
TECH.WIREWG.OVERLAP_TRENCH = 0.010
TECH.WIREWG.EXPANDED_WIDTH = 0.8
TECH.WIREWG.EXPANDED_TAPER_LENGTH = 3.0
TECH.WIREWG.EXPANDED_STRAIGHT = 5.0
TECH.WIREWG.ANGLE_STEP = 1.0
TECH.WIREWG.SLOT_WIDTH = 0.15
TECH.WIREWG.SLOTTED_WIRE_WIDTH = 0.7

TECH.RIBWG = TechnologyTree()
TECH.RIBWG.RIB_WIDTH = 0.65
TECH.RIBWG.TRENCH_WIDTH = 5.0
TECH.RIBWG.CLADDING_WIDTH = TECH.RIBWG.RIB_WIDTH + 2 * TECH.RIBWG.TRENCH_WIDTH
TECH.RIBWG.BEND_RADIUS = 100.0
TECH.RIBWG.SPACING = 5.0


# Required by ipkiss/picazzo, but not strictly used
TECH.WG = TECH.WIREWG                               # Required for picazzo
TECH.RWG = TECH.RIBWG                               # Required for picazzo
TECH.RWG.WIRE_WIDTH = TECH.RIBWG.RIB_WIDTH          # Required for picazzo
TECH.RWG.STRIP_WIDTH = TECH.RIBWG.RIB_WIDTH + 0.1   # Required for picazzo (RibWireWaveguideTemplate)

TECH.CON = TechnologyTree()
TECH.CON.WIDTH = 0.4

TECH.M1 = TechnologyTree()
TECH.M1.LINE_WIDTH = 0.4
TECH.M2 = TechnologyTree()
TECH.M2.LINE_WIDTH = 0.5

TECH.V12 = TechnologyTree()
TECH.V12.WIDTH = 0.3
TECH.V12.TOP_SHAPE = ShapeRectangle(box_size=(TECH.M2.LINE_WIDTH, TECH.M2.LINE_WIDTH))
TECH.V12.VIA_SHAPE = ShapeRectangle(box_size=(TECH.V12.WIDTH, TECH.V12.WIDTH))
TECH.V12.BOTTOM_SHAPE = ShapeRectangle(box_size=(TECH.M1.LINE_WIDTH, TECH.M1.LINE_WIDTH))

TECH.BONDPAD = TechnologyTree()
TECH.BONDPAD.M1_SIZE = (60.0, 60.0)
TECH.BONDPAD.M2_SIZE = (60.0, 60.0)
TECH.BONDPAD.VIA_PITCH = (2.0, 2.0)

# At the moment, minimum line and space are specified on a global level, not per layer.
TECH.TECH = TechnologyTree()
TECH.TECH.MINIMUM_LINE = 0.120
TECH.TECH.MINIMUM_SPACE = 0.120
