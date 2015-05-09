#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# generated by wxGlade 0.7.0 on Sat May  9 17:26:17 2015
#

import wx
import adc1785

# begin wxGlade: dependencies
import gettext
# end wxGlade

# begin wxGlade: extracode
# end wxGlade


class ctrlFrame(wx.Frame):
    def __init__(self, *args, **kwds):
        # begin wxGlade: ctrlFrame.__init__
        wx.Frame.__init__(self, *args, **kwds)
        self.devList = wx.Notebook(self, wx.ID_ANY)
        self.devPanel = wx.Panel(self.devList, wx.ID_ANY)
        self.output0 = wx.TextCtrl(self, wx.ID_ANY, _("ctrlMsg"), style=wx.HSCROLL | wx.TE_MULTILINE | wx.TE_READONLY)
        self.input0 = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.HSCROLL | wx.TE_MULTILINE | wx.TE_PROCESS_ENTER)
        self.button_Load = wx.Button(self, wx.ID_ANY, _("Load"))
        self.button_Conf = wx.Button(self, wx.ID_ANY, _("Conf"))
        self.button_Prep = wx.Button(self, wx.ID_ANY, _("Prep"))
        self.button_Star = wx.Button(self, wx.ID_ANY, _("Star"))
        self.button_Paus = wx.Button(self, wx.ID_ANY, _("Paus"))
        self.button_unLoad = wx.Button(self, wx.ID_ANY, _("unLoad"))
        self.button_unConf = wx.Button(self, wx.ID_ANY, _("unConf"))
        self.button_End = wx.Button(self, wx.ID_ANY, _("End"))
        self.button_Stop = wx.Button(self, wx.ID_ANY, _("Stop"))
        self.button_Exit = wx.Button(self, wx.ID_ANY, _("Exit"))

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_TEXT_ENTER, self.sendMsg, self.input0)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: ctrlFrame.__set_properties
        self.SetTitle(_("daq ctrl"))
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: ctrlFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_1 = wx.GridSizer(2, 5, 0, 0)
        self.devList.AddPage(self.devPanel, _("tab1"))
        sizer_1.Add(self.devList, 1, wx.ALL | wx.EXPAND, 0)
        sizer_1.Add(self.output0, 3, wx.ALL | wx.EXPAND, 1)
        sizer_1.Add(self.input0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(self.button_Load, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_Conf, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_Prep, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_Star, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_Paus, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_unLoad, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_unConf, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_End, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_Stop, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_1.Add(self.button_Exit, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        sizer_1.Add(grid_sizer_1, 0, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 0)
        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        # end wxGlade

    def sendMsg(self, event):  # wxGlade: ctrlFrame.<event_handler>
        print "Event handler 'sendMsg' not implemented!"
        event.Skip()

# end of class ctrlFrame
class ctrlApp(wx.App):
    def OnInit(self):
        wx.InitAllImageHandlers()
        frame0 = ctrlFrame(None, wx.ID_ANY, "")
        frame1 = adc1785.devFrame(None, wx.ID_ANY, "")
        dev1 = adc1785.devApp("adc1785")
        frame1.setDev(dev1)
        dev1.setFrame(frame1)
        self.SetTopWindow(frame0)
        frame0.Show()
        frame1.Show()
        return 1

# end of class ctrlApp

if __name__ == "__main__":
    gettext.install("ctrl0") # replace with the appropriate catalog name

    ctrl0 = ctrlApp(0)
    ctrl0.MainLoop()
