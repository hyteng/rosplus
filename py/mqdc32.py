#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# generated by wxGlade not found on Mon Sep 21 05:48:36 2015
#

import wx
import ROOT as rt
import time

# begin wxGlade: dependencies
import gettext
# end wxGlade

# begin wxGlade: extracode
# end wxGlade


class devFrame(wx.Frame):
    def __init__(self, cSocket, *args, **kwds):
        # begin wxGlade: devFrame.__init__
        wx.Frame.__init__(self, *args, **kwds)
        self.output0 = wx.TextCtrl(self, wx.ID_ANY, "", style=wx.HSCROLL | wx.TE_MULTILINE | wx.TE_READONLY)
        self.BLR = wx.ToggleButton(self, wx.ID_ANY, _("BLR"))
        self.inputCouple0 = wx.ToggleButton(self, wx.ID_ANY, _("couple0AC"))
        self.inputCouple1 = wx.ToggleButton(self, wx.ID_ANY, _("couple1AC"))
        self.terGate0 = wx.ToggleButton(self, wx.ID_ANY, _("terGate0"))
        self.terGate1 = wx.ToggleButton(self, wx.ID_ANY, _("terGate1"))
        self.eclGate1Osc = wx.ToggleButton(self, wx.ID_ANY, _("eclGate1Osc"))
        self.nimGate1Osc = wx.ToggleButton(self, wx.ID_ANY, _("nimGate1Osc"))
        self.label_testPulserStatus = wx.StaticText(self, wx.ID_ANY, _("testPulserStatus"), style=wx.ALIGN_CENTER)
        self.pulserSt = wx.ComboBox(self, wx.ID_ANY, choices=[_("off"), _("amp=0"), _("gate100ns")], style=wx.CB_READONLY)
        self.tsSource = wx.ToggleButton(self, wx.ID_ANY, _("ts:VME"))
        self.extReset = wx.ToggleButton(self, wx.ID_ANY, _("etsReset Enable"))
        self.label_eventCounter = wx.StaticText(self, wx.ID_ANY, _("eventCounter"), style=wx.ALIGN_CENTER)
        self.eventCounter = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=999999, style=wx.TE_READONLY)
        self.stopCounterA = wx.ToggleButton(self, wx.ID_ANY, _("stopCounterA"))
        self.ctraReset = wx.Button(self, wx.ID_ANY, _("ctraReset"))
        self.label_gate1Time = wx.StaticText(self, wx.ID_ANY, _("gate1Time"), style=wx.ALIGN_CENTER)
        self.gate1Time = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=999999, style=wx.TE_READONLY)
        self.terReset = wx.ToggleButton(self, wx.ID_ANY, _("terReset"))
        self.gateSelect = wx.ToggleButton(self, wx.ID_ANY, _("gateSelect"))
        self.terBank0 = wx.ToggleButton(self, wx.ID_ANY, _("terBank0"))
        self.terBank1 = wx.ToggleButton(self, wx.ID_ANY, _("terBank1"))
        self.label_eclFCReset = wx.StaticText(self, wx.ID_ANY, _("eclFCReset"), style=wx.ALIGN_CENTER)
        self.eclFCReset = wx.ComboBox(self, wx.ID_ANY, choices=[_("fastClear"), _("timeStamp"), _("sync")], style=wx.CB_READONLY)
        self.label_nimFCReset = wx.StaticText(self, wx.ID_ANY, _("nimFCReset"), style=wx.ALIGN_CENTER)
        self.nimFCReset = wx.ComboBox(self, wx.ID_ANY, choices=[_("fastClear"), _("timeStamp"), _("sync")], style=wx.CB_READONLY)
        self.label_testPluserDAC = wx.StaticText(self, wx.ID_ANY, _("testPulserDAC"), style=wx.ALIGN_CENTER)
        self.pulserDac = wx.SpinCtrl(self, wx.ID_ANY, "32", min=0, max=255)
        self.label_tsDivisor = wx.StaticText(self, wx.ID_ANY, _("tsDiviSor"), style=wx.ALIGN_CENTER)
        self.tsDivisor = wx.SpinCtrl(self, wx.ID_ANY, "1", min=0, max=65535)
        self.label_tsCounter = wx.StaticText(self, wx.ID_ANY, _("tsCounter"), style=wx.ALIGN_CENTER)
        self.tsCounter = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=999999, style=wx.TE_READONLY)
        self.extReset = wx.ToggleButton(self, wx.ID_ANY, _("extReset"))
        self.panel_1 = wx.Panel(self, wx.ID_ANY)
        self.stopCounterB = wx.ToggleButton(self, wx.ID_ANY, _("stopCounterB"))
        self.ctrbReset = wx.Button(self, wx.ID_ANY, _("ctrbReset"))
        self.label_irqTh = wx.StaticText(self, wx.ID_ANY, _("irqTh"), style=wx.ALIGN_CENTER)
        self.irqTh = wx.SpinCtrl(self, wx.ID_ANY, "1", min=0, max=65535)
        self.label_limit0 = wx.StaticText(self, wx.ID_ANY, _("limit0"), style=wx.ALIGN_CENTER)
        self.limitBank0 = wx.SpinCtrl(self, wx.ID_ANY, "255", min=0, max=255)
        self.label_limit1 = wx.StaticText(self, wx.ID_ANY, _("limit1"), style=wx.ALIGN_CENTER)
        self.limitBank1 = wx.SpinCtrl(self, wx.ID_ANY, "255", min=0, max=255)
        self.label_cbltCtrl = wx.StaticText(self, wx.ID_ANY, _("cbltCtrl"), style=wx.ALIGN_CENTER)
        self.cbltCtrl = wx.ComboBox(self, wx.ID_ANY, choices=[_("noCBLT"), _("CBLT-first"), _("CBLT"), _("CBLT-last")], style=wx.CB_READONLY)
        self.label_cbltAddr = wx.StaticText(self, wx.ID_ANY, _("cbltAddr"), style=wx.ALIGN_CENTER)
        self.cbltAddr = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.bankConnect = wx.ToggleButton(self, wx.ID_ANY, _("bankConnect"))
        self.switchPull0 = wx.ToggleButton(self, wx.ID_ANY, _("Pull0"))
        self.switchPull1 = wx.ToggleButton(self, wx.ID_ANY, _("Pull1"))
        self.label_bank0HighLimit = wx.StaticText(self, wx.ID_ANY, _("bank0HighLimit"), style=wx.ALIGN_CENTER)
        self.limit0High = wx.SpinCtrl(self, wx.ID_ANY, "32", min=0, max=63)
        self.label_bank0LowLimit = wx.StaticText(self, wx.ID_ANY, _("bank0LowLimit"), style=wx.ALIGN_CENTER)
        self.limit0Low = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=63)
        self.label_multiEvent = wx.StaticText(self, wx.ID_ANY, _("multiEvent"), style=wx.ALIGN_CENTER)
        self.multiEvent = wx.SpinCtrl(self, wx.ID_ANY, "2", min=0, max=2)
        self.label_markType = wx.StaticText(self, wx.ID_ANY, _("markType"), style=wx.ALIGN_CENTER)
        self.markType = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=2)
        self.label_maxTransfer = wx.StaticText(self, wx.ID_ANY, _("maxTransfer"), style=wx.ALIGN_CENTER)
        self.maxTransfer = wx.SpinCtrl(self, wx.ID_ANY, "1", min=0, max=65535)
        self.label_delay0 = wx.StaticText(self, wx.ID_ANY, _("delay0"), style=wx.ALIGN_CENTER)
        self.expDelay0 = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=16383)
        self.label_delay1 = wx.StaticText(self, wx.ID_ANY, _("delay1"), style=wx.ALIGN_CENTER)
        self.expDelay1 = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=16383)
        self.label_mcstCtrl = wx.StaticText(self, wx.ID_ANY, _("mcstCtrl"), style=wx.ALIGN_CENTER)
        self.mcstCtrl = wx.ToggleButton(self, wx.ID_ANY, _("msctEnable"))
        self.label_mcstAddr = wx.StaticText(self, wx.ID_ANY, _("mcstAddr"), style=wx.ALIGN_CENTER)
        self.mcstAddr = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=255)
        self.slideScale = wx.ToggleButton(self, wx.ID_ANY, _("slideScale"))
        self.overFlow = wx.ToggleButton(self, wx.ID_ANY, _("overFlow"))
        self.label_bank1HighLimit = wx.StaticText(self, wx.ID_ANY, _("bank1HighLimit"), style=wx.ALIGN_CENTER)
        self.limit1High = wx.SpinCtrl(self, wx.ID_ANY, "16", min=0, max=31)
        self.label_bank1LowLimit = wx.StaticText(self, wx.ID_ANY, _("bank1LowLimit"), style=wx.ALIGN_CENTER)
        self.limit1Low = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=31)
        self.disableTh = wx.ToggleButton(self, wx.ID_ANY, _("disableTh"))
        self.startAcq = wx.ToggleButton(self, wx.ID_ANY, _("startACQ"))
        self.label_CH = wx.StaticText(self, wx.ID_ANY, _("Channel with Threshold"), style=wx.ALIGN_CENTER)
        self.ch00 = wx.ToggleButton(self, wx.ID_ANY, _("ch00"))
        self.Ch00Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch01 = wx.ToggleButton(self, wx.ID_ANY, _("ch01"))
        self.Ch01Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch02 = wx.ToggleButton(self, wx.ID_ANY, _("ch02"))
        self.Ch02Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch03 = wx.ToggleButton(self, wx.ID_ANY, _("ch03"))
        self.Ch03Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch04 = wx.ToggleButton(self, wx.ID_ANY, _("ch04"))
        self.Ch04Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch05 = wx.ToggleButton(self, wx.ID_ANY, _("ch05"))
        self.Ch05Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch06 = wx.ToggleButton(self, wx.ID_ANY, _("ch06"))
        self.Ch06Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch07 = wx.ToggleButton(self, wx.ID_ANY, _("ch07"))
        self.Ch07Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch08 = wx.ToggleButton(self, wx.ID_ANY, _("ch08"))
        self.Ch08Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch09 = wx.ToggleButton(self, wx.ID_ANY, _("ch09"))
        self.Ch09Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch10 = wx.ToggleButton(self, wx.ID_ANY, _("ch10"))
        self.Ch10Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch11 = wx.ToggleButton(self, wx.ID_ANY, _("ch11"))
        self.Ch11Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch12 = wx.ToggleButton(self, wx.ID_ANY, _("ch12"))
        self.Ch12Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch13 = wx.ToggleButton(self, wx.ID_ANY, _("ch13"))
        self.Ch13Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch14 = wx.ToggleButton(self, wx.ID_ANY, _("ch14"))
        self.Ch14Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch15 = wx.ToggleButton(self, wx.ID_ANY, _("ch15"))
        self.Ch15Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch16 = wx.ToggleButton(self, wx.ID_ANY, _("ch16"))
        self.Ch16Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch17 = wx.ToggleButton(self, wx.ID_ANY, _("ch17"))
        self.Ch17Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch18 = wx.ToggleButton(self, wx.ID_ANY, _("ch18"))
        self.Ch18Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch19 = wx.ToggleButton(self, wx.ID_ANY, _("ch19"))
        self.Ch19Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch20 = wx.ToggleButton(self, wx.ID_ANY, _("ch20"))
        self.Ch20Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.c21 = wx.ToggleButton(self, wx.ID_ANY, _("ch21"))
        self.Ch21Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch22 = wx.ToggleButton(self, wx.ID_ANY, _("ch22"))
        self.Ch22Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch23 = wx.ToggleButton(self, wx.ID_ANY, _("ch23"))
        self.Ch23Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch24 = wx.ToggleButton(self, wx.ID_ANY, _("ch24"))
        self.Ch24Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch25 = wx.ToggleButton(self, wx.ID_ANY, _("ch25"))
        self.Ch25Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch26 = wx.ToggleButton(self, wx.ID_ANY, _("ch26"))
        self.Ch26Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch27 = wx.ToggleButton(self, wx.ID_ANY, _("ch27"))
        self.Ch27Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch28 = wx.ToggleButton(self, wx.ID_ANY, _("ch28"))
        self.Ch28Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch29 = wx.ToggleButton(self, wx.ID_ANY, _("ch29"))
        self.Ch29Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch30 = wx.ToggleButton(self, wx.ID_ANY, _("ch30"))
        self.Ch30Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.ch31 = wx.ToggleButton(self, wx.ID_ANY, _("ch31"))
        self.Ch31Th = wx.SpinCtrl(self, wx.ID_ANY, "0", min=0, max=100)
        self.static_line_1 = wx.StaticLine(self, wx.ID_ANY, style=wx.EXPAND)

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_TOGGLEBUTTON, self.setTSSource, self.tsSource)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.setExtTSResetSW, self.extReset)
        self.Bind(wx.EVT_SPINCTRL, self.setTestPulserDAC, self.pulserDac)
        self.Bind(wx.EVT_SPINCTRL, self.setIrqTh, self.irqTh)
        self.Bind(wx.EVT_SPINCTRL, self.setMaxTransfer, self.maxTransfer)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.mcstControl, self.mcstCtrl)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.disableThreshold, self.disableTh)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.runACQ, self.startAcq)
        # end wxGlade
        self.Bind(wx.EVT_CLOSE, self.on_close)
        # user ctrl 
        self.ctrl = cSocket

    def on_close(self, event):
        self.Hide()

    def __set_properties(self):
        # begin wxGlade: devFrame.__set_properties
        self.SetTitle(_("mqdc0"))
        self.pulserSt.SetSelection(-1)
        self.eclFCReset.SetSelection(0)
        self.nimFCReset.SetSelection(-1)
        self.cbltCtrl.SetSelection(0)
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: devFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_1 = wx.GridSizer(2, 8, 0, 0)
        grid_sizer_1F = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_1E = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_1D = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_1C = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_1B = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_1A = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_19 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_18 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_17 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_16 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_15 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_14 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_13 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_12 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_11 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_10 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_2 = wx.GridSizer(2, 8, 0, 0)
        grid_sizer_2F = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_2E = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_2D = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_2C = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_2B = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_2A = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_29 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_28 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_27 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_26 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_25 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_24 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_23 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_22 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_21 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_20 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_3 = wx.GridSizer(2, 8, 1, 1)
        grid_sizer_3F = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_3E = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_3D = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_3C = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_3B = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_3A = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_39 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_391 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_390 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_38 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_37 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_371 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_370 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_36 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_35 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_34 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_340 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_33 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_32 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_31 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_311 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_310 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_30 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4 = wx.GridSizer(2, 8, 0, 0)
        grid_sizer_4F = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4E = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4D = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4C = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4B = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4A = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_4A1 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_4A0 = wx.GridSizer(1, 2, 0, 0)
        grid_sizer_49 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_48 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_47 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_46 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_45 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_44 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_43 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_42 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_41 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_40 = wx.GridSizer(2, 1, 0, 0)
        grid_sizer_400 = wx.GridSizer(1, 2, 0, 0)
        sizer_1.Add(self.output0, 2, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_40.Add(self.BLR, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_400.Add(self.inputCouple0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_400.Add(self.inputCouple1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_40.Add(grid_sizer_400, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_40, 1, 0, 1)
        grid_sizer_41.Add(self.terGate0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_41.Add(self.terGate1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_41, 1, 0, 1)
        grid_sizer_42.Add(self.eclGate1Osc, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_42.Add(self.nimGate1Osc, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_42, 1, 0, 1)
        grid_sizer_43.Add(self.label_testPulserStatus, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_43.Add(self.pulserSt, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_43, 1, 0, 0)
        grid_sizer_44.Add(self.tsSource, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_44.Add(self.extReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_44, 1, 0, 0)
        grid_sizer_45.Add(self.label_eventCounter, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_45.Add(self.eventCounter, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_45, 1, 0, 0)
        grid_sizer_46.Add(self.stopCounterA, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_46.Add(self.ctraReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_46, 1, 0, 0)
        grid_sizer_47.Add(self.label_gate1Time, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_47.Add(self.gate1Time, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_47, 1, 0, 0)
        grid_sizer_48.Add(self.terReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_48.Add(self.gateSelect, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_48, 1, 0, 1)
        grid_sizer_49.Add(self.terBank0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_49.Add(self.terBank1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_49, 1, 0, 1)
        grid_sizer_4A0.Add(self.label_eclFCReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4A0.Add(self.eclFCReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4A.Add(grid_sizer_4A0, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4A1.Add(self.label_nimFCReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4A1.Add(self.nimFCReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4A.Add(grid_sizer_4A1, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_4A, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4B.Add(self.label_testPluserDAC, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4B.Add(self.pulserDac, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_4B, 1, 0, 0)
        grid_sizer_4C.Add(self.label_tsDivisor, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4C.Add(self.tsDivisor, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_4C, 1, 0, 0)
        grid_sizer_4D.Add(self.label_tsCounter, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4D.Add(self.tsCounter, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_4D, 1, 0, 0)
        grid_sizer_4E.Add(self.extReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4E.Add(self.panel_1, 1, 0, 0)
        grid_sizer_4.Add(grid_sizer_4E, 1, 0, 0)
        grid_sizer_4F.Add(self.stopCounterB, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4F.Add(self.ctrbReset, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_4.Add(grid_sizer_4F, 1, 0, 0)
        sizer_1.Add(grid_sizer_4, 2, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 4)
        grid_sizer_30.Add(self.label_irqTh, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_30.Add(self.irqTh, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_30, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_310.Add(self.label_limit0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_310.Add(self.limitBank0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_31.Add(grid_sizer_310, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_311.Add(self.label_limit1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_311.Add(self.limitBank1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_31.Add(grid_sizer_311, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_31, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_32.Add(self.label_cbltCtrl, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_32.Add(self.cbltCtrl, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_32, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_33.Add(self.label_cbltAddr, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_33.Add(self.cbltAddr, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_33, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_34.Add(self.bankConnect, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_340.Add(self.switchPull0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_340.Add(self.switchPull1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_34.Add(grid_sizer_340, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_34, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_35.Add(self.label_bank0HighLimit, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_35.Add(self.limit0High, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_35, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_36.Add(self.label_bank0LowLimit, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_36.Add(self.limit0Low, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_36, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_370.Add(self.label_multiEvent, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_370.Add(self.multiEvent, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_37.Add(grid_sizer_370, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_371.Add(self.label_markType, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_371.Add(self.markType, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_37.Add(grid_sizer_371, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_37, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_38.Add(self.label_maxTransfer, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_38.Add(self.maxTransfer, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_38, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_390.Add(self.label_delay0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_390.Add(self.expDelay0, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_39.Add(grid_sizer_390, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_391.Add(self.label_delay1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_391.Add(self.expDelay1, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_39.Add(grid_sizer_391, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_39, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3A.Add(self.label_mcstCtrl, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3A.Add(self.mcstCtrl, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_3A, 1, wx.ALL | wx.EXPAND, 0)
        grid_sizer_3B.Add(self.label_mcstAddr, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3B.Add(self.mcstAddr, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_3B, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3C.Add(self.slideScale, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3C.Add(self.overFlow, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_3C, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3D.Add(self.label_bank1HighLimit, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3D.Add(self.limit1High, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_3D, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3E.Add(self.label_bank1LowLimit, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3E.Add(self.limit1Low, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_3E, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3F.Add(self.disableTh, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3F.Add(self.startAcq, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_3.Add(grid_sizer_3F, 1, wx.ALL | wx.EXPAND, 1)
        sizer_1.Add(grid_sizer_3, 2, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        sizer_1.Add(self.label_CH, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL | wx.EXPAND, 1)
        grid_sizer_20.Add(self.ch00, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_20.Add(self.Ch00Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_20, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_21.Add(self.ch01, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_21.Add(self.Ch01Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_21, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_22.Add(self.ch02, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_22.Add(self.Ch02Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_22, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_23.Add(self.ch03, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_23.Add(self.Ch03Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_23, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_24.Add(self.ch04, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_24.Add(self.Ch04Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_24, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_25.Add(self.ch05, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_25.Add(self.Ch05Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_25, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_26.Add(self.ch06, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_26.Add(self.Ch06Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_26, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_27.Add(self.ch07, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_27.Add(self.Ch07Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_27, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_28.Add(self.ch08, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_28.Add(self.Ch08Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_28, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_29.Add(self.ch09, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_29.Add(self.Ch09Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_29, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2A.Add(self.ch10, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2A.Add(self.Ch10Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_2A, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2B.Add(self.ch11, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2B.Add(self.Ch11Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_2B, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2C.Add(self.ch12, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2C.Add(self.Ch12Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_2C, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2D.Add(self.ch13, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2D.Add(self.Ch13Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_2D, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2E.Add(self.ch14, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2E.Add(self.Ch14Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_2E, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2F.Add(self.ch15, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2F.Add(self.Ch15Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_2.Add(grid_sizer_2F, 1, wx.ALL | wx.EXPAND, 1)
        sizer_1.Add(grid_sizer_2, 2, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        grid_sizer_10.Add(self.ch16, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_10.Add(self.Ch16Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_10, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_11.Add(self.ch17, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_11.Add(self.Ch17Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_11, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_12.Add(self.ch18, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_12.Add(self.Ch18Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_12, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_13.Add(self.ch19, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_13.Add(self.Ch19Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_13, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_14.Add(self.ch20, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_14.Add(self.Ch20Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_14, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_15.Add(self.c21, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_15.Add(self.Ch21Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_15, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_16.Add(self.ch22, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_16.Add(self.Ch22Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_16, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_17.Add(self.ch23, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_17.Add(self.Ch23Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_17, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_18.Add(self.ch24, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_18.Add(self.Ch24Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_18, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_19.Add(self.ch25, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_19.Add(self.Ch25Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_19, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1A.Add(self.ch26, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1A.Add(self.Ch26Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_1A, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1B.Add(self.ch27, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1B.Add(self.Ch27Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_1B, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1C.Add(self.ch28, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1C.Add(self.Ch28Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_1C, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1D.Add(self.ch29, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1D.Add(self.Ch29Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_1D, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1E.Add(self.ch30, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1E.Add(self.Ch30Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_1E, 1, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1F.Add(self.ch31, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1F.Add(self.Ch31Th, 0, wx.ALL | wx.EXPAND, 1)
        grid_sizer_1.Add(grid_sizer_1F, 1, wx.ALL | wx.EXPAND, 1)
        sizer_1.Add(grid_sizer_1, 2, wx.ALL | wx.EXPAND | wx.FIXED_MINSIZE, 1)
        sizer_1.Add(self.static_line_1, 0, wx.ALL | wx.EXPAND, 0)
        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        # end wxGlade

    def setTSSource(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setTSSource' not implemented!"
        event.Skip()

    def setExtTSResetSW(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setExtTSResetSW' not implemented!"
        event.Skip()

    def setTestPulserDAC(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setTestPulserDAC' not implemented!"
        event.Skip()

    def setIrqTh(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setIrqTh' not implemented!"
        event.Skip()

    def setMaxTransfer(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'setMaxTransfer' not implemented!"
        event.Skip()

    def mcstControl(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'mcstControl' not implemented!"
        event.Skip()

    def disableThreshold(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'disableThreshold' not implemented!"
        event.Skip()

    def runACQ(self, event):  # wxGlade: devFrame.<event_handler>
        print "Event handler 'runACQ' not implemented!"
        event.Skip()

    def setDev(self, d):
        self.dev = d
        self.SetTitle(_(d.name))

# end of class devFrame
class devApp:#(wx.App):
    #def OnInit(self):
    def __init__(self, n):   
        #wx.InitAllImageHandlers()
        #mqdc = devFrame(None, wx.ID_ANY, "")
        #self.SetTopWindow(mqdc)
        #mqdc.Show()
        self.name = n;
        self.callTimer = True
        self.frame = -1
        self.stId = -1
        #return 1

    def setFrame(self, f):
        self.frame = f


    def ctrlHandler(self, control, ret):
        idx0 = ret.find(';')
        self.stId = ret[:idx0]
        while idx0!=-1:
            idx1 = ret.find(';', idx0+1)
            if idx1==-1 :
                break
            ctrlName = ret[idx0+1:idx1]
            idx2 = ret.find(';', idx1+1)
            if idx2==-1 :
                break
            rw = ret[idx1+1:idx2]
            idx3 = ret.find(';', idx2+1)
            if idx3==-1 :
                break
            value = [int(i) for i in ret[idx2+1:idx3].split(',')]
            print "ctrlHandler for %s: %s, %s, %d, %d, %d" %(self.name, ctrlName, rw, idx1, idx2, idx3)
            print value
            #time.sleep(1)
            if (ctrlName!="") and (hasattr(self.frame, ctrlName)) :
                obj = getattr(self.frame, ctrlName)
                if obj!=None :
                    objClass = obj.GetClassName()
                    print "ctrl object: %s" %(objClass)
                    if (objClass=="wxSpinCtrl") :
                        obj.SetValue(int(value[0]))
            idx0 = idx3

    def timerHandler(self):
        #print ""
        print "timerHandler for %s" %(self.name)

# end of class devMQDC32App

if __name__ == "__main__":
    gettext.install("devMQDC32") # replace with the appropriate catalog name

    devMQDC32 = devMQDC32App(0)
    devMQDC32.MainLoop()
