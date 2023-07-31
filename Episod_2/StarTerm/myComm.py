#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat May  6 00:03:59 2023

@author: starmark
"""

import sys
import str_common as str_c
import PFP_Parser as prs

import serial
from serial.tools import  list_ports

from threading import Thread, Event
from collections import deque

from time import sleep
import datetime
from datetime import date


DEBUG = True

#OPSYS = "WIN"
OPSYS = "LIN"


class Comm():
    def __init__(self,master,baudrate, metavar, metavar2, pars=0):
        self.dict_stat_port = {"0":" closed  ", "1":" opened  "}         
        self.stat_port = 0          # port OFF
        self.metavar = metavar
        self.metavar2 = metavar2
        self.pars = pars            # parser variant
        
        self.PORT = ""              # port name
        self.ser = None             # descriptor      

        self.q = deque()
        self.SIGNAL = False 
        self.SIGNAL2 = True 
        
        self.full_name_port = ""    # для инф
        

        self.BAUDRATE = baudrate
        self.TIMEOUT  = 0.001

        self.count = 0
        self.item = []
        self.val = 0.0

    def change_port(self):
        #--- параметр DEV_ID получим как константу
        str_c.zagolovok('Get port from list:')
        nmport = 0
        my_ports = []
  
        listports = list_ports.comports()
        print(f"<<< Work with utilites: 'List comm ports':\n\n{listports}\n")
        
        for port in listports:
            
            st_port = str(port)
            full_name = st_port
            
            print(f"st_ports: {st_port}\n")
            
       
            if(OPSYS == "LIN"):
                if(st_port[0:12] not in ["/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyACM0"]):
                    continue
                else:                    
                    return st_port[0:12] , full_name  
                
            elif(OPSYS == "WIN"):    
                full_name = st_port
            
                st_port = st_port[:st_port.index(' ')]
                my_ports.append(st_port)        
        
                prt = str(port)
                ind_tire = prt.find(" - ")        
        
                ind_beg = ind_tire+3
                prt1 = prt[ind_beg:]
        
                ind_scoba = prt1.find(" (")
                prt2 = prt1[:ind_scoba]
            
                #--- Тут надо вывести все найденные порты в комбобоч и там выбрать\
                '''
                print("Device_ID= %s\t DEV_ID= %s" % (self.Device_ID, self.DEV_ID))
            
                if(self.Device_ID == self.DEV_ID):
                    print(prt2)    
                    print("%5s -> %d" % (st_port, nmport))
                    return st_port , full_name            
                else:
                    nmport += 1
                '''
                
        return "COM0", "COM0"
         


    def ComPort_Work(self):
        
        #--- поток байт принять
        if(self.pars == 0):

            #--- Сбрасываем буфер приема
            self.ser.flushInput()
            lin = []
            itm = []
            while(True):

                if (self.ser.in_waiting > 0):
                    lin.append(self.ser.read(self.ser.in_waiting))

                    for el in lin:
                        bt = el.decode('utf-8')
                        itm.append(el)
                        
                        for ch in bt:                             
                            print(ch ,end='')
                            #itm.append(str(ch))
                            
                            if(ch == '\n'):                                
                                print()
                                #itm.append(str(ch))
                            
                        if ((len(self.q) == 0) and (self.SIGNAL == False)):
                            self.q.append(itm)     #--- пойдет в таблицу

                            sleep(0.002)
                            itm.clear()
                            self.SIGNAL2 = True
                          
                    lin.clear()        


        #--- принять особый поток данных    
        elif(self.pars == 1):    
       
            #--- По приходу bBegin {0x7E} переводим start = 1
            #--- потом ждем 2-й байт с количеством байт в посылке
            start = 0
            fl_stop = 0   
            #--- должно быть в приходе        
            quantity_bytes = 0
            #--- реально пришлых байт
            cnt_bytes = 0
            
            #--- Сбрасываем буфер приема
            self.ser.flushInput()
            
            #--- Цикл вычитывания посылки и ее обработка 
            while fl_stop == 0:        
        
                #--- буфер для прихода
                lst = []
        
                #--- Забрать данные из буфера порта, как только появятся  
                if (self.ser.in_waiting > 0):
                    lin = self.ser.read(self.ser.in_waiting)            
        
                    for el in lin:                
                        if (el == prs.bBEGIN and start == 0):
                            #--- Наконец встречен стартовый символ. Старт!!!
                            lst.append(el)
                            #--- наращивая количество принятых реально байт
                            cnt_bytes += 1
                            #--- Взвод флажка и переход к ловле блох
                            start = 1                     
                        else:
                            #--- Запомнить оглашенное в посылке к-во бвйт
                            if(cnt_bytes == 1):              
                                quantity_bytes = prs.Get_Quantity(el)   
                            #--- и продолжить наполнять список    
                            lst.append(el)
                            #--- наращивая количество принятых реально байт
                            cnt_bytes += 1
                            #--- ставим точки от скуки
                            if(DEBUG == True):
                                print(".", end = " ")                              
                              
                    sleep(0.002) 
                     
                    if(DEBUG == True):
                        print("cnt_bytes = %d" % cnt_bytes)                
            
                    #--- Обрезаем лишние байты с конца списка
                    lst = lst[0:quantity_bytes]
                    #--- Показать приход 
                    if(DEBUG == True):
                        self.Print_Codes(lst)
                       
                    #--- Парсерить приход -----------------------------------
                    dct = {}
                    dct = prs.Parser(lst)
                    #--------------------------------------------------------
        
                    if(dct["sMode"]=='INDICATION'):
                        dt = date.today().strftime("%d-%m-%Y") 
                        tm = datetime.datetime.now().strftime("%H:%M:%S")            
                        self.item.append(self.count)
                        self.item.append(dt)
                        self.item.append(tm)
                        self.item.append(dct['sFM_NAME'])
                        self.item.append(dct['len_wave'])
                        self.val = dct['Val_1']
                        self.item.append(dct['Val_1'])
                        self.item.append(dct['unit_1'])
                        self.item.append(dct['Val_2'])
                        self.item.append(dct['unit_2'])
                        self.item.append(dct['sBell'])
                        BothReading = 1
                        
                    if(dct["sMode"]=='AKK'): 
                        mvlt = dct['mvolts[]']
                        self.item.append(mvlt[0]/1000.)
                        self.item.append(mvlt[1]/1000.)
                        self.item.append(mvlt[2]/1000.)              
                        BothReading = 2
        
                    if(BothReading == 2):
                        #--- отправка в 2 очереди - в таблицу и в график
                        if( (len(self.q) == 0) and (self.SIGNAL == False) ):             
                            self.q.append(self.item)#--- пойдет в таблицу                     
                            BothReading = 0                      
                            sleep(0.002)                    
                            self.SIGNAL2 = True                              
                            
                        self.count += 1  
                    #--------------------------------------------------------
                          
                    #--- Сбросить переменные и флажки для начала нового цикла                  
                    cnt_bytes = 0
                    start = 0
                    fl_stop = 0
                    
                    self.ser.reset_input_buffer()
                    self.ser.flushInput()
                    
         
    
    def ComPort_Open(self, Port):
            str_c.zagolovok('Port opening:')
      
            try:
                ser = serial.Serial(      
                port = Port,
                baudrate = self.BAUDRATE,
                parity = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                bytesize = serial.EIGHTBITS,
                timeout = self.TIMEOUT, 
                ) 
                print("<<< Connect to: ",ser.port)
     
                ser.setDTR(False) 
                ser.setRTS(False)       
    
                print('\n--- Соединение удалось.\n') 
                return ser 
    
            except serial.SerialException:
                print('\n--- Error: Соединение не удалось ---')
                print() 
                sys.exit()          
          
    
    def Port_Open(self):
        if((self.pars == 0) or (self.pars == 1)):
            
            st, self.full_name_port = self.change_port()
            print(f"st: {st}\tFull name port {self.full_name_port}") 
            if(st == "COM0"):  
                print("<<< Device not found: st=%s\tfull_name_port=%s" % (st, self.full_name_port))
                print("<<< Connect Device now!!!")
            else:     
                self.PORT = st
                print(f"\nSELF.PORT = st: {self.PORT}")
            
                #--- получаем дескриптор соединения с портом
                self.ser = self.ComPort_Open(self.PORT)
        
                #--- вывод в statusbar табличку ----
                self.metavar2.set(self.full_name_port)
                #-----------------------------------
                self.stat_port = "1"    
                self.changePortText(self.stat_port, self.PORT)

            #=== Запуск потока приема из порта =========
            Thread(target=self.ComPort_Work, args=()).start() 
            #===========================================
        
    def Port_Close(self):
        if((self.pars == 0) or (self.pars == 1)):    
            #--- вывод в statusbar табличку ----
            self.full_name_port = "Device closed.      Press 'Open' key menu"
            self.metavar2.set(self.full_name_port)
            #----------------------------------=
            if(self.stat_port == "1"):
                self.stat_port = "0"
                self.changePortText(self.stat_port, self.PORT)     
                print('\nЗакрываем порт')
                self.ser.close()  
            
    def changePortText(self, stat_port, prt):
        str_comm = "Port " + prt + self.dict_stat_port.get(stat_port)
        self.metavar.set(str_comm)
        
    def Port_Info(self, stat_port):
        #--- Вначале порт закрыт
        if(stat_port == "0"):
            self.changePortText(stat_port, "COMxx") 
        print("stat_port = %s\t{%s}" % (stat_port, self.dict_stat_port.get(stat_port) ) )                         
            
    def Print_Codes(self, lst):
        print("< ", end = ' ')
        for el in lst:        
            print(hex(el),end=' ')
        print(" >\n")     