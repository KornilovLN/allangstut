#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 19:18:23 2023

@author: starmark
"""

import tkinter as tk
from tkinter import ttk
from tkinter import font
from tkinter.messagebox import showerror, showwarning, showinfo

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
  

#--- построение панелей и виджетов -------------------------------------------
class PanelMethods():
    def __init__(self,padding): 
        self.PADDING = padding
        
        #--- Применим стили --------------------------------------------------
        self.style = ttk.Style()
        self.style.theme_use("clam")
        self.style.configure("TFrame",
                         background=COLOR_FRM["bg"], 
                         foreground=COLOR_FRM["fg"],
                       )
        self.style.configure("Treeview.Heading", 
                         font=('Verdana', LARGE_FONT),
                         background="gray", 
                         foreground="yellow",
                         rowheight = int(LARGE_FONT*2.5),
                       )                        
        self.style.configure("Treeview",
                         font=('Verdana', NOTEBOOK_FONTSIZE),
                         background = "silver",
                         foreground = "navy",
                         rowheight = int(LARGE_FONT*2.5),
                         fieldbackground = "silver",                
                       )
        self.style.map('Treeview', background=[('selected', 'purple')])        
        
        self.font_lab = font.Font(family= "Verdana", 
                             size=12, weight="normal", 
                             slant="roman", 
                             underline=False, overstrike=False)
          
    def Pan(self, mast, sid, fil, anch, wdth, hght, border):   
        frm = ttk.Frame(master=mast, style='TFrame', width=wdth, height=hght,                        
                        borderwidth=border, relief=tk.SOLID, padding=[self.PADDING, self.PADDING])
        frm.pack(side=sid, expand=True, fill=fil, anchor=anch) 
        frm.pack_propagate()
        return frm
    
    def Lab(self, mast, sid, fil, anch, name):    
        lab = tk.Label(master=mast, text=name, font=self.font_lab,
                       bg=COLOR_LAB["bg"], fg=COLOR_LAB["fg"],
                       bd=1, relief=tk.SUNKEN, anchor="c")   
        lab.pack(side=sid, expand=True, fill=fil) 
        return lab
    
    def create_BOTTOM(self, mast, anch, wdth, hght):
        frm = ttk.Frame(master=mast, style='TFrame', width=wdth, height=hght,
                        borderwidth=1,relief=tk.SOLID,padding=[0, 0])
        frm.pack(side=tk.BOTTOM, expand=False, fill=tk.Y,  anchor=anch)
        return frm   

    def create_FrameLab(self,mast,sid,fil,anch,title,w,h,colors,borders):
        frmlab = []
        pan_0 = self.Pan(mast,sid,fil,anch,w,h,borders[0])
        lab_0 = tk.Label(master=pan_0,text=title,font=self.font_lab,
                           bg=colors[0],fg=colors[1],bd=borders[1], 
                           relief=tk.SUNKEN, anchor="c") 
        lab_0.pack(side=tk.TOP, expand=True, fill=tk.X)
        pan_1 = self.Pan(pan_0,tk.TOP,tk.Y,"nw",w-4,h-4,borders[2]) 
        frmlab.append(pan_0)
        frmlab.append(lab_0)
        frmlab.append(pan_1)
        return frmlab
    
    def create_FrameEntry(self,mast,sid,expand,fil,anch,title,w,h,width_ent,colors,borders):
        pan_0 = self.Pan(mast,sid,fil,anch,w,h,borders[0])
        lab_0 = tk.Label(master=pan_0, text=title,font=self.font_lab, 
                         bg=colors[0],fg=colors[1],bd=borders[1], 
                         relief=tk.SUNKEN, anchor="w") 
        lab_0.pack(side=tk.LEFT, fill=tk.X)        
        ent_0 = tk.Entry(pan_0, width=width_ent)
        ent_0.pack(side=tk.LEFT, fill=tk.X)
        return ent_0
    
    def create_Button(self,mast,cmd,sid,expand,fil,anch,title,w,h,borders):
        pan_0 = self.Pan(mast,sid,fil,anch,w,h,borders[0])
        button_0 = ttk.Button(pan_0, text=title, command=cmd)
        button_0.pack(side=tk.TOP, expand=expand, fill=tk.X, anchor= "c")
        return button_0
    
    def create_Viewer(self,mast,cmd,sid,expand,fil,anch,title,text_button,w,h,colors,borders):
        pan_0 = self.Pan(mast,sid,fil,anch,w,h,borders[0])
        lab_0 = tk.Label(master=pan_0,text=title, 
                              font=self.font_lab, 
                              bg=colors[0],fg=colors[1],bd=borders[1],
                              relief=tk.SUNKEN, anchor="c") 
        lab_0.pack(side=tk.TOP, expand=expand, fill=tk.X)      
                
        frm_C1 = ttk.Frame(master=pan_0, style='TFrame',                        
                         borderwidth=borders[1], relief=tk.SOLID, 
                         height=25, 
                         padding=[8, 8])
        frm_C1.pack(side=tk.TOP, expand=expand, fill=tk.Y, anchor=anch)  
        
        pan_1 = self.Pan(pan_0,tk.TOP,tk.X,anch,w,h,borders[2])
        
        viewer_button = ttk.Button(pan_1, text=text_button, command=cmd)
        viewer_button.pack(side=tk.RIGHT, fill=tk.X)
        return viewer_button
        
        
    
    def create_statusbar(self, mast, metavar, metavar2):
        self.statusbar = tk.Label(mast, textvariable=metavar, 
                                  bg=COLOR_STAT["bg"], fg=COLOR_STAT["fg"], 
                                  bd=0, relief=tk.SUNKEN, anchor=tk.SW)
        self.statusbar.pack(side=tk.LEFT, fill=tk.X) 
        
        zazor = tk.Label(mast, text= "        ", 
                         bg=COLOR_STAT["bg"], fg=COLOR_STAT["fg"],
                         bd=0, relief=tk.SUNKEN, anchor=tk.SW)
        zazor.pack(side=tk.LEFT, fill=tk.X)  
        
        self.statusbar2 = tk.Label(mast, textvariable=metavar2,
                                   bg=COLOR_STAT["bg"], fg=COLOR_STAT["fg"],
                                   bd=0, relief=tk.SUNKEN, anchor=tk.SW)
        self.statusbar2.pack(side=tk.LEFT, fill=tk.X) 
        
        ender = tk.Label(mast, text= " "*500, 
                         bg=COLOR_STAT["bg"], fg=COLOR_STAT["fg"],
                         bd=0, relief=tk.SUNKEN, anchor=tk.SW)
        ender.pack(side=tk.LEFT, fill=tk.X)
        
    def create_Text(self, mast, name, w, h, b_g, f_g):        
        lab = tk.Label(master=mast, text=name, font=self.font_lab, 
                       bg=COLOR_LAB["bg"], fg=COLOR_LAB["fg"], 
                       bd=1, relief=tk.SUNKEN, anchor="c") 
        lab.pack(side=tk.TOP, fill=tk.X)
        
        frm = ttk.Frame(master=mast, style='TFrame',                        
                        borderwidth=0, relief=tk.SOLID, padding=[8, 8])
        frm.pack(side=tk.TOP, expand=True, fill=tk.BOTH, anchor="nw") 

        txt = tk.Text(master=frm,width=w,height=h,bg=b_g,fg=f_g,wrap=tk.WORD)
        txt.pack(side=tk.LEFT, fill=tk.BOTH)     
        
        scroll_bar = tk.Scrollbar(frm,orient='vertical')
        scroll_bar.pack(side=tk.RIGHT,fill='y',  anchor="ne")
        txt.configure(yscrollcommand=scroll_bar.set)
        scroll_bar.config(command=txt.yview)        
        return txt
    
    def create_VidVar(self, mast, n, name, var, sid, fil, anch, wdth, hght, border, nstyle):  
        bgclr = [["darkolivegreen", "dimgray", "darkolivegreen"],
                 ["darkgoldenrod4", "dimgray", "darkgoldenrod4"]]
        
        fgclr = [["khaki1", "white", "khaki1"],
                 ["antiquewhite1", "white", "antiquewhite1"]]
        
        frm = ttk.Frame(master=mast, style='TFrame', width=wdth, height=hght,                        
                        borderwidth=border, relief=tk.SOLID, 
                        padding=[self.PADDING/2, self.PADDING/2])
        frm.pack(side=sid, expand=True, fill=fil, anchor=anch) 
        frm.pack_propagate()
        
        labN = tk.Label(master=frm, text=str(n), font=self.font_lab, 
                       bg=bgclr[nstyle][0], fg=fgclr[nstyle][0],
                       width = 4,                                          
                       bd=1, relief=tk.SUNKEN, anchor="nw") 
        labN.pack(side=tk.LEFT, fill=tk.X)
        
        lab = tk.Label(master=frm, text=" "+name, font=self.font_lab, 
                       bg=bgclr[nstyle][1], fg=fgclr[nstyle][1],
                       width = 16,                                          
                       bd=0, relief=tk.SUNKEN, anchor="nw") 
        lab.pack(side=tk.LEFT, fill=tk.X)
        
        labvar = tk.Label(master=frm, textvariable=var, font=self.font_lab, 
                       bg=bgclr[nstyle][2], fg=fgclr[nstyle][2], 
                       width = 10, 
                       bd=1, relief=tk.SUNKEN, anchor="ne") 
        labvar.pack(side=tk.LEFT, fill=tk.X)        
        return frm
    
    
    
def main():
    pass

if __name__ == "__main__":
    main()    