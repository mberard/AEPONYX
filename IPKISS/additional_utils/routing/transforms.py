from ipkiss3 import all as i3

def relative_placer(childcell_dict,
                    transformation_dict,
                    port1,
                    port2,
                    translation=(0.0, 0.0),
                    rotation=0.0):
    port_strings = [port1, port2]
    layouts = []
    ports = []
    instance_names = []
    for p in port_strings:
        instance_name, port_name = p.split(":")
        instance_names.append([instance_name])
        layouts.append(childcell_dict[instance_name].get_default_view(i3.LayoutView))
        ports.append(layouts[-1].ports[port_name])

    return i3.Rotation(rotation_center=ports[1], rotation=rotation) + \
           i3.vector_match_transform(ports[1], ports[0]) + \
           i3.Translation(translation=translation) + transformation_dict[instance_names[0][0]]



