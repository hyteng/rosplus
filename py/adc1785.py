#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# generated by wxGlade 0.7.0 on Sat May  9 21:24:17 2015
#

import wx
import struct
import ROOT as rt
#import matplotlib.pyplot as plt
#import numpy as np

# begin wxGlade: dependencies
import gettext
# end wxGlade

# begin wxGlade: extracode
# end wxGlade


class devFrame(wx.Frame):
    def __init__(self, cSocket, *args, **kwds):
        # begin wxGlade: devFrame.__init__
        style = wx.MINIMIZE_BOX|wx.MAXIMIZE_BOX|wx.RESIZE_BORDER
        wx.Frame.__init__(self, *args, **kwds)
        self.output0 = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.HSCROLL | wx.TE_MULTILINE | wx.TE_READONLY)
        self.label_crateSel = wx.StaticText(self, wx.ID_ANY, _("crateSel"), style=wx.ALIGN_CENTER)
        self.crateSel = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.stepTh = wx.ToggleButton(self, wx.ID_ANY, _("stepTh X16"))
        self.clearData = wx.Button(self, wx.ID_ANY, _("clearData"))
        self.tmp0 = wx.StaticText(self, wx.ID_ANY, "")
        self.tmp1 = wx.StaticText(self, wx.ID_ANY, "")
        self.label_eventCount = wx.StaticText(self, wx.ID_ANY, _("event Count"), style=wx.ALIGN_RIGHT)
        self.eventCount = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.TE_CENTRE | wx.TE_READONLY)
        self.countReset = wx.Button(self, wx.ID_ANY, _("count Reset"))
        self.label_fclrw = wx.StaticText(self, wx.ID_ANY, _("fclrw"), style=wx.ALIGN_CENTER)
        self.fclrw = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=1024)
        self.offline = wx.ToggleButton(self, wx.ID_ANY, _("offline"))
        self.swComm = wx.Button(self, wx.ID_ANY, _("swComm"))
        self.tmp2 = wx.StaticText(self, wx.ID_ANY, "")
        self.tmp3 = wx.StaticText(self, wx.ID_ANY, "")
        self.tmp4 = wx.StaticText(self, wx.ID_ANY, "")
        self.buffFull = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.TE_CENTRE | wx.TE_READONLY)
        self.softReset = wx.Button(self, wx.ID_ANY, _("soft Reset"))
        self.ch0L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch0L_En"))
        self.threshold0L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch1L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch1L_En"))
        self.threshold1L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch2L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch2L_En"))
        self.threshold2L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch3L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch3L_En"))
        self.threshold3L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch4L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch4L_En"))
        self.threshold4L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch5L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch5L_En"))
        self.threshold5L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch6L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch6L_En"))
        self.threshold6L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch7L_En = wx.ToggleButton(self, wx.ID_ANY, _("ch7L_En"))
        self.threshold7L = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch0H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch0H_En"))
        self.threshold0H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch1H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch1H_En"))
        self.threshold1H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch2H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch2H_En"))
        self.threshold2H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch3H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch3H_En"))
        self.threshold3H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch4H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch4H_En"))
        self.threshold4H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch5H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch5H_En"))
        self.threshold5H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch6H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch6H_En"))
        self.threshold6H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.ch7H_En = wx.ToggleButton(self, wx.ID_ANY, _("ch7H_En"))
        self.threshold7H = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)

        #self.crateSel.SetBase(16)
        #self.fclrw.SetBase(16)
        #self.threshold0L.SetBase(16)
        #self.threshold1L.SetBase(16)
        #self.threshold2L.SetBase(16)
        #self.threshold3L.SetBase(16)
        #self.threshold4L.SetBase(16)
        #self.threshold5L.SetBase(16)
        #self.threshold6L.SetBase(16)
        #self.threshold7L.SetBase(16)
        #self.threshold0H.SetBase(16)
        #self.threshold1H.SetBase(16)
        #self.threshold2H.SetBase(16)
        #self.threshold3H.SetBase(16)
        #self.threshold4H.SetBase(16)
        #self.threshold5H.SetBase(16)
        #self.threshold6H.SetBase(16)
        #self.threshold7H.SetBase(16)

        self.ch0_L = wx.ToggleButton(self, wx.ID_ANY, _("ch0_L"))
        self.ch1_L = wx.ToggleButton(self, wx.ID_ANY, _("ch1_L"))
        self.ch2_L = wx.ToggleButton(self, wx.ID_ANY, _("ch2_L"))
        self.ch3_L = wx.ToggleButton(self, wx.ID_ANY, _("ch3_L"))
        self.ch4_L = wx.ToggleButton(self, wx.ID_ANY, _("ch4_L"))
        self.ch5_L = wx.ToggleButton(self, wx.ID_ANY, _("ch5_L"))
        self.ch6_L = wx.ToggleButton(self, wx.ID_ANY, _("ch6_L"))
        self.ch7_L = wx.ToggleButton(self, wx.ID_ANY, _("ch7_L"))
        self.ch0_H = wx.ToggleButton(self, wx.ID_ANY, _("ch0_H"))
        self.ch1_H = wx.ToggleButton(self, wx.ID_ANY, _("ch1_H"))
        self.ch2_H = wx.ToggleButton(self, wx.ID_ANY, _("ch2_H"))
        self.ch3_H = wx.ToggleButton(self, wx.ID_ANY, _("ch3_H"))
        self.ch4_H = wx.ToggleButton(self, wx.ID_ANY, _("ch4_H"))
        self.ch5_H = wx.ToggleButton(self, wx.ID_ANY, _("ch5_H"))
        self.ch6_H = wx.ToggleButton(self, wx.ID_ANY, _("ch6_H"))
        self.ch7_H = wx.ToggleButton(self, wx.ID_ANY, _("ch7_H"))
        self.C = [-1 for i in range(16)]
        self.dev = -1
        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_SPINCTRL, self.setCrateSel, self.crateSel)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.setStepTh, self.stepTh)
        self.Bind(wx.EVT_BUTTON, self.clearBuff, self.clearData)
        self.Bind(wx.EVT_BUTTON, self.resetCount, self.countReset)
        self.Bind(wx.EVT_SPINCTRL, self.setFclrw, self.fclrw)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.setOffLine, self.offline)
        self.Bind(wx.EVT_BUTTON, self.comm, self.swComm)
        self.Bind(wx.EVT_BUTTON, self.reset, self.softReset)

        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=0 :self.showCh(event, n), self.ch0_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=1 :self.showCh(event, n), self.ch1_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=2 :self.showCh(event, n), self.ch2_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=3 :self.showCh(event, n), self.ch3_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=4 :self.showCh(event, n), self.ch4_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=5 :self.showCh(event, n), self.ch5_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=6 :self.showCh(event, n), self.ch6_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=7 :self.showCh(event, n), self.ch7_L)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=8 :self.showCh(event, n), self.ch0_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=9 :self.showCh(event, n), self.ch1_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=10 :self.showCh(event, n), self.ch2_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=11 :self.showCh(event, n), self.ch3_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=12 :self.showCh(event, n), self.ch4_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=13 :self.showCh(event, n), self.ch5_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=14 :self.showCh(event, n), self.ch6_H)
        self.Bind(wx.EVT_TOGGLEBUTTON, lambda event, n=15 :self.showCh(event, n), self.ch7_H)
        # end wxGlade
        self.Bind(wx.EVT_CLOSE, self.on_close)
        # user ctrl 
        self.ctrl = cSocket

    def on_close(self, event):
        self.Hide()

    def __set_properties(self):
        # begin wxGlade: devFrame.__set_properties
        self.SetTitle(_("adc1785"))
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: devFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_1 = wx.GridSizer(2, 8, 1, 1)
        grid_sizer_2 = wx.GridSizer(2, 8, 0, 0)
        grid_sizer_2F = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_2E = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_2D = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_2C = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_2B = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_2A = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_29 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_28 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_27 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_26 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_25 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_24 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_23 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_22 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_21 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_20 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_3 = wx.GridSizer(2, 8, 1, 1)
        grid_sizer_31 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_30 = wx.GridSizer(1, 2, 0, 0)
        sizer_1.Add(self.output0, 2, wx.EXPAND, 0)

        grid_sizer_30.Add(self.label_crateSel, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_30.Add(self.crateSel, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(grid_sizer_30, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(self.stepTh, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(self.clearData, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(self.tmp0, 0, wx.EXPAND, 0)
        grid_sizer_3.Add(self.tmp1, 0, wx.EXPAND, 0)
        grid_sizer_3.Add(self.label_eventCount, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(self.eventCount, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(self.countReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_31.Add(self.label_fclrw, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_31.Add(self.fclrw, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(grid_sizer_31, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(self.offline, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(self.swComm, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(self.tmp2, 0, wx.EXPAND, 0)
        grid_sizer_3.Add(self.tmp3, 0, wx.EXPAND, 0)
        grid_sizer_3.Add(self.tmp4, 0, wx.EXPAND, 0)
        grid_sizer_3.Add(self.buffFull, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3.Add(self.softReset, 0, wx.ALL | wx.EXPAND, 1)
        sizer_1.Add(grid_sizer_3, 1, wx.ALL | wx.EXPAND, 0)

        grid_sizer_20.Add(self.ch0L_En, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_20.Add(self.threshold0L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_20, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_21.Add(self.ch1L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_21.Add(self.threshold1L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_21, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_22.Add(self.ch2L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_22.Add(self.threshold2L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_22, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_23.Add(self.ch3L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_23.Add(self.threshold3L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_23, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_24.Add(self.ch4L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_24.Add(self.threshold4L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_24, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_25.Add(self.ch5L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_25.Add(self.threshold5L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_25, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_26.Add(self.ch6L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_26.Add(self.threshold6L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_26, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_27.Add(self.ch7L_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_27.Add(self.threshold7L, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_27, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_28.Add(self.ch0H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_28.Add(self.threshold0H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_28, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_29.Add(self.ch1H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_29.Add(self.threshold1H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_29, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2A.Add(self.ch2H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2A.Add(self.threshold2H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_2A, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2B.Add(self.ch3H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2B.Add(self.threshold3H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_2B, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2C.Add(self.ch4H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2C.Add(self.threshold4H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_2C, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2D.Add(self.ch5H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2D.Add(self.threshold5H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_2D, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2E.Add(self.ch6H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2E.Add(self.threshold6H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_2E, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2F.Add(self.ch7H_En, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2F.Add(self.threshold7H, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.Add(grid_sizer_2F, 1, wx.ALL | wx.EXPAND, 0)
        sizer_1.Add(grid_sizer_2, 1, wx.ALL | wx.EXPAND, 0)

        grid_sizer_1.Add(self.ch0_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch1_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch2_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch3_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch4_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch5_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch6_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch7_L, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch0_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch1_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch2_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch3_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch4_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch5_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch6_H, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.ch7_H, 1, wx.ALL | wx.EXPAND, 1)
        sizer_1.Add(grid_sizer_1, 1, wx.ALL | wx.EXPAND, 1)

        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        # end wxGlade

    def setCrateSel(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setCrateSel' !"
        spin = event.GetEventObject()
        v = spin.GetValue()
        vs = '%x' %v
        cs = "ctrl#"+self.dev.name+"#crateSel;w;"+vs+";"
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def setStepTh(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setStepTh' !"
        tb = event.GetEventObject()
        st = tb.GetValue()
        cs = "ctrl#"+self.dev.name+"#stepTh;w;"
        if st==0 :
            cs += "0;"
            tb.SetLabel("stepTh X16")
        else :
            cs += "1;"
            tb.SetLabel("stepTh X2")
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def clearBuff(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'clearBuff' !"
        cs = "ctrl#"+self.dev.name+"#clearData;w;1;"
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def resetCount(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'resetCount' !"
        cs = "ctrl#"+self.dev.name+"#countReset;w;1"
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def setFclrw(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setFclrw' !"
        spin = event.GetEventObject()
        v = spin.GetValue()
        vs = '%x' %v
        cs = "ctrl#"+self.dev.name+"#fclrw;w;"+vs+";"
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def setOffLine(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setOffLine' !"
        tb = event.GetEventObject()
        st = tb.GetValue()
        sts = '%d' %st
        cs = "ctrl#"+self.dev.name+"#offline;w;"+sts+";";
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def comm(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'swComm' !"
        cs = "ctrl#"+self.dev.name+"#swComm;w;1"
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def reset(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'softReset' !"
        cs = "ctrl#"+self.dev.name+"#ssReset;w;1;"
        self.ctrl.sendCtrl(cs.encode('utf8'))
        event.Skip()

    def showCh(self, event, n):  # wxGlade: devFrame.<event_handler>
        print "Event handler showCh for button%d\n"%(n)
        idx0 = n/8
        idx1 = n%8
        b = event.GetEventObject()
        st = b.GetValue()
        if st==0 :
            #plt.figure(n).show()
            if self.C[n] != -1 :
                self.C[n].Close()
                self.C[n] = -1
        else :
            #plt.figure(n)
            #plt.show()
            self.C[n] = rt.TCanvas("ch"+str(n), "ch"+str(n), 800, 600)
            self.C[n].cd()
            if self.dev!=-1 :
                self.dev.hist[idx0][idx1].Draw()
                self.C[n].Modified()
                self.C[n].Update()


        event.Skip()

    def setDev(self, d):
        self.dev = d
        self.SetTitle(_(d.name))
        #for i in range(16) :
            #self.C[i] = plt.figure(i)
            #self.C[i].add_subplot(111).plot()
            #plt.close()
            #hist(self.dev.hist[idx0][idx1], bins=4098, range=(-1.5, 4096.5), normed=False, weights=None, cumulative=False, bottom=None, histtype='step', align='mid', orientation='vertical', rwidth=None, log=False, color=None, label=None, stacked=False, hold=None)

# end of class devFrame


class devApp:
    #def OnInit(self):
    def __init__(self, n):
        #wx.InitAllImageHandlers()
        #dev0 = devFrame(None, wx.ID_ANY, "")
        #self.SetTopWindow(dev0)
        self.name = n;
        self.callTimer = True
        self.frame = -1
        self.th = [[0 for i in range(8)] for j in range(2)]
        self.hist = [[0 for i in range(8)] for j in range(2)]
        for idx0 in (0,1) :
            for idx1 in (0,1,2,3,4,5,6,7) :
                print "%s"%(str(idx0*8+idx1))
                self.hist[idx0][idx1] = rt.TH1F(str(idx0*8+idx1), str(idx0*8+idx1), 4098, -1.5, 4096.5)
                #self.hist[idx0][idx1] = []
        
        #dev0.Show()
    
    def setFrame(self, f):
        self.frame = f

    def fillEvent(self, event):
        print "filling event size %d, %s"%(len(event),event)
        num = len(event)/4
        for idx in range(num) :
            data = event[idx*4:(idx+1)*4]
            print "data word: %s"%(data)
            #if (idx>0) and (idx<num-1) :
            if idx>=0 :
                value = struct.unpack(">I", data)
                print "value: %x"%(value[0])
                ch = (value[0]&0x001C0000)>>18
                rg = (value[0]&0x00020000)>>17
                un = (value[0]&0x00002000)>>13
                ov = (value[0]&0x00001000)>>12
                adc = (value[0]&0x00000FFF)
                n = (1-rg)*8+ch
                print "channel %d: %d"%(n, adc)
                self.fillCh(n, adc)
                continue
            #if idx==0 :
                #continue
            #if idx==num-1 :
                #continue
        print "filling event done"
        
    def fillCh(self, n, v):
        idx0 = n/8
        idx1 = n%8
        self.hist[idx0][idx1].Fill(v)
        #self.hist[idx0][idx1].append(v)
        if self.frame!=-1 :
            if (self.frame.C[n]!=-1) and (self.frame.C[n]!=None) :
                print "fill and update hist%d"%(n)
                self.frame.C[n].Modified()
                self.frame.C[n].Update()
                #plt.plot("hist", self.hist[idx0][idx1])
                #self.frame.C[n].show()


    def clearCh(self, n, v):
        idx0 = n/8
        idx1 = n%8
        self.hist[idx0][idx1].Clear()
        #self.hist[idx0][idx1].clear()
        if self.frame!=-1 :
            if (self.frame.C[n]!=-1) and (self.frame.C[n]!=None) :
                print "fill and update hist%d\n"%(n)
                self.frame.C[n].Modified()
                self.frame.C[n].Update()
                #self.frame.C[n].draw()
                #plt.plot("hist", self.hist[idx0][idx1])

    def ctrlHandler(self, control, ret):
        if control=="ctrl" :
            idx = ret.find(';')
            ctrlName = ret[:idx]
            idx = ret.find(';', idx+1)
            rw = ret[len(ctrlName)+1:idx]
            value = ret[idx+1:-1]
            print "ctrlHandler for adc1785 %s" %(ctrlName) 
            obj = getattr(self.frame, ctrlName)
            if obj!=None :
                #if obj.ctrlHander!=None :
                    #obj.ctrlHander(rw, value)
                print "control acknowledge from %s with value %s" %(ctrlName, value)

    def timerHandler(self):
        #print ""
        print "timerHandler for adc1785"


# end of class devApp

if __name__ == "__main__":
    gettext.install("dev0") # replace with the appropriate catalog name

    dev0 = devApp(0)
    dev0.MainLoop()
