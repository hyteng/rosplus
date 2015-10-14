#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#
# generated by wxGlade 0.7.0 on Sat May  9 17:26:17 2015
#

import wx
import os
import threading
import time
from socket import *

#import adc1785

# begin wxGlade: dependencies
import gettext
# end wxGlade

# begin wxGlade: extracode
# end wxGlade

HOST='127.0.0.1'
MSGPORT=4000
DATAPORT=4001
CTRLPORT=4002

class ctrlFrame(wx.Frame):
    def __init__(self, cSocket, dCount, dName, *args, **kwds):
        # begin wxGlade: ctrlFrame.__init__
        wx.Frame.__init__(self, *args, **kwds)

        self.ctrl = cSocket
        self.dCount = dCount
        self.dName = dName
        self.dev_button = []
        self.devShow = []
        for i in range(self.dCount) :
            #self.dev_button.append(wx.ToggleButton(self, wx.ID_ANY, _(self.dName[i])))
            self.dev_button.append(wx.BitmapButton(self, wx.ID_ANY, wx.Image("vme1-adc1785-0.png", wx.BITMAP_TYPE_PNG).ConvertToBitmap()))
            self.devShow.append(0)

        self.output0 = wx.TextCtrl(self, wx.ID_ANY, _("ctrlMsg"), style=wx.HSCROLL | wx.TE_BESTWRAP | wx.TE_MULTILINE | wx.TE_READONLY)
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

        self.Bind(wx.EVT_TEXT_ENTER, self.sendMsg, self.input0)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=0: self.sendCmd(evt, cmdId), self.button_Load)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=2: self.sendCmd(evt, cmdId), self.button_Conf)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=4: self.sendCmd(evt, cmdId), self.button_Prep)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=6: self.sendCmd(evt, cmdId), self.button_Star)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=8: self.sendCmd(evt, cmdId), self.button_Paus)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=1: self.sendCmd(evt, cmdId), self.button_unLoad)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=3: self.sendCmd(evt, cmdId), self.button_unConf)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=5: self.sendCmd(evt, cmdId), self.button_End)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=7: self.sendCmd(evt, cmdId), self.button_Stop)
        self.Bind(wx.EVT_BUTTON, lambda evt, cmdId=9: self.sendCmd(evt, cmdId), self.button_Resu)
        for i in range(self.dCount) :
            self.Bind(wx.EVT_BUTTON, lambda evt, devId=i: self.showDev(evt, devId), self.dev_button[i])

        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: ctrlFrame.__set_properties
        self.SetTitle(_("daq ctrl"))
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: ctrlFrame.__do_layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_1 = wx.GridSizer(2, 5, 0, 0)

        grid_sizer_2 = wx.FlexGridSizer(1, self.dCount, 0, 0)
        for i in range(self.dCount) :
            grid_sizer_2.Add(self.dev_button[i], 0, wx.ALL | wx.EXPAND, 0)
        grid_sizer_2.AddGrowableRow(0)

        sizer_1.Add(grid_sizer_2, 5, wx.ALL | wx.EXPAND, 0)
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

    def sendMsg(self, event):  # wxGlade: ctrlFrame.<event_handler>
        print "Event handler 'sendMsg'"
        txt = event.GetEventObject()
        n = txt.GetNumberOfLines()
        if (n>=1) and (self.ctrl!=-1) :
            msg = txt.GetLineText(n)
            print "send msg %s to ctrl"%(msg)
            self.ctrl.sendCtrl(msg.encode('utf8'))

        event.Skip()

    def sendCmd(self, event, cmdId):  # wxGlade: ctrlFrame.<event_handler>
        print "Event handler 'sendCmd'"
        #if cmdId==8 :
            #b = event.GetEventObject()
            #st = b.GetValue()
            #cmdId += st
        msg = "cmd#all#"+str(cmdId)+"#"
        self.ctrl.sendCtrl(msg.encode('utf8'))
        event.Skip()

    def setApp(self, a):
        self.app = a

    def showDev(self, event, devId):
        print "Event handler 'showDev'"
        #b = event.GetEventObject()
        #st = b.GetValue()
        #if self.devShow[devId]!=st :
        df = self.app.frames[devId+1]
        if df!=None :
            st = df.IsShown()
            if st==0 :
                df.Show()
            else :
                df.Hide()
        self.devShow[devId] = df.IsShown()

# end of class ctrlFrame

class msgSwitch(threading.Thread):
    def __init__(self, s):
        super(msgSwitch, self).__init__()
        self.socket = s
        self.frameList = []
        self.nameList = []

    def setList(self, fl, nl):
        self.frameList = fl
        self.nameList = nl

    def run(self):
        print "msgSwitch is running"
        line = ""
        while True :
            data = self.socket.recv(4)
            if data=='' :
                break
            line += data
            idx = line.find('\0')
            if idx<0 :
                continue
            else :
                msg = line[:idx]
                line = line[idx+1:]
                #print "%s"%(msg)
                name = msg[:msg.find('#')]
                frame = -1
                #print name
                if self.nameList.count(name)==1 : 
                    i = self.nameList.index(name)
                    #print "message from %s in devList %dth"%(name,i)
                    frame = self.frameList[i]
                    if (frame!=-1) and (frame!=None) :
                        txt = frame.output0
                        wx.CallAfter(txt.AppendText, msg)
        print "msgSwitch is finished."

