#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# generated by wxGlade not found on Tue Jun 30 11:04:44 2015
#

import wx

# begin wxGlade: dependencies
import gettext
# end wxGlade

# begin wxGlade: extracode
# end wxGlade


class ctrlFrame(wx.Frame):
    def __init__(self, *args, **kwds):
        # begin wxGlade: ctrlFrame.__init__
        wx.Frame.__init__(self, *args, **kwds)
        self.bitmap_button_1 = wx.BitmapButton(self, wx.ID_ANY, wx.Bitmap("/home/hyteng/Downloads/adc1785_button.png", wx.BITMAP_TYPE_ANY))
        self.output0 = wx.TextCtrl(self, wx.ID_ANY, _("ctrlMsg\n"), style=wx.HSCROLL | wx.TE_BESTWRAP | wx.TE_MULTILINE | wx.TE_READONLY)
        self.input0 = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.HSCROLL | wx.TE_BESTWRAP | wx.TE_MULTILINE | wx.TE_PROCESS_ENTER)
        self.button_Load = wx.Button(self, wx.ID_ANY, _("Load"))
        self.button_Conf = wx.Button(self, wx.ID_ANY, _("Conf"))
        self.button_Prep = wx.Button(self, wx.ID_ANY, _("Prep"))
        self.button_Star = wx.Button(self, wx.ID_ANY, _("Star"))
        self.button_Paus = wx.Button(self, wx.ID_ANY, _("Paus"))
        self.button_unLoad = wx.Button(self, wx.ID_ANY, _("unLoad"))
        self.button_unConf = wx.Button(self, wx.ID_ANY, _("unConf"))
        self.button_End = wx.Button(self, wx.ID_ANY, _("End"))
        self.button_Stop = wx.Button(self, wx.ID_ANY, _("Stop"))
        self.button_Resu = wx.Button(self, wx.ID_ANY, _("Resu"))

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_BUTTON, self.selectDev, self.bitmap_button_1)
        self.Bind(wx.EVT_TEXT_ENTER, self.sendMsg, self.input0)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Load)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Conf)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Prep)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Star)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Paus)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_unLoad)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_unConf)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_End)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Stop)
        self.Bind(wx.EVT_BUTTON, self.sendCmd, self.button_Resu)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: ctrlFrame.__set_properties
        self.SetTitle(_("daq ctrl"))
        self.bitmap_button_1.SetSize(self.bitmap_button_1.GetBestSize())
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: ctrlFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_1 = wx.GridSizer(2, 5, 0, 0)
        grid_sizer_2 = wx.FlexGridSizer(1, 3, 0, 0)
        grid_sizer_2.Add(self.bitmap_button_1, 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.AddGrowableRow(0)
        sizer_1.Add(grid_sizer_2, 5, wx.ALL, 0)
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
        grid_sizer_1.Add(self.button_Resu, 1, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        sizer_1.Add(grid_sizer_1, 0, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 0)
        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        # end wxGlade

    def selectDev(self, event):  # wxGlade: ctrlFrame.<event_handler>
        print "Event handler 'selectDev' not implemented!"
        event.Skip()

    def sendMsg(self, event):  # wxGlade: ctrlFrame.<event_handler>
        print "Event handler 'sendMsg' not implemented!"
        event.Skip()

    def sendCmd(self, event):  # wxGlade: ctrlFrame.<event_handler>
        print "Event handler 'sendCmd' not implemented!"
        event.Skip()

# end of class ctrlFrame
class ctrlApp(wx.App):
    def OnInit(self):
        wx.InitAllImageHandlers()
        frame0 = ctrlFrame(None, wx.ID_ANY, "")
        self.SetTopWindow(frame0)
        frame0.Show()
        return 1

# end of class ctrlApp

if __name__ == "__main__":
    gettext.install("ctrl0") # replace with the appropriate catalog name

    ctrl0 = ctrlApp(0)
    ctrl0.MainLoop()