###############
# CUSTOM COLORS
###############
from ipkiss.visualisation.color import Color
COLOR_BABY_BLUE = Color(name="Baby blue", red=0.78, green=0.84, blue=0.91)
COLOR_UGLY_PINK = Color(name="Ugly pink", red=0.82, green=0.59, blue=0.58)
COLOR_DARK_UGLY_PINK = Color(name="Dark ugly pink", red=0.63, green=0.43, blue=0.43)
COLOR_LIGHT_UGLY_PINK = Color(name="Ligtht ugly pink", red=0.90, green=0.76, blue=0.76)
COLOR_DIAPER_BLUE = Color(name="Diaper blue", red=0.58, green=0.70, blue=0.84)
COLOR_COPPER_BROWN = Color(name="Copper Brown", red=200.0/255.0, green=117.0/255.0, blue=51.0/255.0)
COLOR_AL_GRAY = Color(name="Aluminum Gray", red=173.0/255, green=178.0/255, blue=189.0/255)
COLOR_LIGHT_BLUE = Color(name="Lightblue", red=0.4, green=0.4, blue=0.9)
COLOR_DARK_BLUE = Color(name="Darkblue", red=0.0, green=0.0, blue=0.4)
COLOR_LIGHT_RED = Color(name="Lightred", red=0.9, green=0.4, blue=0.4)
COLOR_DARK_RED = Color(name="Darkred", red=0.4, green=0.0, blue=0.0)
COLOR_TUNGSTEN = Color(name="Tungsten", red=0.47, green=0.49, blue=0.52)

####################################################################
# MATERIALS
####################################################################
from ipkiss.technology import get_technology
from pysics.materials import electromagnetics as _
from pysics.basics.material.material import Material, MaterialFactory
from ipkiss.visualisation.display_style import DisplayStyle
from ipkiss.visualisation import color

TECH = get_technology()

TECH.overwrite_allowed.append('MATERIALS')
TECH.MATERIALS = MaterialFactory()

TECH.MATERIALS.AIR = Material(name="air", display_style=DisplayStyle(color=color.COLOR_BLUE_CRAYOLA), solid=False)
TECH.MATERIALS.MSN = Material(name="medium stress nitride", display_style=DisplayStyle(color=COLOR_UGLY_PINK))
TECH.MATERIALS.SILICON_OXIDE = Material(name="silicon oxide", display_style=DisplayStyle(color=COLOR_BABY_BLUE))
TECH.MATERIALS.P_SILICON = Material(name="P+ silicon", display_style=DisplayStyle(color=COLOR_LIGHT_RED))
TECH.MATERIALS.N_SILICON = Material(name="N+ silicon", display_style=DisplayStyle(color=COLOR_LIGHT_BLUE))
TECH.MATERIALS.PPLUS_SILICON = Material(name="P++-silicon", display_style=DisplayStyle(color=COLOR_DARK_RED))
TECH.MATERIALS.NPLUS_SILICON = Material(name="N++-silicon", display_style=DisplayStyle(color=COLOR_DARK_BLUE))
TECH.MATERIALS.GERMANIUM = Material(name="germanium", display_style=DisplayStyle(color=color.COLOR_CHERRY))
TECH.MATERIALS.SILICIDE = Material(name="silicide", display_style=DisplayStyle(color=color.COLOR_COPPER))
TECH.MATERIALS.TUNGSTEN = Material(name="tungsten", display_style=DisplayStyle(color=COLOR_TUNGSTEN))
TECH.MATERIALS.COPPER = Material(name="copper", display_style=DisplayStyle(color=COLOR_COPPER_BROWN))
TECH.MATERIALS.ALUMINIUM = Material(name="aluminium", display_style=DisplayStyle(color=COLOR_AL_GRAY))


TECH.MATERIALS.MSN.epsilon = 8
TECH.MATERIALS.SILICON_OXIDE.epsilon = 2.3104
TECH.MATERIALS.AIR.epsilon = 1


####################################################################
# MATERIALS STACKS
####################################################################

from pysics.basics.material.material_stack import MaterialStack, MaterialStackFactory

TECH.overwrite_allowed.append('MATERIAL_STACKS')
TECH.MATERIAL_STACKS = MaterialStackFactory()

# Thicknesses of the differents layers

MSTACK_ONO_SILICON_OXIDE_HEIGHT = 3.2
MSTACK_ONO_MSN_HEIGHT = 0.44
MSTACK_ONO_RIB_HEIGHT = 0.22
MSTACK_SILICIDE_HEIGHT = 0.05
MSTACK_CON_HEIGHT = 0.5 - (MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT) # always have 0.5 contact thickness in total
MSTACK_M1_HEIGHT = 0.5
MSTACK_IMD12_HEIGHT = 1.0
MSTACK_M2_HEIGHT = 0.5

# Material stacks in the MSN layer
TECH.MATERIAL_STACKS.MSTACK_ONO_INVALID = MaterialStack(name="INVALID",
                                                        materials_heights=[(TECH.MATERIALS.AIR, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                           (TECH.MATERIALS.AIR, MSTACK_ONO_MSN_HEIGHT)],
                                                        display_style=DisplayStyle(color=color.COLOR_WHITE))

TECH.MATERIAL_STACKS.MSTACK_ONO_OX = MaterialStack(name="Oxide",
                                                   materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                      (TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_MSN_HEIGHT)],
                                                   display_style=DisplayStyle(color=COLOR_BABY_BLUE))

TECH.MATERIAL_STACKS.MSTACK_ONO_MSN = MaterialStack(name="{}nm MSN".format(int(MSTACK_ONO_MSN_HEIGHT*1000)),
                                                   materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                      (TECH.MATERIALS.MSN, MSTACK_ONO_MSN_HEIGHT)],
                                                   display_style=DisplayStyle(color=COLOR_DARK_UGLY_PINK))

