import matplotlib.pyplot as plt
import numpy as np

points = np.loadtxt('output')

fig, ax = plt.subplots()
for pt in points:
    c = plt.Circle(pt, .5, linestyle='-')
    ax.add_patch(c)

# ax.relim()
# ax.autoscale_view()
# plt.draw()
pt_min = np.min(points)
pt_max = np.max(points)
plt.xlim(pt_min, pt_max)
plt.ylim(pt_min, pt_max)

plt.savefig('output.png')
