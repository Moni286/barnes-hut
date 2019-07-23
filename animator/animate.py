import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib as mpl
import numpy as np
import sys
import math
import time

# Code adapted from https://stackoverflow.com/questions/9401658/how-to-animate-a-scatter-plot

class AnimatedScatter(object):
    """An animated scatter plot using matplotlib.animations.FuncAnimation."""
    def __init__(self):
        self.stream = self.data_stream()

        # Setup the figure and axes...
        self.fig, self.ax = plt.subplots()

        # Writer = animation.writers['ffmpeg']
        # writer = Writer(fps=30, metadata=dict(artist='x'), bitrate=4000)

        self.ani = animation.FuncAnimation(self.fig, self.update, interval=1, 
                                          init_func=self.setup_plot, blit=True)

        self.lastFrameTime = time.time()

        # self.ani.save("particles.mp4", writer=writer)


    def setup_plot(self):
        x, y, s,= next(self.stream).T
        self.scat = self.ax.scatter(x, y, s=0.565, c='#bae9ff')
        
        self.ax.set_facecolor((0, 0, 0))

        self.fpsTxt = self.ax.text(0.05, 0.95, '', transform=plt.gca().transAxes, va = "top", ha="left", color="w")
        return self.scat, self.fpsTxt

    def data_stream(self):
        xy = np.zeros((self.numpoints, 2))
        s = np.zeros(self.numpoints)
        while True:
            i=0
            for line in sys.stdin:
                if line == "ENDITER\n":
                    
                    if self.numpoints != i:
                        self.numpoints = i
                        xy = np.zeros((self.numpoints, 2))
                        s = np.zeros(self.numpoints)
                    break

                try:
                    row = line.split(",")
                    row = [float(row[0]), float(row[1]), float(row[2])]

                    xy[i, 0] = row[0]
                    xy[i, 1] = row[1]
                    s[i] = math.log(row[2])
                    s[i] = 0.565 if s[i] < 0.565 else s[i]
                    i += 1
                except ValueError:
                    print("ValueError: could not convert line to float values\n\t" + line)

            yield np.c_[xy[:,0], xy[:,1], s]

    def update(self, i):
        
        data = next(self.stream)

        self.scat.set_sizes(data[:, 2])
        self.scat.set_offsets(data[:, :2])

        elapsed = time.time() - self.lastFrameTime
        self.lastFrameTime = time.time()
        self.fpsTxt.set_text("FPS: %.1f" % (1 / elapsed))
        
        axis = list(self.ax.axis())

        minX = np.min(data[:,0])
        maxX = np.max(data[:,0])
        minY = np.min(data[:,1])
        maxY = np.max(data[:,1])

        axesChange = False
        if (minX < axis[0]):
            axesChange = True
        
        if (maxX > axis[1]):
            axesChange = True

        if (minY < axis[2]):
            axesChange = True

        if (maxY > axis[3]):
            axesChange = True

        if axesChange == True:
            axis[0] -= 100000
            axis[1] += 100000
            axis[2] -= 100000
            axis[3] += 100000
            self.ax.axis(axis, emit=True)
            return (self.ax, self.fpsTxt, self.scat)
        
        return (self.fpsTxt, self.scat)



if __name__ == '__main__':

    try:
        a = AnimatedScatter()

        plt.show()

    except KeyboardInterrupt:
        quit()

    print("done")



