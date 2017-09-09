###################################################################################################
### Written by : Eric Tan
###
### Plot script for plotting the timing of quicksort.
###################################################################################################

import numpy as np
import matplotlib.pyplot as plt

N = np.loadtxt("timing.txt", usecols=(0,))
hybrid    = np.loadtxt("timing.txt", usecols=(1,))
task      = np.loadtxt("timing.txt", usecols=(2,))
random    = np.loadtxt("timing.txt", usecols=(3,))
quicksort = np.loadtxt("timing.txt", usecols=(4,))
mergesort = np.loadtxt("timing.txt", usecols=(5,))

plt.figure(figsize=(20, 16), dpi=80, facecolor='w', edgecolor='k')
plt.rcParams.update({'font.size':12})

plt.loglog(N, hybrid)
plt.loglog(N, task)
plt.loglog(N, random)
plt.loglog(N, quicksort)
plt.loglog(N, mergesort)

plt.title("Comparsion of Different Quicksort Implamentations")
plt.grid(True)
plt.xlabel("Number of elements")
plt.ylabel(r"Timing $\mu$s")
plt.legend(["Hybrid", "Task Parallel", "Randomize Pivot", "Standard", "Mergesort (reference)"])

plt.show()
