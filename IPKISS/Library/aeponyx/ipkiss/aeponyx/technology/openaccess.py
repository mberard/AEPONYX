"""Settings for IPKISS.eda
These are required technology keys for exporting to OpenAccess (so they can be used in EDA tools that support it)

"""

try:
    import oatools
    HAVE_OA = True

    from ipkiss.technology import get_technology
    from ipkiss.technology.technology import TechnologyTree, DelayedInitTechnologyTree
    from oatools.to_oa.export_layer_map import AutoOALayerExportMap, AutoOAPurposeExportMap
    TECH = get_technology()

    TECH.OPENACCESS = TechnologyTree()

    process_map = {TECH.PROCESS.MSN:     1,
                   TECH.PROCESS.SHALLOW: 2,
                   TECH.PROCESS.M1:      3,
                   TECH.PROCESS.M2:      4,
                   TECH.PROCESS.V12:     5,
                   TECH.PROCESS.SIL:     10,
                   TECH.PROCESS.N:       11,
                   TECH.PROCESS.P:       12,
                   TECH.PROCESS.NPLUS:   13,
                   TECH.PROCESS.PPLUS:   14,
                   TECH.PROCESS.CON:     15,
                   TECH.PROCESS.GE:      16,
                   TECH.PROCESS.NONE:    100
                   }

    purpose_map = {TECH.PURPOSE.BBOX:    21,
                   TECH.PURPOSE.PINREC:  22,
                   TECH.PURPOSE.TEXT:    24,
                   TECH.PURPOSE.TRACE:   25,
                   TECH.PURPOSE.DOC:     50,
                   TECH.PURPOSE.ERROR:   100,
                   TECH.PURPOSE.UNUSED:  101,
                   }

    TECH.OPENACCESS.EXPORT_LAYER_MAP = AutoOALayerExportMap(base_counter=1000, known_layers=process_map)
    TECH.OPENACCESS.EXPORT_PURPOSE_MAP = AutoOAPurposeExportMap(base_counter=1000, known_purposes=purpose_map)

    from ipkiss.primitives.filters.path_cut_filter import PathCutFilter
    from ipkiss.primitives.filters.empty_filter import EmptyFilter
    from ipkiss.primitives.filters.path_to_boundary_filter import PathToBoundaryFilter
    from ipkiss.primitives.filters.boundary_cut_filter import BoundaryCutFilter
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

    TECH.OPENACCESS.FILTER = f

    # Routing in IPKISS-EDA


    class RoutingTechTree(DelayedInitTechnologyTree):

        def initialize(self):
            from aeponyx.components.waveguides.wire import RoundedWireWaveguide
            from aeponyx.components.waveguides.rib import RoundedRibWaveguide
            self.WAVEGUIDE_GENERATION_GUIDE_LAYERS = {TECH.PPLAYER.WIRE_TRACE: RoundedWireWaveguide,
                                                      TECH.PPLAYER.RIB_TRACE: RoundedRibWaveguide}

    TECH.ROUTING = RoutingTechTree()

except:
    HAVE_OA = False

