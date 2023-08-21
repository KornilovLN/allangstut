#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 21:32:35 2023

@author: starmark
"""

import tkinter as tk
from tkinter import ttk
from tkinter import font
from tkinter.messagebox import showerror, showwarning, showinfo

#--- Menu -------------------------------------------------------------------- 
class MyMenu():
    def __init__(self,master,com):
        self.master = master
        self.com = com 
        
    def create_menu(self):     
        file_menu = tk.Menu(tearoff=0)    
        file_menu.add_command(label="Open port", background="darkgray", foreground="yellow", command=self.open_click)
        file_menu.add_command(label="Close port", background="darkgray", foreground="yellow", command=self.close_click)
        file_menu.add_command(label="Save XLS", background="darkgray", foreground="yellow",command=self.save_click)
        file_menu.add_command(label="Load Config", background="darkgray", foreground="yellow",command=self.load_config)    
        file_menu.add_command(label="Save Config", background="darkgray", foreground="yellow",command=self.save_config)          
        file_menu.add_separator()
        file_menu.add_command(label="Exit", background="darkgray", foreground="yellow",command=self.exit_click)
            
        edit_menu = tk.Menu(tearoff=0)    
        edit_menu.add_command(label="EditParams", background="darkgray", foreground="yellow",command=self.editParams_click)        
        edit_menu.add_command(label="Editconfig", background="darkgray", foreground="yellow",command=self.editConfig_click)
    
        view_menu = tk.Menu(tearoff=0)    
        view_menu.add_command(label="ViewStart", background="darkgray", foreground="yellow",command=self.viewStart_click)
            
        about_menu = tk.Menu(tearoff=0)  
        about_menu.add_command(label="Rules", background="darkgray", foreground="yellow",command=self.rules_click)
        about_menu.add_command(label="Owner", background="darkgray", foreground="yellow",command=self.owner_click)
        about_menu.add_separator()
        about_menu.add_command(label="Author", background="darkgray", foreground="yellow",command=self.author_click)   
            
        # Создано и передано как параметр main_menu
        main_menu = tk.Menu(tearoff=0) 
        main_menu.configure(bg = "gray")
        main_menu.add_cascade(label="File", background="darkgray", foreground="yellow",menu=file_menu)
        main_menu.add_cascade(label="Edit", background="darkgray", foreground="yellow",menu=edit_menu)
        main_menu.add_cascade(label="View", background="darkgray", foreground="yellow",menu=view_menu)
        main_menu.add_cascade(label="About", background="darkgray", foreground="yellow",menu=about_menu)
        #master.config(menu=main_menu) 
        return main_menu

    def finish(self):         
        self.com.Port_Close() 


        self.master.destroy()    # ручное закрытие окна и всего приложения
        print("Закрытие приложения")
        
        
    def exit_click(self):        
        self.finish()
    
    def open_click(self):  
        self.com.Port_Open()        
        
    def close_click(self):
        self.com.Port_Close()

        
    def save_click(self):
        pass
        '''        
        s="Save all after working program\n"
        showinfo(title="Save all panel:", message=s)
        '''
    
    def load_config(self):
        pass
        ''' 
        self.pem = self.cfg.loadConfig
        #self.cfgedit = self.create_ConfigEditor(self.pem)
        '''
        
    def save_config(self):
        pass
        '''
        self.cfg.saveConfig() 
        '''
    
    def editParams_click(self):
        s="Fragment program\n"
        s+="Edit parameters program"
        showinfo(title="Editor parameters panel:", message=s)
    def editConfig_click(self):
        s="Fragment program\n"
        s+="Edit config panel"
        showinfo(title="Editor config panel:", message=s)
        
    def viewStart_click(self):
        pass
    
    def author_click(self):
        s="Programmer\n"
        s+="Starmark LN\n"
        s+="e-mail: ln.starmark@ekatra.io\n"
        s+="e-mail: ln.starmark@gmail.com\n"
        s+="tel: +380 66 9805661"    
        showinfo(title="About author:", message=s)
        
    def owner_click(self):
        s="STARMARK DESIGN\n"
        s+="help center\n"
        s+="{v 1.01}\n"
        showinfo(title="Owner:", message=s)
        
    def rules_click(self):
        s = "The program is designed for quick deployment"
        s += "\nof a certain class of window applications"
        s += "\nfor practicing control of remote devices"
        s += "\nvia channels: RS-485, Wi-Fi, nRF24,..."
        showinfo(title="Forewarning:", message=s)     
    
def main():
    pass

if __name__ == "__main__":
    main()    