TECH.MATERIAL_STACKS.MSTACK_ONO_RIB = MaterialStack(name="{}nm MSN".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                    materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                       (TECH.MATERIALS.MSN, MSTACK_ONO_RIB_HEIGHT),
                                                                       (TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                    display_style=DisplayStyle(color=COLOR_LIGHT_UGLY_PINK))

TECH.MATERIAL_STACKS.MSTACK_ONO_N = MaterialStack(name="{}nm N+ implant".format(int(MSTACK_ONO_MSN_HEIGHT*1000)),
                                                  materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                     (TECH.MATERIALS.N_SILICON, MSTACK_ONO_MSN_HEIGHT)],
                                                  display_style=DisplayStyle(color=color.COLOR_BLUE))

TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_N = MaterialStack(name="{}nm N+ implant".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                      materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                         (TECH.MATERIALS.N_SILICON, MSTACK_ONO_RIB_HEIGHT),
                                                                         (TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                      display_style=DisplayStyle(color=COLOR_LIGHT_BLUE))

TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS = MaterialStack(name="{}nm N++ implant".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                      materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                         (TECH.MATERIALS.NPLUS_SILICON, MSTACK_ONO_RIB_HEIGHT),
                                                                         (TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                      display_style=DisplayStyle(color=COLOR_DARK_BLUE))

TECH.MATERIAL_STACKS.MSTACK_ONO_NPLUS_CON = MaterialStack(name="{}nm N++ implant + Contact".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                          materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                             (TECH.MATERIALS.NPLUS_SILICON, MSTACK_ONO_RIB_HEIGHT),
                                                                             (TECH.MATERIALS.TUNGSTEN, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                          display_style=DisplayStyle(color=COLOR_TUNGSTEN))

TECH.MATERIAL_STACKS.MSTACK_ONO_P = MaterialStack(name="{}nm P+ implant".format(int(MSTACK_ONO_MSN_HEIGHT*1000)),
                                                  materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                     (TECH.MATERIALS.P_SILICON, MSTACK_ONO_MSN_HEIGHT)],
                                                  display_style=DisplayStyle(color=color.COLOR_RED))

TECH.MATERIAL_STACKS.MSTACK_ONO_RIB_P = MaterialStack(name="{}nm P+ implant".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                      materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                         (TECH.MATERIALS.P_SILICON, MSTACK_ONO_RIB_HEIGHT),
                                                                         (TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                      display_style=DisplayStyle(color=COLOR_LIGHT_RED))

TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS = MaterialStack(name="{}nm P++ implant".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                      materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                         (TECH.MATERIALS.PPLUS_SILICON, MSTACK_ONO_RIB_HEIGHT),
                                                                         (TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                      display_style=DisplayStyle(color=COLOR_DARK_RED))

TECH.MATERIAL_STACKS.MSTACK_ONO_PPLUS_CON = MaterialStack(name="{}nm P++ implant + Contact".format(int(MSTACK_ONO_RIB_HEIGHT*1000)),
                                                          materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_ONO_SILICON_OXIDE_HEIGHT),
                                                                             (TECH.MATERIALS.PPLUS_SILICON, MSTACK_ONO_RIB_HEIGHT),
                                                                             (TECH.MATERIALS.TUNGSTEN, MSTACK_ONO_MSN_HEIGHT-MSTACK_ONO_RIB_HEIGHT)],
                                                          display_style=DisplayStyle(color=COLOR_TUNGSTEN))


# Material stacks in the metal layer (only M1)
TECH.MATERIAL_STACKS.MSTACK_METAL_INVALID = MaterialStack(name="INVALID",
                                                          materials_heights=[(TECH.MATERIALS.AIR, MSTACK_CON_HEIGHT),
                                                                             (TECH.MATERIALS.AIR, MSTACK_M1_HEIGHT)],
                                                          display_style=DisplayStyle(color=color.COLOR_WHITE))

TECH.MATERIAL_STACKS.MSTACK_METAL_DIEL = MaterialStack(name="Oxide",
                                                       materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_CON_HEIGHT),
                                                                          (TECH.MATERIALS.SILICON_OXIDE, MSTACK_M1_HEIGHT)],
                                                       display_style=DisplayStyle(color=COLOR_BABY_BLUE))

TECH.MATERIAL_STACKS.MSTACK_METAL_M1 = MaterialStack(name="M1",
                                                     materials_heights=[(TECH.MATERIALS.SILICON_OXIDE, MSTACK_CON_HEIGHT),
                                                                        (TECH.MATERIALS.ALUMINIUM, MSTACK_M1_HEIGHT)],
                                                     display_style=DisplayStyle(color=COLOR_AL_GRAY))

TECH.MATERIAL_STACKS.MSTACK_METAL_M1_CON = MaterialStack(name="M1 + Contact",
                                                         materials_heights=[(TECH.MATERIALS.TUNGSTEN, MSTACK_CON_HEIGHT),
                                                                            (TECH.MATERIALS.ALUMINIUM, MSTACK_M1_HEIGHT)],
                                                         display_style=DisplayStyle(color=COLOR_TUNGSTEN))

#neff @1550
TECH.MATERIAL_STACKS.MSTACK_ONO_RIB.effective_index_epsilon = 2.539**2
TECH.MATERIAL_STACKS.MSTACK_ONO_MSN.effective_index_epsilon = 2.844**2
TECH.MATERIAL_STACKS.MSTACK_ONO_OX.effective_index_epsilon = 1.44**2
