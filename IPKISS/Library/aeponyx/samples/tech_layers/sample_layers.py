"""
This sample draws a square with text label for each process-purpose layer defined in the tech tree
"""

from aeponyx import technology
import ipkiss3.all as i3

from ipkiss.technology.technology import TechnologyTree


def get_pplayers_from_tree(techtree):
    pplayers = []
    for pplayer in techtree.keys():
        ppl = techtree[pplayer]
        if isinstance(ppl, TechnologyTree):
            pplayers.extend(get_pplayers_from_tree(ppl))
        elif isinstance(ppl, i3.PPLayer):
            pplayers.append(ppl)
    return pplayers


class SampleLayers(i3.PCell):

    class Layout(i3.LayoutView):

        def _generate_elements(self, elems):

            num_x = 5
            x_step = 200
            y_step = 200

            pplayers = get_pplayers_from_tree(i3.TECH.PPLAYER)

            xcnt = 0
            ycnt = 0

            for p in pplayers:

                x = xcnt * x_step
                y = ycnt * y_step

                if not (p.process, p.purpose) in i3.TECH.GDSII.LAYERTABLE:
                    continue
                elems += i3.Rectangle(layer=p, center=(x, y), box_size=(100.0, 100.0))
                elems += i3.PolygonText(layer=p, text=p.name, coordinate=(x, y + 75.0), height=15.0)

                if xcnt == num_x - 1:
                    xcnt = 0
                    ycnt += 1
                else:
                    xcnt += 1

            return elems


S = SampleLayers(name="Layers")
S_layout = S.Layout()
S_layout.write_gdsii("sample_layers.gds")
print("Done, written layers to sample_layers.gds")
