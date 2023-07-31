#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 16:37:08 2023

@author: starmark
"""

import sys
import os

import serial
from serial.tools import  list_ports

import str_common as str_c
import Config as config
import PanelMethods as pn_mtd
import myComm as com
import myMenu as men

import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
from tkinter import font
from tkinter import messagebox
from tkinter.messagebox import showerror, showwarning, showinfo
import warnings

from threading import Thread, Event
from collections import deque

from time import sleep

import str_modbustk as str_mb


"""
import numpy as np 
import math as mt
import pandas as pd



import time
import timeit
import datetime
from datetime import date


import usb
import usb.core
import usb.util
#import win32api


import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

import PFP_Parser as prs
"""

#=============================================================================
DEBUG = False
WRITE_TO_CONSOLE = False
WRITE_TO_HOLDING = False

''' 
OPSYS = "WIN"
PATH_MAIN_ICON = "img\\dali.png"
PATH_CONFIG = 'config\\Config_StarTerm.ini'
PATH_XLSX = "xlsx\\StarTerm"
KOSAIA = "\\"
#USB_INFO = "USB\\VID_0x1a86&PID_0x7523"
'''

OPSYS = "LIN"
PATH_MAIN_ICON = "img/dali.png"
PATH_CONFIG = 'config/Config_StarTerm.ini'
PATH_XLSX = "xlsx/StarTerm"
KOSAIA = "/"
#USB_INFO = "USB\\VID_0x1a86&PID_0x7523"

    
PRG_NAME = "StarTerm"
PRG_USER = "LN Starmark"
VAR_STR = "Test string"

MAIN_TITLE = PRG_NAME


BAUDRATE = 19200

MAXVALUE_RG = 65535

#---PORTSET = {'portNbr':'/dev/ttyACM0', 'baudrate':115200, 'timeoutSp':0.004}
PORTSET = {'portNbr':'/dev/ttyACM0', 'baudrate':19200, 'timeoutSp':0.018}
INPUTS_RG = [0, 30]
HOLDINGS_RG = [0, 23]
SLAVE = 1   

#--- Factory settings
BAZE_X=1500
BAZE_Y=1100
SHFT_X=0
SHFT_Y=0
PADDING=0
GUI_RESIZABLE_X=False
GUI_RESIZABLE_Y=False
COMMON_LABEL=False
ALL_LABEL=True
meny=40
mova="EN"
BAUDRATE=19200
PARSER=2

COLOR_LAB = {"bg":"azure4", "fg":"antiquewhite1",}
COLOR_FRM = {"bg":"dimgray", "fg":"bisque",}
COLOR_STAT = {"bg":"darkolivegreen", "fg":"antiquewhite1",}
COLOR_TEXT_COMM = {"bg":"darkslategray", "fg":"antiquewhite1",}

COLOR_EVEN_BG = 'DarkSeaGreen1'
COLOR_EVEN_FG = 'navy'
COLOR_ODD_BG = 'lightyellow'
COLOR_ODD_FG = 'brown'  

LARGE_FONT = 8
MON_FONTSIZE = 10
NOTEBOOK_FONTSIZE = 9 
#=============================================================================

class App(ttk.Frame):
   
    def __init__(self, master=None):        
        super().__init__(master)
        self.pack()

        self.font_lb = font.Font(family= "Verdana", 
                             size=12, weight="normal", 
                             slant="roman", 
                             underline=False, overstrike=False)
        
        #--- Meta Variables --------------------------------------------------
        self.metaVar = MetaVariables()    
   
        #--- Читаем конфигурацию
        self.prm = dict()
        self.cfg = config.Cfg(PATH_CONFIG, self.prm)
        self.prm = self.cfg.loadConfig()  
        self.cfg.OutConfig()         
        
        sWIN = self.cfg.Get_sWIN()
        self.COMMON_LABEL = self.cfg.Get_COMMON_LABEL()
        self.ALL_LABEL = self.cfg.Get_ALL_LABEL()
        self.BAZE_X = self.cfg.Get_BAZE_X()
        self.BAZE_Y = self.cfg.Get_BAZE_Y()
        self.PADDING = self.cfg.Get_PADDING()
        self.baud = self.cfg.Get_Baud()
        self.parserN = self.cfg.Get_Parser()
        
        self.mb = None

        #--- Утилиты построения окон и виджктов 
        self.pn = pn_mtd.PanelMethods(self.PADDING)
        
        #--- Уствнавливает порт ----------------------------------------------
        self.com = com.Comm(master,
                            self.baud, 
                            self.metaVar.statPortText, 
                            self.metaVar.InfoAddText,
                            self.parserN)  
        self.q = self.com.q    
        self.SIGNAL = self.com.SIGNAL
        self.SIGNAL2 = self.com.SIGNAL2
        
        if(self.parserN == 2):
            print(f"\n<<< Modbus init >>>")
            self.mb = str_mb.MB(PORTSET, INPUTS_RG, HOLDINGS_RG, SLAVE)
        
        #--- Устанавливаем главное окно --------------------------------------
        master.geometry(sWIN)
        master.resizable(self.cfg.Get_GUI_RESIZABLE_X(), 
                         self.cfg.Get_GUI_RESIZABLE_Y()) 
        master.title(MAIN_TITLE)    
        #self.icon = ImageTk.PhotoImage(file = PATH_MAIN_ICON) 
        #master.iconphoto(True, self.icon)  
        master.tk.call('wm', 'iconphoto', root._w, tk.PhotoImage(file=PATH_MAIN_ICON))
        #master.iconbitmap(PATH_MAIN_ICON)
      
        
        #--- строим paneles --------------------------------------------------        
        self.pan0 = master      
        
        #--- сразу установим меню --------------------------------------------
        self.menu = men.MyMenu(self.pan0, self.com)
        self.main_menu = self.menu.create_menu()
        master.config(menu=self.main_menu) 
        
        
        self.frm_status = self.pn.create_BOTTOM(self.pan0, "nw", self.BAZE_X, self.BAZE_Y)      
        self.pn.create_statusbar(self.frm_status, self.metaVar.statPortText, self.metaVar.InfoAddText)   
        
        BAZE_X = self.BAZE_X
        BAZE_Y = self.BAZE_Y     
        self.pan01 = self.pn.Pan(self.pan0, tk.TOP, tk.BOTH, "nw", 
                                 BAZE_X-2*self.PADDING,BAZE_Y-2*self.PADDING,1)

        #--- Левая панедь ----------------------------------------------------
        w = BAZE_X*3//5-2*self.PADDING
        h = BAZE_Y-2*self.PADDING
        self.pan01L = self.pn.Pan(self.pan01,tk.LEFT,tk.BOTH,"nw",w,h,1)
        
        #--- Левая верхняя панедь --------------------------------------------
        w1 = w-2*self.PADDING
        h1 = h//3-2*self.PADDING
        self.pan01LT = self.pn.Pan(self.pan01L,tk.TOP,tk.BOTH,"nw",w1,h1,1)
        
        w11 = w1//4-self.PADDING
        h11 = h1-2*self.PADDING
        self.frmlab_COM = self.pn.create_FrameLab(self.pan01LT,
                                                  tk.LEFT,tk.BOTH,"nw",
                                                  f"        COMM port       ",
                                                  w11,h11,
                                                  ["khaki4","lightyellow"],
                                                  [1,1,1])
        
        self.frmlab_CFG = self.pn.create_FrameLab(self.pan01LT,
                                                  tk.LEFT,tk.BOTH,"nw",
                                                  f"         Config         ",
                                                  w11,h11,
                                                  ["khaki4","lightyellow"],
                                                  [1,1,1])

        self.frmlab_PAR = self.pn.create_FrameLab(self.pan01LT,
                                                  tk.LEFT,tk.BOTH,"nw",
                                                  f"        Parameters      ",
                                                  w11,h11,
                                                  ["khaki4","lightyellow"],
                                                  [1,1,1])
        
        self.frmlab_ABOUT = self.pn.create_FrameLab(self.pan01LT,
                                                  tk.LEFT,tk.BOTH,"nw",
                                                  f"          About         ",
                                                  w11,h11,
                                                  ["khaki4","lightyellow"],
                                                  [1,1,1])

        

        #--- Разделитель панедей горизонтальный    
        h2 = self.PADDING/2 
        self.pan01LM = self.pn.Pan(self.pan01L, tk.TOP, tk.BOTH, "nw",w,h2,0)      
        
        
        #--- Левая нижняя панедь
        self.viewer_EN = False
        
        w1 = w-2*self.PADDING
        h3 = h*2//3-4*self.PADDING            
        self.pan01LB = self.pn.Pan(self.pan01L, tk.TOP, tk.BOTH, "nw",w1,h3,1)        
        self.txt_01LBL = self.pn.create_Text(self.pan01LB,"Out data from Commport", 
                                             120, 40,"black","yellow")
        
        
        #--- Панель нижняя после панели текстового вывда
        self.pan_A = self.pn.Pan(self.pan01LB, tk.BOTTOM, tk.BOTH, "nw", 20, 20, 1)

        
        #--- Панель ввода в регистры Holdings
        self.pan_B = self.pn.Pan(self.pan_A, tk.LEFT, tk.X, "nw", 20, 20, 1)
        self.lab_B = tk.Label(master=self.pan_B, text="Edit HoldingRG", font=self.font_lb, 
                              bg="khaki4", fg="lightyellow", 
                              bd=1, relief=tk.SUNKEN, anchor="c") 
        self.lab_B.pack(side=tk.TOP, fill=tk.BOTH)
        
        self.ent_addr = 0
        self.ent_quant= 0        
        self.ent_addr = self.pn.create_FrameEntry(self.pan_B,tk.TOP,True,tk.Y,"nw",
                                            "Set start RG <0..22>:    ",
                                            40, 20 , 30,
                                            [COLOR_FRM['bg'],"lightyellow"],
                                            [0,0])
        
        self.ent_quant = self.pn.create_FrameEntry(self.pan_B,tk.TOP,True,tk.Y,"nw",
                                            "Set datas_RG <23-start>: ",
                                            40, 20, 70,
                                            [COLOR_FRM['bg'],"lightyellow"],
                                            [0,0])
        
        self.edit_button = self.pn.create_Button(self.pan_B,self.edit_button_callback,
                                                 tk.TOP,False,tk.Y,"nw",
                                                 "Write",40,20,[0])
                
        
        #--- Панель вьювера
        self.viewer = self.pn.create_Viewer(self.pan_A,self.viewer_button_callback,
                                         tk.LEFT,True,tk.BOTH,"nw",
                                         "Text Viewer","Enable viewer",
                                         w,h,
                                         ["khaki4","lightyellow"],
                                         [1,0,0])
        #---------------------------------------------------------------------
       
        
        #--- Разделитель длевой и правой панелей
        w = self.PADDING/2
        h = self.BAZE_Y-2*self.PADDING
        self.pan01M = self.pn.Pan(self.pan01, tk.LEFT, tk.BOTH, "nw", w, h, 0)    
        
        
        
        #--- Правая панедь ----------------------------------------------------   
        InpRGnames = ["IdBaud","Timeout","Mode","Errcount","Status",
                      "Serial_0","Serial_1","Serial_2","Serial_3","Serial_4","Serial_5",
                      "Flashsize","Devtype","Firmware",
                      "Inp_0","Inp_1","Inp_2","Inp_3","Inp_4","Inp_5","Inp_6","Inp_7",
                      "P_86BCD","Conductivity","T_fahr","T_chip","T_cels","T_86BSD",
                      "Reserv_1","Reserv_2"]
        
        HldRGnames = ["IdBaud","Timeout","Mode","Editinfo","Pwd",
                      "Ser_0","Ser_1","Ser_2","Ser_3","Ser_4","Ser_5",
                      "NewID",
                      "Dt_0","Dt_1","Dt_2","Dt_3",
                      "Par_0","Par_1","Par_2","Par_3",
                      "Reserv_0","Reserv_1","Reserv_2"]
                
        self.HldRG_quantity = len(HldRGnames)
        self.InpRG_quantity = len(InpRGnames)
        
        self.Vid_InpRG = []
        self.Vid_HldRG = []
        
        w_loc = 18
        h_loc = 20
        b_loc = 0     
        
        
        w = self.BAZE_X*2/10-2*self.PADDING
        h = self.BAZE_Y-2*self.PADDING
        self.pan01R = self.pn.Pan(self.pan01, tk.LEFT, tk.BOTH, "nw", w, h, 1)       
        
        w = w//2 - 2*self.PADDING
        h = h - 2*self.PADDING
        self.pan011R = self.pn.Pan(self.pan01R, tk.TOP, tk.X, "nw", w, 20, 0)
        self.lab_vars = self.pn.Lab(self.pan011R, tk.TOP, tk.X, "nw", f"Input Variables: [{self.InpRG_quantity}]")        
        self.pan012R = self.pn.Pan(self.pan01R, tk.TOP, tk.BOTH, "nw", w, h, 0)        
        #--- подпора
        self.pan013R = self.pn.Pan(self.pan01R, tk.TOP, tk.BOTH, "nw", w, h, 0) 
        
        for i in range(self.InpRG_quantity):
            vid = self.pn.create_VidVar(self.pan012R, i, InpRGnames[i], 
                                        '{}'.format(self.metaVar.InpRGvar[i]),                                           
                                        tk.TOP,tk.X,"nw",w_loc,h_loc,b_loc, 0)
            self.Vid_InpRG.append(vid)     

        
        w1 = self.BAZE_X*2/10-2*self.PADDING
        h1 = self.BAZE_Y-2*self.PADDING
        self.pan02R = self.pn.Pan(self.pan01, tk.LEFT, tk.BOTH, "nw", w1, h1, 1)  

        self.pan021R = self.pn.Pan(self.pan02R, tk.TOP, tk.X, "nw", w, 20, 0)
        self.lab_vars = self.pn.Lab(self.pan021R, tk.TOP, tk.X, "nw", f"Holding Variables: [{self.HldRG_quantity}]")        
        self.pan022R = self.pn.Pan(self.pan02R, tk.TOP, tk.BOTH, "nw", w, h, 0)
        #--- подпора
        self.pan023R = self.pn.Pan(self.pan02R, tk.TOP, tk.BOTH, "nw", w, h, 0) 
        
        
        
        for i in range(self.HldRG_quantity):
            vid = self.pn.create_VidVar(self.pan022R, i, HldRGnames[i], 
                                        '{}'.format(self.metaVar.HldRGvar[i]),                                           
                                        tk.TOP,tk.X,"nw",w_loc,h_loc,b_loc,1)
            self.Vid_HldRG.append(vid)
            
            
            
        #===========================================
        if(self.parserN == 2):
            print(f"\n<<< Modbus work >>>") 
            self.thread_mb = Thread(target=self.MB_Test, args=(self.metaVar, self.viewer_EN, 10000, 0.005))
            self.thread_mb.start()
        
        #self.thread_out = Thread(target=self.Item_to_table, args=())
        #self.thread_out.start()        
        
        #===========================================
        
        # Регистрация события закрытия окна и привязка к функции -------------
        root.protocol("WM_DELETE_WINDOW", self.menu.finish)
        
###===========================================================================

    def viewer_button_callback(self):
        if(self.viewer_EN):
            self.viewer_EN = False
            self.viewer['text'] = 'Enable  Viewer'
        else:    
            self.viewer_EN = True
            self.viewer['text'] = 'Disable Viewer'


    def is_number(self, str):
        try:
            float(str)
            return True
        except ValueError:
            return False
        
        
    def edit_button_callback(self):
        #---Забираем строки стартового адреса и данных из entry
        s_start = self.ent_addr.get()
        s_data = self.ent_quant.get()
        
        #--- Проверка стартового адреса
        if(self.is_number(s_start)):
            start = int(s_start)
            
            if(start > self.HldRG_quantity-1):
                self.ent_addr.delete(0, tk.END)
                self.ent_quant.delete(0, tk.END)
                self.edit_button['text'] = f"<<< Error enter 'start'={start} > {self.HldRG_quantity-1} >>>"
                return  
            
            s_datas = s_data.split()
            
            datas = []
            for el in s_datas:
                if(self.is_number(el)):
                    iel = int(el)
                    if(iel < MAXVALUE_RG):
                        datas.append(iel)
                    else:
                        self.ent_addr.delete(0, tk.END)
                        self.ent_quant.delete(0, tk.END)
                        self.edit_button['text'] = f"<<< Error: 'data'>{MAXVALUE_RG} >>>"
                        return 
                    
                else:
                    self.ent_addr.delete(0, tk.END)
                    self.ent_quant.delete(0, tk.END)
                    self.edit_button['text'] = f"<<< Error: 'data'={s_datas} >>>"
                    return               
                
            datas = datas[0:self.HldRG_quantity-start]    
            self.edit_button['text'] = f"{start}:{datas} " 
            self.mb.Write_HoldingRG(start, datas) 
            
            self.ent_quant.delete(0, tk.END)
            self.edit_button['text'] = f"Start={start}"       
                
        else:
            self.ent_addr.delete(0, tk.END)
            self.ent_quant.delete(0, tk.END)
            self.edit_button['text'] = f"<<< Error: 'start'={s_start} >>>"
            return

    
    def MB_Test(self, metavar, viewer_EN, n=10, pause=0.01):
        regs = []
        cnt = n
        while cnt > 0:
            
            #--- Пишем для проверки функциями 6 и 16
            if(WRITE_TO_HOLDING): 
                self.mb.Write_HoldingOneRG(22,cnt)     
            
                lst_pay = [cnt,n-cnt,cnt*7-cnt,cnt*7]
                self.mb.Write_HoldingRG(12,lst_pay)
                            
            #--- читаем все
            regs.append(n-cnt)
            regs.append(self.mb.ReadAll_InputRG())           
            regs.append(self.mb.ReadAll_HoldingRG())
            
            
            #--- Можно писать в консоль
            if(WRITE_TO_CONSOLE):
                print(f"N InpRG[{regs[0]:6}]: ",end='')
                print(regs[1])  
                print(f"  HldRG        : ",end='')
                print(regs[2]) 
                
            
            if (self.viewer_EN):
                self.txt_01LBL.insert(tk.INSERT, regs[0])
                self.txt_01LBL.insert(tk.INSERT, "\n")
                self.txt_01LBL.yview_scroll(number=1, what="units")
                self.txt_01LBL.insert(tk.INSERT, regs[1])
                self.txt_01LBL.insert(tk.INSERT, "\n")
                self.txt_01LBL.yview_scroll(number=1, what="units")
                self.txt_01LBL.insert(tk.INSERT, regs[2])
                self.txt_01LBL.insert(tk.INSERT, "\n")
                self.txt_01LBL.yview_scroll(number=1, what="units")    


            #--- Data from Input RG
            for i in range(30):
                metavar.InpRGvar[i].set(regs[1][i])
           
            #--- Data from Holding RG
            for i in range(23):
                metavar.HldRGvar[i].set(regs[2][i])
                
            
            sleep(pause)            
            cnt -= 1
            regs.clear()
            
        self.mb.MB_close()    

        
    def Item_to_table(self):        
                                
            while True:     
                
               if( (len(self.q) != 0) and (self.SIGNAL2 == True) ):                    
                   itm = self.q.popleft()

                   #print(itm)
    
                   if(self.parserN == 0):

                       s = ''
                       for bbb in itm: 
                           
                           barr=bytearray(bbb)
                           
                           s = ''
                           for el in barr: 
                               s += hex(el)
                               s += ' '                   
                           s += '\n'                                              
                           
                       self.txt_01LBL.insert(tk.INSERT, s)
                       self.txt_01LBL.yview_scroll(number=1, what="units")                             

                       sleep(0.002)
                       self.SIGNAL = False
                       itm.clear()
                        
                       
                   elif(self.parserN == 1): 
                       
                       s = ' '.join(map(str, itm))
                       s += "\n"
                       self.txt_01LBL.insert(tk.INSERT, s)
                       self.txt_01LBL.yview_scroll(number=1, what="units")                        
                       
                       sleep(0.1)
                       self.SIGNAL = False
                       itm.clear()        
                           
                
                   
        
class MetaVariables():
    def __init__(self):
        self.statPortText = tk.StringVar()
        self.statPortText.set("Port closed")                                         
        self.InfoAddText = tk.StringVar()
        self.InfoAddText.set("Device not present. Press 'Open' key menu")

        self.NameDevText = tk.StringVar()
        self.NameDevText.set("Factory name")
        
        
        self.InpRGvar = []
        for i in range(30):
            el = tk.IntVar()
            el.set(0)
            self.InpRGvar.append(el)
            
        self.HldRGvar = []
        for i in range(23):
            el = tk.IntVar()
            el.set(0)
            self.HldRGvar.append(el)    
            
            
            
 
    def Set_statPortText(self, s):
        self.statPortText.set(s)

    def Set_InfoAddText(self, s):
        self.InfoAddText.set(s)
        
    def Set_NameDevText(self, s):
        self.NameDevText.set(s)
        
    def Get_statPortText(self):
        return self.statPortText.get()

    def Get_InfoAddText(self):
        return self.InfoAddText.get()
        
    def Get_NameDevText(self):
        return self.NameDevText.get()   


    
               
    def Set_InpRGvar(self,n,v):
        self.InpRGvar[n].set(v)
        
    def Get_InpRGvar(self,n):
        return self.InpRGvar[n].get()    


        
          
        
    
###===========================================================================   
str_c.titleprogram("Terminal GUI application", 
                   "Program template for work with Comm ports", 
                   "LN Starmark", mult = str_c.MULT1)        
root = tk.Tk()
app = App(master=root)
root.mainloop() 
###===========================================================================
