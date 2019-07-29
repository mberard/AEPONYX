from aeponyx import technology
from aeponyx.components.mmi.cell import MMI
from ipkiss3 import all as i3
import numpy as np


class MMISweep(i3.PCell):

    mmis = i3.ChildCellListProperty()
    lengths = i3.NumpyArrayProperty()

    def _default_lengths(self):
        return np.linspace(30, 40.0, 10)

    def _default_mmis(self):
        mmis = []
        for cnt, l in enumerate(self.lengths):
            mmi = MMI(name=self.name + "_mmi_{}".format(l))
            mmi.Layout(length=l)
            mmis.append(mmi)
        return mmis

    class Layout(i3.LayoutView):
        mmi_sep = i3.PositiveNumberProperty(default=25.0 * 3 / 2.0)

        def _generate_instances(self, insts):
            for cnt, mmi in enumerate(self.mmis):
                si = mmi.size_info()
                t = i3.Translation(translation=(0, cnt * self.mmi_sep))
                if cnt % 2 == 0:
                    transformation = i3.HMirror(mirror_plane_x=si.center[0]) + t
                else:
                    transformation = t
                insts += i3.SRef(name="mmi_{}".format(cnt), reference=mmi, transformation=transformation)
            return insts

        def _generate_ports(self, ports):
            for cnt, i in enumerate(self.instances.itervalues()):
                for p in i.ports:
                    ports += p.modified_copy(name="{}_{}".format(p.name, cnt))

            return ports


cell = MMISweep(name="MMIsweep")
lv = cell.Layout()
lv.visualize(annotate=True)
lv.write_gdsii("mmi_sweep.gds")

from picazzo3.container.iofibcoup import IoFibcoup
cont = IoFibcoup(contents=cell)
cont_layout = cont.Layout()
cont_layout.write_gdsii("mmi_sweep_routed.gds")
print "done"
