from crossing_utils import TraceChainWithCenterLine
from ipkiss3 import all as i3


def combine_connectors(connector_functions=[], transformations=[]):
    """
    :param connector_functions: List of routing functions that you want aggregate
    :param transformations: : List transformations for the intermediate points relative to the starting port
    :return: new_routing function. shape and connector_kwargs are unused in the new function.
    """

    if len(connector_functions) != len(transformations) + 1:
        raise Exception("The length of the transformation array need to be equal to the length of the routing functions - 1")

    cleaned_transformations = []
    for t in transformations:
        if isinstance(t, tuple):
            cleaned_transformations.append((i3.Translation(t)))
        else:
            cleaned_transformations.append(t)

    def new_function(start_port, end_port, name=None,  shape=None, connector_kwargs={}):

        start_ports = [start_port] + [i3.OpticalPort(trace_template=start_port.trace_template,
                                                     position=(0.0, 0.0)).transform(transformation=t)
                                      for t in cleaned_transformations]

        end_ports = [p.modified_copy(angle=p.angle + 180.0)
                     for p in start_ports[1:]] + [end_port]

        traces = []
        for cnt, (cf, sp, ep) in enumerate(zip(connector_functions, start_ports, end_ports)):
            seg_name = "{}_segment_{}".format(name, cnt)
            traces.append(cf(start_port=sp, end_port=ep, name=seg_name))


        return TraceChainWithCenterLine(name=name, traces=traces)

    return new_function



