# GDSII IMPORT AND EXPORT SETTINGS
#
# The technology keys below are required keys for Ipkiss
# You can change the values or refer them to your own technology keys, but their name should not be changed

from ipkiss.technology import get_technology
from ipkiss.technology.technology import TechnologyTree
from ipkiss.process.layer_map import GenericGdsiiPPLayerOutputMap, GenericGdsiiPPLayerInputMap
import string

# Layer mapping
TECH = get_technology()

from ipkiss.technology.technology import DelayedInitTechnologyTree
TECH = get_technology()
TECH.GDSII = TechnologyTree()


##################################
# SETTINGS
##################################
TECH.GDSII.STRNAME_CHARACTER_DICT = {" -./": "_"}                                           # Mapping for illegal characters in cell names
TECH.GDSII.STRNAME_ALLOWED_CHARACTERS = string.ascii_letters + string.digits + '_$'         # Allowed characters in cell names

TECH.GDSII.MAX_COORDINATES = 200                                                            # Max number of vertices in a path
TECH.GDSII.MAX_PATH_LENGTH = 100                                                            # Max number of control points in a path
TECH.GDSII.MAX_VERTEX_COUNT = 4000                                                          # Max number of vertices in a polygon
TECH.GDSII.MAX_NAME_LENGTH = 255                                                            # Max length of a cell name

##################################
# LAYER MAP
##################################

TECH.GDSII.LAYERTABLE = {
    # (ProcessLayer, PatternPurpose) : (GDSIILayer, GDSIIDatatype)
    (TECH.PROCESS.MSN,  TECH.PURPOSE.DRAWING): (10, 0),
    (TECH.PROCESS.MSN,  TECH.PURPOSE.TEXT): (10, 11),

    (TECH.PROCESS.MSN_TRENCH,   TECH.PURPOSE.DRAWING): (10, 51),

    (TECH.PROCESS.SHALLOW,  TECH.PURPOSE.DRAWING): (11, 0),
    (TECH.PROCESS.SHALLOW,  TECH.PURPOSE.DRWSUB): (11, 2),

    (TECH.PROCESS.CLADDING, TECH.PURPOSE.DRAWING): (20, 0),

    (TECH.PROCESS.N,        TECH.PURPOSE.DRAWING): (5, 0),
    (TECH.PROCESS.NPLUS,    TECH.PURPOSE.DRAWING): (6, 0),
    (TECH.PROCESS.P,        TECH.PURPOSE.DRAWING): (7, 0),
    (TECH.PROCESS.PPLUS,    TECH.PURPOSE.DRAWING): (8, 0),

    (TECH.PROCESS.SIL,      TECH.PURPOSE.DRAWING): (10, 0),
    (TECH.PROCESS.CON,      TECH.PURPOSE.DRAWING): (11, 0),

    (TECH.PROCESS.HT,       TECH.PURPOSE.DRAWING): (15, 0),
    (TECH.PROCESS.M1,       TECH.PURPOSE.DRAWING): (20, 0),
    (TECH.PROCESS.V12,      TECH.PURPOSE.DRAWING): (21, 0),
    (TECH.PROCESS.M2,       TECH.PURPOSE.DRAWING): (22, 0),

    (TECH.PROCESS.GE,       TECH.PURPOSE.DRAWING): (30, 0),

    (TECH.PROCESS.NONE,     TECH.PURPOSE.DOC):     (50, 0),
    (TECH.PROCESS.NONE,     TECH.PURPOSE.ERROR):   (50, 1),
    (TECH.PROCESS.NONE,     TECH.PURPOSE.UNUSED):  (50, 2),
}
TECH.GDSII.EXPORT_LAYER_MAP = GenericGdsiiPPLayerOutputMap(pplayer_map=TECH.GDSII.LAYERTABLE,       # GDSII export map - required
                                                           ignore_undefined_mappings=False)
TECH.GDSII.IMPORT_LAYER_MAP = GenericGdsiiPPLayerInputMap(pplayer_map=TECH.GDSII.LAYERTABLE)        # GDSII import map - required


##################################
# FILTERS
##################################

from ipkiss.primitives.filters.path_cut_filter import PathCutFilter
from ipkiss.primitives.filters.empty_filter import EmptyFilter
from ipkiss.primitives.filters.path_to_boundary_filter import PathToBoundaryFilter
from ipkiss.primitives.filters.boundary_cut_filter import BoundaryCutFilter
from ipkiss.primitives.filters.name_scramble_filter import NameScrambleFilter
from ipkiss.primitives.filters.name_error_filter import PCellNameErrorFilter
from ipkiss.primitives.filter import ToggledCompoundFilter

f = ToggledCompoundFilter()
f += PathCutFilter(name="cut_path",
                   max_path_length=TECH.GDSII.MAX_COORDINATES,
                   grids_per_unit=int(TECH.METRICS.UNIT / TECH.METRICS.GRID),
                   overlap=1)
f += PathToBoundaryFilter(name="path_to_boundary")
f += BoundaryCutFilter(name="cut_boundary", max_vertex_count=TECH.GDSII.MAX_VERTEX_COUNT)
f += EmptyFilter(name="write_empty")
f += PCellNameErrorFilter(name="name_error_filter", allowed_characters=TECH.GDSII.STRNAME_ALLOWED_CHARACTERS)
f["cut_path"] = True
f["path_to_boundary"] = True
f["cut_boundary"] = True
f["write_empty"] = True
f["name_error_filter"] = False
TECH.GDSII.FILTER = f                             # GDSII export filters (several filter which can be toggled on or off) - required

TECH.GDSII.NAME_FILTER = NameScrambleFilter(allowed_characters=TECH.GDSII.STRNAME_ALLOWED_CHARACTERS,   # GDSII cell name filter - required
                                            replace_characters=TECH.GDSII.STRNAME_CHARACTER_DICT,
                                            default_replacement="",
                                            max_name_length=TECH.GDSII.MAX_NAME_LENGTH,
                                            scramble_all=False)
