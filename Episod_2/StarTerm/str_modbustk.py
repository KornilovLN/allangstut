#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 15 21:24:05 2023

@target: Class MB test (used modbus_tk)

@author: ln.starmark@gmail.com
         ln.starmark@ekatra.io
"""
import time
import traceback
import serial

import modbus_tk.defines as tkCst
import modbus_tk.modbus_rtu as tkRtu

import str_common as strc

#---PORTSET = {'portNbr':'/dev/ttyACM0', 'baudrate':115200, 'timeoutSp':0.004}
PORTSET = {'portNbr':'/dev/ttyACM0', 'baudrate':19200, 'timeoutSp':0.018}
INPUTS_RG = [0, 30]
HOLDINGS_RG = [0, 23]
SLAVE = 1

DEBUG = False #True

def MBtestSpeed(iterSp):
    slavesArr = [1]
    err3Cnt = 0
    err4Cnt = 0    
    tb = None
    
    rez3 = []
    rez4 = []
    
    print(f"\n<<< modbus_tk working {iterSp} times >>>")

    tkmc = tkRtu.RtuMaster(serial.Serial(port=PORTSET['portNbr'], 
                                         baudrate=PORTSET['baudrate']))
    tkmc.set_timeout(PORTSET['timeoutSp'])

    startTs = time.time()
    for i in range(iterSp):
    
        if(DEBUG): 
            print(f"i: {i}")
    
        for slaveId in slavesArr:
      
            if(DEBUG): 
                print(f"slaveId: {slaveId} InputsRG  ",end=' >> ')          
        
        try:
            rez4 = tkmc.execute(slaveId, tkCst.READ_INPUT_REGISTERS, INPUTS_RG[0], INPUTS_RG[1])
            if(DEBUG): 
                print(rez4)
        except:
            err4Cnt += 1
            tb = traceback.format_exc()
    
            if(DEBUG): 
                print(f"slaveId: {slaveId} HoldingsRG",end=' >> ')    
        try:
            rez3 = tkmc.execute(slaveId, tkCst.READ_HOLDING_REGISTERS, HOLDINGS_RG[0], HOLDINGS_RG[1])
            if(DEBUG): 
                print(rez3)
        except:
            err3Cnt += 1
            tb = traceback.format_exc()    

    stopTs = time.time()
    timeDiff = stopTs  - startTs
    print(f"timeDiff = {timeDiff} sec")

    if(DEBUG == False): 
        print(rez4)
        print(rez3)
        print()        

    if err3Cnt >0:
        print(f"   !modbus-tk:\terrCnt: err3Cnt: {err3Cnt}; last tb: {tb}")
    if err4Cnt >0:
        print(f"   !modbus-tk:\terrCnt: err4Cnt: {err4Cnt}; last tb: {tb}")
    
    tkmc.close()


def Test_ReadSpeed(mb, fn, cnt):
    
    if(fn == 4):
        print(f"\n<<< Inputs   registers >>>")
        startTs = time.time()
    
        for i in range(cnt):
            regs = mb.ReadAll_InputRG()
        
        timeDiff = time.time()  - startTs
        print(f"timeDiff = {timeDiff} sec")
        print(regs)  

    if(fn == 3):
        print(f"\n<<< Holdings registers >>>")
        startTs = time.time()
    
        for i in range(cnt):
            regs = mb.ReadAll_HoldingRG()   
        
        timeDiff = time.time()  - startTs
        print(f"timeDiff = {timeDiff} sec")
        print(regs) 
        

def Test_OutOneValueInputRG(mb, addr, counter):
    cnt = 0
    lim = 32768
    print()
    print(f"<<< Out inpRG[{addr}] {counter} times >>>")
    while (cnt < counter):
        #print(mb.Read_InputsRG())
        rg = mb.Read_InputRG(addr,1)
        if(rg == None):
            break
        
        rez = rg[0]
        if(rez >= lim):
            rez = rez-65535
            
        print(f"rez={rez}")
        
        cnt += 1
        
    
def Test_WriteMultipleRG(mb, addr, data):
    mb.Write_HoldingRG(addr, data)
    print()
    print(f"<<< Test_WriteMultipleRG: addr={addr},  data={data}  >>>")
    print(mb.Read_HoldingRG(addr, len(data)))   
    

def Test_WriteOneRG(mb, addr, value):
    mb.Write_HoldingOneRG(addr, value)
    print()
    print(f"<<< Test_WriteOneRG: addr={addr},  value={value}  >>>")
    print(mb.Read_HoldingRG(addr, 1)) 

    
    
class MB():
    def __init__(self,portset, inp_rg, hld_rg, slave):
        self.portset = portset
        self.ser = None
        
        self.slave = slave
        self.inp_rg = inp_rg
        self.hld_rg = hld_rg
        
        self.tkmc = None
        
        self.tb = None
        
        try:
            self.ser = serial.Serial(port=self.portset['portNbr'], 
                                     baudrate=portset['baudrate'])
            self.tkmc=tkRtu.RtuMaster(self.ser)
            self.tkmc.set_timeout(self.portset['timeoutSp'])
            print(f"<<< MB create Ok >>>")
        except:
            self.tb = traceback.format_exc()
            print(f"<<< MB not create >>>")
            print(self.tb)
            
    
    def ReadAll_InputRG(self):
        rez = None
        try:
            rez = self.tkmc.execute(self.slave, 
                                    tkCst.READ_INPUT_REGISTERS, 
                                    self.inp_rg[0], self.inp_rg[1])
        except:
            self.tb = traceback.format_exc()
            print(self.tb)
        return rez       
    
 
    def ReadAll_HoldingRG(self):
        rez = None
        try:
            rez = self.tkmc.execute(self.slave, 
                                    tkCst.READ_HOLDING_REGISTERS, 
                                    self.hld_rg[0], self.hld_rg[1])
        except:
            self.tb = traceback.format_exc()
            print(self.tb)
        return rez      
    
    
    def Read_InputRG(self, startRG, quantRG):
        rez = None
        if( (startRG + quantRG) > (self.inp_rg[0] + self.inp_rg[1]) ):
            print(f"<<< Error registers [{startRG},{quantRG}] >>>")
            return rez
        
        try:
            rez = self.tkmc.execute(self.slave, 
                                    tkCst.READ_INPUT_REGISTERS, 
                                    startRG, quantRG)
        except:
            self.tb = traceback.format_exc()
            print(self.tb)
        return rez   
    
    
    def Read_HoldingRG(self, startRG, quantRG):
        rez = None
        if( (startRG + quantRG) > (self.inp_rg[0] + self.inp_rg[1]) ):
            print(f"<<< Error registers [{startRG},{quantRG}] >>>")
            return rez
        
        try:
            rez = self.tkmc.execute(self.slave, 
                                    tkCst.READ_HOLDING_REGISTERS, 
                                    startRG, quantRG)
        except:
            self.tb = traceback.format_exc()
            print(self.tb)
        return rez  


    def Write_HoldingOneRG(self, startRG, value):
        rez = False
        if( (startRG + 1) > (self.inp_rg[0] + self.inp_rg[1]) ):
            print(f"<<< Error registers [{startRG}] >>>")
            return rez
        
        try:
            self.tkmc.execute(self.slave, 
                              tkCst.WRITE_SINGLE_REGISTER, 
                              startRG, 
                              output_value=value)
        except:
            self.tb = traceback.format_exc()
            print(self.tb)     
    
        return True
    
    
    def Write_HoldingRG(self, startRG, data):      
        rez = False
        if( (startRG + len(data)) > (self.inp_rg[0] + self.inp_rg[1]) ):
            print(f"<<< Error registers [{startRG}] >>>")
            return rez        
        
        try:
            self.tkmc.execute(self.slave, 
                              tkCst.WRITE_MULTIPLE_REGISTERS, 
                              startRG, 
                              output_value=data)
        except:
            self.tb = traceback.format_exc()
            print(self.tb)
  
        return True

           
    
    def MB_close(self):
        self.tkmc.close()
        
        
#=============================================================================

def main():
    
    strc.titleprogram("Цель программы - обучение работы с modbus_tk",
                      "Tutorial use Modbus_tk library",
                      "ln.starmark@ekatra.io")
    
    print(f"\n<<< Now in work Baudrate = {PORTSET['baudrate']} >>>\n")
    
    mb = MB(PORTSET, INPUTS_RG, HOLDINGS_RG, SLAVE)
    
    Test_OutOneValueInputRG(mb, 14, 1)
    Test_OutOneValueInputRG(mb, 15, 1)
    
    Test_ReadSpeed(mb, 4, 10)    
    Test_ReadSpeed(mb, 3, 10)    
    MBtestSpeed(10)       
    
    Test_WriteOneRG(mb, 18, 777)   
    Test_WriteOneRG(mb, 19, 888)  
    Test_WriteMultipleRG(mb, 20, (1828, 31415,))
    
###===========================================================================
if __name__ == "__main__":
    main()  
###===========================================================================   
