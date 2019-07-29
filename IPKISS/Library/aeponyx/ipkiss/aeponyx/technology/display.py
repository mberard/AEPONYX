# Display styles
#
# Required keys for ipkiss visualization
# Note: this information is also used when exporting to an OpenAccess database: a display.drf file is made, which
# contains the relevant information about the color styles.

from ipkiss.technology import get_technology
from ipkiss.technology.technology import TechnologyTree, DelayedInitTechnologyTree
from ipkiss.visualisation.display_style import DisplayStyle, DisplayStyleSet
from ipkiss.visualisation import color
from ipkiss.visualisation import stipple

TECH = get_technology()

DISPLAY_BLACK = DisplayStyle(color=color.COLOR_BLACK, stipple=stipple.STIPPLE_FILLED, edgewidth=0.0)
DISPLAY_WHITE = DisplayStyle(color=color.COLOR_WHITE, stipple=stipple.STIPPLE_FILLED, edgewidth=0.0)
DISPLAY_BLUE = DisplayStyle(color=color.COLOR_BLUE, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_BLUE_SPARSE = DisplayStyle(color=color.COLOR_BLUE, stipple=stipple.STIPPLE_LINES_DIAGONAL_L, alpha=0.5, edgewidth=1.0)
DISPLAY_BLUECRAYOLA = DisplayStyle(color=color.COLOR_BLUE_CRAYOLA, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_GREEN = DisplayStyle(color=color.COLOR_GREEN, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_GREEN_EMPTY = DisplayStyle(color=color.COLOR_GREEN, stipple=stipple.STIPPLE_NONE, alpha=0.5, edgewidth=1.0)
DISPLAY_DARKGREEN = DisplayStyle(color=color.COLOR_DARK_GREEN, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_DEEPGREEN = DisplayStyle(color=color.COLOR_DEEP_GREEN, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_YELLOW = DisplayStyle(color=color.COLOR_YELLOW, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_YELLOW_SPARSE = DisplayStyle(color=color.COLOR_YELLOW, stipple=stipple.STIPPLE_LINES_DIAGONAL_L, alpha=0.5, edgewidth=1.0)
DISPLAY_MAGENTA_SPARSE = DisplayStyle(color=color.COLOR_MAGENTA, stipple=stipple.STIPPLE_LINES_V_DENSE, alpha=0.5, edgewidth=1.0)
DISPLAY_SCARLET = DisplayStyle(color=color.COLOR_SCARLET, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_RED = DisplayStyle(color=color.COLOR_RED, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_CYAN = DisplayStyle(color=color.COLOR_CYAN, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_CYAN_SPARSE = DisplayStyle(color=color.COLOR_CYAN, stipple=stipple.STIPPLE_LINES_V_DENSE, alpha=0.5, edgewidth=1.0)
DISPLAY_SANGRIA_SPARSE = DisplayStyle(color=color.COLOR_SANGRIA, stipple=stipple.STIPPLE_LINES_V_DENSE, alpha=0.5, edgewidth=1.0)
DISPLAY_PURPLE = DisplayStyle(color=color.COLOR_PURPLE, stipple=stipple.STIPPLE_LINES_H_BOLD, alpha=0.5, edgewidth=1.0)
DISPLAY_TITANIUM_YELLOW = DisplayStyle(color=color.COLOR_TITANIUM_YELLOW, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_COPPER = DisplayStyle(color=color.COLOR_COPPER, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_CHAMPAGNE = DisplayStyle(color=color.COLOR_CHAMPAGNE, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_SILVER = DisplayStyle(color=color.COLOR_SILVER, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_GRAY = DisplayStyle(color=color.COLOR_GRAY, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_CHERRY = DisplayStyle(color=color.COLOR_CHERRY, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_VIOLET = DisplayStyle(color=color.COLOR_BLUE_VIOLET, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_ORANGE = DisplayStyle(color=color.COLOR_ORANGE, stipple=stipple.STIPPLE_FILLED, alpha=0.5, edgewidth=1.0)
DISPLAY_GREEN_SPARSE = DisplayStyle(color=color.COLOR_GREEN, stipple=stipple.STIPPLE_LINES_DIAGONAL_L, alpha=0.5, edgewidth=1.0)
DISPLAY_RED_SPARSE = DisplayStyle(color=color.COLOR_RED, stipple=stipple.STIPPLE_LINES_DIAGONAL_L, alpha=0.5, edgewidth=1.0)
DISPLAY_INVISBLE = DisplayStyle(color=color.COLOR_WHITE, stipple=stipple.STIPPLE_NONE, alpha=0.0, edgewidth=0.)


class TechDisplayTree(DelayedInitTechnologyTree):

    def initialize(self):
        from ipkiss.process import PPLayer

        self.PREDEFINED_STYLE_SETS = TechnologyTree()

        style_set = DisplayStyleSet()
        style_set.background = DISPLAY_WHITE

        style_set += [
            # Note: the order in which the layer:color pairs are added here decides the drawing order.
            (TECH.PPLAYER.MSN,            DISPLAY_GREEN),
            (TECH.PPLAYER.MSN_TRENCH,     DISPLAY_ORANGE),

            (TECH.PPLAYER.CLADDING,       DISPLAY_YELLOW),

            (TECH.PPLAYER.SHALLOW,        DISPLAY_CYAN_SPARSE),
            (TECH.PPLAYER.SHALLOW_TRENCH, DISPLAY_SCARLET),

            (TECH.PPLAYER.M1,             DISPLAY_COPPER),
            (TECH.PPLAYER.V12,            DISPLAY_CHAMPAGNE),
            (TECH.PPLAYER.M2,             DISPLAY_SILVER),

            (TECH.PPLAYER.HT,             DISPLAY_GRAY),
            (TECH.PPLAYER.GE,             DISPLAY_YELLOW),
            (TECH.PPLAYER.CON,            DISPLAY_TITANIUM_YELLOW),
            (TECH.PPLAYER.SIL,            DISPLAY_VIOLET),

            (TECH.PPLAYER.N,              DISPLAY_BLUE_SPARSE),
            (TECH.PPLAYER.NPLUS,          DISPLAY_BLUE),
            (TECH.PPLAYER.P,              DISPLAY_RED_SPARSE),
            (TECH.PPLAYER.PPLUS,          DISPLAY_RED),

            (TECH.PPLAYER.PINREC,         DISPLAY_ORANGE),
            (TECH.PPLAYER.ERROR.GENERIC,  DISPLAY_PURPLE),
            (TECH.PPLAYER.DOC,            DISPLAY_CHERRY),
            (TECH.PPLAYER.NONE,           DISPLAY_INVISBLE),

            (TECH.PPLAYER.WIRE_TRACE,     DISPLAY_MAGENTA_SPARSE),
            (TECH.PPLAYER.RIB_TRACE,      DISPLAY_SANGRIA_SPARSE),
        ]

        from ipkiss.visualisation.color import Color
        from numpy import linspace
        style_set += [(i, DisplayStyle(color=Color(name="gray_" + str(i),      # Visualization for simple Layers which may be present
                                                   red=c_val,
                                                   green=c_val,
                                                   blue=c_val),
                                       alpha=.5))
                      for i, c_val in enumerate(linspace(.9, 0.0, num=256))]

        self.DEFAULT_DISPLAY_STYLE_SET = style_set   # required

TECH.DISPLAY = TechDisplayTree()   # required
