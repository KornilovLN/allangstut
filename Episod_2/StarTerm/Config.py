#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 19:05:14 2023

@author: starmark
"""

import configparser as cfprs

#--- config ------------------------------------------------------------------ 
class Cfg():
    def __init__(self, path, dct):
        self.path = path
        self.dct = dict(dct)
        
    ###--- Загрузить config
    def loadConfig(self):
        cfg = cfprs.ConfigParser()  # создаём объекта парсера
        cfg.read(self.path)  # читаем конфиг
        self.dct["BAZE_X"] =      int(cfg["Main"]["BAZE_X"])
        self.dct["BAZE_Y"] =      int(cfg["Main"]["BAZE_Y"])
        self.dct["SHFT_X"] =      int(cfg["Main"]["SHFT_X"])
        self.dct["SHFT_Y"] =      int(cfg["Main"]["SHFT_Y"])
        self.dct["PADDING"] =     int(cfg["Main"]["PADDING"])        
        self.dct["GUI_RESIZABLE_X"] =   cfg["Main"]["GUI_RESIZABLE_X"]
        self.dct["GUI_RESIZABLE_Y"] =   cfg["Main"]["GUI_RESIZABLE_Y"]  
        self.dct["COMMON_LABEL"]    =   cfg["Main"]["COMMON_LABEL"]
        self.dct["ALL_LABEL"]       =   cfg["Main"]["ALL_LABEL"] 
                       
        self.dct["MenY"] =        int(cfg["Main"]["MenY"])
        self.dct["mova"] =        cfg["Main"]["mova"]
        self.dct["BAUDRATE"] =    int(cfg["Main"]["BAUDRATE"])
        self.dct["PARSER"] =    int(cfg["Main"]["PARSER"])
        
        return self.dct
    
    ###--- Сохранить config
    def saveConfig(self):
        cfg = cfprs.ConfigParser()
        cfg['Main'] = {'BAZE_X': self.dct["BAZE_X"],
                       'BAZE_Y': self.dct["BAZE_Y"],                       
                       'SHFT_X' : self.dct["SHFT_X"],
                       'SHFT_Y' : self.dct["SHFT_Y"],
                       'PADDING' : self.dct["PADDING"],                       
                       'GUI_RESIZABLE_X' : self.dct["GUI_RESIZABLE_X"],
                       'GUI_RESIZABLE_Y' : self.dct["GUI_RESIZABLE_Y"], 
                       'COMMON_LABEL' : self.dct["COMMON_LABEL"],
                       'ALL_LABEL' : self.dct["ALL_LABEL"], 
                        
                       'MenY' : self.dct["MenY"],
                       'mova': self.dct["mova"],
                       'BAUDRATE' : self.dct["BAUDRATE"],
                       'PARSER' : self.dct["PARSER"],
                      }    
                   
        with open(self.path, 'w') as configfile:
            cfg.write(configfile)
            
            
    def OutConfig(self):
        print("\nCurrent configuration\n")
        for el in  self.dct:
            print(f"{el:16}\t= {self.dct[el]}")
        
    def Get_sWIN(self):
        sWIN = str(self.dct["BAZE_X"])+"x"+str(self.dct["BAZE_Y"])
        sWIN += "+"+str(self.dct["SHFT_X"])+"+"+str(self.dct["SHFT_Y"])
        return sWIN
    
    def Get_GUI_RESIZABLE_X(self):
        return self.dct["GUI_RESIZABLE_X"]   

    def Get_GUI_RESIZABLE_Y(self):
        return self.dct["GUI_RESIZABLE_Y"]      
 
    def Get_BAZE_X(self):
        return self.dct["BAZE_X"]    

    def Get_BAZE_Y(self):
        return self.dct["BAZE_Y"]        
 
    def Get_PADDING(self):
        return self.dct["PADDING"]     
    
    def Get_COMMON_LABEL(self):
        return self.dct["COMMON_LABEL"]  
    
    def Get_ALL_LABEL(self):
        return self.dct["ALL_LABEL"]  

    def Get_Baud(self):
        return self.dct["BAUDRATE"]   
    
    def Get_Parser(self):
        return self.dct["PARSER"]  
    
def main():
    pass

if __name__ == "__main__":
    main()      