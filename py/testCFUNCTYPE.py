#-*- coding:UTF-8-*-
#######################

# for python3.4
#from tkinter import *
#from tkinter.filedialog import askopenfilename
# for python 2.7
from Tkinter import *
from tkFileDialog import askopenfilename
from multiprocessing import Process
from ctypes import *

import ROOT 

import threading
import time  
import numpy as np
import matplotlib.pyplot as plt
import pylab as pl
#import app_start as app


def frame(root, side):
    w = Frame(root)
    w.pack(side=side, expand=YES, fill=BOTH)
    return w
#end of def
 
def button(root, side, text, command=None):
    w = Button(root, text=text, command=command)
    w.pack(side=side, expand=YES, fill=BOTH)
    return w
#end of def
 
class vmeGUI(Frame):
    def __init__(self):
        Frame.__init__(self)
        self.option_add('*Font', 'Verdana 12 bold')
        self.pack(expand=YES, fill=BOTH)
        self.master.title('Simple Cal')
        self.master.iconname('calc1')
        
        display = StringVar()
        cfgfilename = StringVar()
        Entry(self, relief=SUNKEN, textvariable=display).pack(side=TOP, expand=YES, fill=BOTH)
        Entry(self, relief=SUNKEN, textvariable=cfgfilename).pack(side=TOP, expand=YES, fill=BOTH)
        swF0 = frame(self, TOP)
        for sw in ('LOAD', 'RDCF', 'CONF', 'PREP', 'SATR'):
            btn = button(swF0, LEFT, sw, lambda w=display, c=sw: w.set(c))
            btn.bind('<ButtonRelease-1>', lambda s=self, w=display, c=sw: app.Dispatcher2(c), '+')

        swF1 = frame(self, TOP)
        for sw in ('UNLD', 'UNCF', 'SPTR', 'STOP', 'EXIT'):
            btn = button(swF1, LEFT, sw, lambda w=display, c=sw: w.set(c))
            btn.bind('<ButtonRelease-1>', lambda s=self, w=display, c=sw: app.Dispatcher2(c), '+')

        swF2 = frame(self, TOP)
        btn1 = button(swF2, LEFT, 'cfgFile', lambda w=cfgfilename: w.set(askopenfilename(initialdir = '.')))
        btn2 = button(swF2, LEFT, 'viewChannel', lambda : hist[0][0].Fill(1000))
        btn3 = button(swF2, LEFT, 'hideChannel', lambda : hist[0][0].Reset())

    #end of def
     
#end of class

def plot_graph():
    global C0
    global C1
    # hist is filled in different thread and Pad could not receive the modified signal
    for idx in (0,1,2,3,4,5,6,7):
        C0.cd(idx+1).Modified()
        C0.cd(idx+1).Update()
        C1.cd(idx+1).Modified()
        C1.cd(idx+1).Update()
    global t        #Notice: use global variable!
    t = threading.Timer(0.1, plot_graph)
    t.start()

#    bins = [i for i in range(0, 4095)]
#    x = 1000*pl.randn(10000)
#    plt.close('all')
#    F0, axarr0 = plt.subplots(2, 4, sharex=True)
#    F1, axarr1 = plt.subplots(2, 4, sharex=True)
#    for idx0 in (0,1):
#        for idx1 in (0,1,2,3):
#            axarr0[idx0][idx1].hist(x, 100, histtype='step')
#            axarr1[idx0][idx1].hist(x, 100, histtype='step')
#    plt.show()

def fillhist(Range, Ch, Value) :
    print "fillihist call"
    global hist
    hist[Range][Ch].Fill(Value)
    return 1

if __name__ == '__main__':
    #plt.ion()
    app=cdll.LoadLibrary("./_app_start.so")
    app.app_start()

    C0 = ROOT.TCanvas("High", "High", 800, 600)
    C0.Divide(2,4)
    C1 = ROOT.TCanvas("Low", "Low", 800, 600)
    C1.Divide(2,4)
    hist = [[0 for i in range(8)] for j in range(2)]
    for idx0 in (0,1): 
        for idx1 in (0,1,2,3,4,5,6,7):
            if idx0==0 :
                C0.cd(idx1+1)
            else :
                C1.cd(idx1+1)
            hist[idx0][idx1] = ROOT.TH1F(str(idx0*8+idx1), str(idx0*8+idx1), 4098, -1.5, 4096.5)
            #hist[idx0][idx1].Fill((idx0*8+idx1+1)*100)
            hist[idx0][idx1].Draw()

    #p = Process(target=plot_graph, args=())
    #p.start()
    t = threading.Timer(0.1, plot_graph)
    t.start()

    CFunc=CFUNCTYPE(c_int, c_int, c_int, c_int)
    pFillHist=CFunc(fillhist)
    app.setFillHist(pFillHist)

    vmeGUI().mainloop()
    #p.join()