class dataSwitch(threading.Thread):
    def __init__(self, s):
        super(dataSwitch, self).__init__()
        self.socket = s
        self.devList = []
        self.nameList = []

    def setList(self, dl, nl):
        self.devList = dl
        self.nameList = nl

    def run(self):
        print "dataSwitch is running"
        buf=''
        while True :
            data = self.socket.recv(4)
            print "data: %s"%(data)
            if data=='' :
                break
            buf += data
            idx = buf.find('end$')
            if idx<0 :
                continue
            else :
                event = buf[:idx]
                buf = buf[idx+4:]
                print "event before: %s"%(event)
                self.switchEvent(event)
                print "event after: %s"%(event)
        print "dataSwitch is finished."

    def switchEvent(self, event):
        idx = event.find('#')
        if idx<0 :
            print "not valid spacer '#' in event record"
        else :
            name = event[:idx]
            data = event[idx+1:]
            if self.nameList.count(name)==1 : 
                i = self.nameList.index(name)
                print "message from %s in devList %dth, %s"%(name,i,data)
                dev = self.devList[i]
                dev.fillEvent(data)


class ctrlSwitch(threading.Thread):
    def __init__(self, s):
        super(ctrlSwitch, self).__init__()
        self.socket = s
        self.devList = []
        self.nameList = []
        self.mutex = threading.Lock()
    
    def setList(self, dl, nl):
        self.devList = dl
        self.nameList = nl

    def sendCtrl(self, cs):
        if self.mutex.acquire(1) :
            self.socket.send(cs.encode('utf8'))
            print "ctrlSwitch is finished."
            self.mutex.release()
        else :
            print "ctrlSwitch is busy."

    def run(self):
        print "ctrlSwitch is running"
        line = ""
        while True :
            data = self.socket.recv(4)
            if data=='' :
                break
            line += data
            idx = line.find('\0')
            if idx<0 :
                continue
            else :
                msg = line[:idx]
                line = line[idx+1:]
            #print "%s"%(msg)
            idx = msg.find('#')
            control = msg[:idx]
            idx = msg.find('#', idx+1)
            name = msg[len(control)+1:idx]
            ret = msg[idx+1:-1]
            dev = -1
            #print self.nameList
            if self.nameList.count(name)==1 : 
                i = self.nameList.index(name)
                #print "message from %s in devList %dth"%(name,i)
                dev = self.devList[i]
                print "%s" %(dev.name)
                if (dev!=-1) and (dev!=None) :
                    print "%s, %s" %(control, ret)
                    dev.ctrlHandler(control, ret)
                    #wx.CallAfter(dev.ctrlHandler, msg)
        print "ctrlSwitch is finished."

class timeDamon(threading.Thread):
    def __init__(self, d, t):
        super(timeDamon, self).__init__()
        self.dev = d
        self.runTime = t

    def run(self):
        print "timeDamon is running"
        while True : 
            time.sleep(self.runTime)
            #print "time step %f" %(self.runTime)
            self.dev.timerHandler()
        print "timeDamon is finished"


class ctrlApp(wx.App):
    def OnInit(self):
        wx.InitAllImageHandlers()
    
        self.addrMsg = (HOST, MSGPORT)
        self.socketMsg = socket(AF_INET, SOCK_STREAM)
        self.addrData = (HOST, DATAPORT)
        self.socketData = socket(AF_INET, SOCK_STREAM)
        self.addrCtrl = (HOST, CTRLPORT)
        self.socketCtrl = socket(AF_INET, SOCK_STREAM)

        self.thpool = []
        self.thpool.append(msgSwitch(self.socketMsg))
        self.thpool.append(dataSwitch(self.socketData))
        self.thpool.append(ctrlSwitch(self.socketCtrl))
        self.thpool.append(timeDamon(self, 1))
        self.msg = self.thpool[0]
        self.data = self.thpool[1]
        self.ctrl = self.thpool[2]

        self.isCfg=os.path.exists("./mlist.conf")
        self.cfgFile = None
        self.modLines = []
        self.modList = []
        self.modName = []
        self.modCount = 0
        if self.isCfg==True :
            self.cfgFile = open("mlist.conf", 'r')
            self.modLines = self.cfgFile.readlines()
            for idx in self.modLines :
                m = idx[:idx.find(' ')]
                n = idx[idx.find(' ')+1:idx.find('\n')]
                self.modList.append(__import__(m))
                self.modName.append(n)
                self.modCount += 1 

        frame0 = ctrlFrame(self.ctrl, self.modCount, self.modName, None, wx.ID_ANY, "")
        frame0.setApp(self)
        self.frames = []
        self.devs = []
        self.names = []
        self.frames.append(frame0)
        self.devs.append(self)
        self.names.append("ctrl")
        for idx in range(self.modCount) :
            self.frames.append(self.modList[idx].devFrame(self.ctrl, None, wx.ID_ANY, ""))
            self.devs.append(self.modList[idx].devApp(self.modName[idx]))
            self.names.append(self.modName[idx])
            self.frames[-1].setDev(self.devs[-1])
            self.devs[-1].setFrame(self.frames[-1])

        self.msg.setList(self.frames, self.names)
        self.data.setList(self.devs, self.names)
        self.ctrl.setList(self.devs, self.names)

        self.SetTopWindow(frame0)
        frame0.Show()

        return 1

    def timerHandler(self):
        for dev in self.devs :
            if dev!=self :
                dev.timerHandler()

    def start(self):
        self.socketMsg.connect(self.addrMsg)
        self.socketData.connect(self.addrData)
        self.socketCtrl.connect(self.addrCtrl)

        for th in self.thpool :
            th.start()

        return 1

    def stop(self):
        self.socketMsg.close()
        self.socketData.close()
        for th in self.thpool :
            th.join()

        return 1

    def OnExit(self):
        self.stop()
        if frame0!=None :
            frame0.Destroy()

        return 1


# end of class ctrlApp

if __name__ == "__main__":
    gettext.install("ctrl0") # replace with the appropriate catalog name

    ctrl0 = ctrlApp(0)
    ctrl0.start()
    ctrl0.MainLoop()
