import camfr
from ipkiss3 import all as i3
import numpy as np
import pylab
from ipkiss.plugins.simulation import camfr_stack_expr_for_structure


def simulate_splitter(layout,
                      wavelength=1.5,
                      num_mod=10,
                      discretization_res=30,
                      north=2.0,
                      plot=True,
                      aspect='equal'):
    """
    Simulates a symmetric splitter and returns the transmission and reflection

    Parameters
    ----------

    layout : LayoutView
      layoutview of the splitter
    wavelength: float
      Wavelength of simulation in um
    num_mod: int
      number_of_modes
    discretization_res : int
      discratization resolution of the structure.
    north : northbound limit of the simulation
    plot : bool
      plot the fiels if True

    Retunrs
    --------

    transmission,reflection
       absolute value of transmission and reflection
    """

    # simulate again for the optimum wavelength
    camfr.set_lambda(wavelength)
    camfr.set_N(num_mod)  # we can see convergence when increasing the number of modes to 20, 30, ... : the result doesn't change
    camfr.set_polarisation(camfr.TM)  # TE in 3D is equivalent to TM in 2D
    camfr.set_upper_PML(-0.05)
    camfr.set_lower_wall(camfr.slab_E_wall)
    camfr.set_backward_modes(True)
    if len(layout.ports):
        sim_window = i3.SizeInfo(west=layout.ports["W0"].position.x,
                                 east=layout.ports["E0"].position.x,
                                 south=-0.0,
                                 north=north)
    else:
        si_layout = layout.size_info()
        sim_window = i3.SizeInfo(west=si_layout.west,
                                 east=si_layout.east,
                                 south=-0.0,
                                 north=north)

    stack_expr = camfr_stack_expr_for_structure(structure=layout,
                                                discretisation_resolution=discretization_res,
                                                window_size_info=sim_window,
                                                process_flow=i3.TECH.VFABRICATION.PROCESS_FLOW)

    camfr_stack = camfr.Stack(stack_expr)
    # run the simulation

    # set incident field
    inc = np.zeros(camfr.N())
    inc[0] = 1
    camfr_stack.set_inc_field(inc)
    camfr.set_lower_wall(camfr.slab_E_wall)
    camfr_stack.calc()

    transmission = abs(camfr_stack.T12(0, 0))**2
    reflection = abs(camfr_stack.R12(0, 0))**2

    if plot:

        # Create a coordinate mesh
        grid_step = 0.05
        x_coords = np.arange(0.0, sim_window.width, grid_step)
        y_coords = np.arange(0.0, sim_window.height, grid_step)
        x_mesh, y_mesh = np.meshgrid(x_coords, y_coords)

        n = np.ndarray(shape=np.shape(x_mesh), dtype=np.complex)

        #################
        # Extracting and plotting field profiles
        #################
        fields = np.ndarray(shape=np.shape(x_mesh), dtype=np.complex)

        for ix, x in enumerate(x_coords):
            for iy, y in enumerate(y_coords):
                coord = camfr.Coord(y, 0.0, x)  # different coordinate system
                fields[iy, ix] = camfr_stack.field(coord).H2()
                n[iy, ix] = camfr_stack.n(coord)

        # Make a red-blue colormap
        rb_map = {'red':  ((0.0, 0.0, 0.0),
                           (0.5, 1.0, 1.0),
                           (1.0, 1.0, 1.0)),

                  'green': ((0.0, 0.0, 0.0),
                            (0.5, 1.0, 1.0),
                            (1.0, 0.0, 0.0)),

                  'blue':  ((0.0, 1.0, 1.0),
                            (0.5, 1.0, 1.0),
                            (1.0, 0.0, 0.0)),

                  }
        pylab.register_cmap(name='RedBlue', data=rb_map)

        # Plot real part of fields with red-blue colormap

        ####################
        # Overlay refractive index on field plot
        ####################
        real_field = np.real(fields)
        limits = [-np.max(np.abs(real_field)), np.max(np.abs(real_field))]
        pylab.figure()
        pylab.contourf(x_mesh, y_mesh, real_field, 100, cmap='RedBlue', clim=limits)
        pylab.contourf(x_mesh, y_mesh, np.real(n), cmap='Greys', alpha=0.1)
        pylab.axis('image')
        ax = pylab.gca()
        ax.set_aspect(aspect)
        pylab.show()

    return (transmission, reflection)
