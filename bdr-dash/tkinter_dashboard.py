from tkinter import *
from tkinter import ttk
import sys
import signal

import time

def current_milli_time():
    return round(time.time() * 1000)

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 480

BOTTOM_HEIGHT = 60
SIDEBAR_WIDTH = 160

CENTER_WIDTH = 360
CENTER_BOTTOM_HEIGHT = 320

QUIT_BUTTON_OFFSET = SCREEN_WIDTH - 95

global_root = None

# displays data returned by calling display_func, label updates every freq milliseconds
# displays it in middle of parent_frame
class DynamicLabel:
    def __init__(self, parent_frame, display_func, freq, font_config="Arial 16"):
        self.display_func = display_func
        self.freq = freq
        self.font_config = font_config
        self.parent_frame = parent_frame
        self.label = ttk.Label(parent_frame, font=font_config, text=self.display_func())
        self.label.place(relx=0.5, rely=0.5, anchor=CENTER)
        #self.label.pack(side="top", fill="both", expand="true", anchor="center")
        self.label.after(self.freq, self.update)
    def update(self):
        self.label.configure(text=self.display_func())
        self.label.after(self.freq, self.update)

# time elapsed display label
class TimeElapsedLabel(DynamicLabel):
    def __init__(self, parent_frame, freq, font_config="Arial 32"):
        self.start_time = current_milli_time()
        func = self.get_elapsed_time
        super().__init__(parent_frame, func, freq, font_config)
    def get_elapsed_time(self):
        cur_time = current_milli_time()
        dt = cur_time - self.start_time
        dt = int(dt/1000) # convert to seconds
        seconds = str(dt % 60).zfill(2)
        minutes = str(dt // 60).zfill(2)
        return(minutes + ":" + seconds)

def get_speed():
    return "Speed: [tbd]"

def get_charge():
    if not hasattr(get_charge, 'state'):
        get_charge.state = False
    get_charge.state = not get_charge.state
    return "Charge: 100" if get_charge.state else "Charge: 99"

def get_ready():
    return "Ready/Not Ready to Drive"

def get_regen():
    return "Regen On/Off"

def get_regen_scale():
    return "Regen Scale Here"

def get_voltage():
    return "Voltage Here"

def get_current_draw():
    return "Current Draw Here"

def get_temp_here():
    return "Battery temp here"

def get_acceleration():
    return "Accleration: [tbd] ms^-2"

def setup_gui():
    root = Tk()

    global_root = root

    root.geometry("{}x{}".format(SCREEN_WIDTH, SCREEN_HEIGHT))

    root.attributes("-fullscreen", True)

    
    s = ttk.Style()
    
    s.configure("Debug_Cyan.TFrame", background='cyan')
    s.configure("Debug_Magenta.TFrame", background='magenta')
    s.configure("Debug_Yellow.TFrame", background='yellow')
    s.configure("Bordered.TFrame", relief="ridge", borderwidth=10)
    s.configure("Bordered.TLabel", relief="solid", borderwidth=2)

    btm_frame = ttk.Frame(root, width=SCREEN_WIDTH, height=BOTTOM_HEIGHT)
    btm_frame.grid(column=0, row=1, columnspan=3, sticky="sw", padx=10, pady=5)

    # left sidebar - ready indicator & regen
    sidebar_l_frame = ttk.Frame(root, width=SIDEBAR_WIDTH, height=SCREEN_HEIGHT - BOTTOM_HEIGHT)
    sidebar_l_frame.grid(column=0, row=0, sticky="nw", padx=10, pady=10)
    #sidebar_l_frame.configure(style="Debug_Cyan.TFrame")

    # center frame - most important data
    center_frame = ttk.Frame(root, width=CENTER_WIDTH, height=SCREEN_HEIGHT - BOTTOM_HEIGHT)
    center_frame.grid(column=1, row=0, sticky="nw", padx=10, pady=10)
    #center_frame.configure(style="Debug_Magenta.TFrame")

    # right sidebar - BMS data
    sidebar_r_frame = ttk.Frame(root, width=SIDEBAR_WIDTH, height=SCREEN_HEIGHT - BOTTOM_HEIGHT)
    sidebar_r_frame.grid(column=2, row=0, sticky="nw", padx=10, pady=10)
    #sidebar_r_frame.configure(style="Debug_Yellow.TFrame")

    # bottom frame - error & quit
    l1 = ttk.Label(btm_frame, text="Put error message here")
    l1.place(x=0, y=0)
    # button just here for easier testing, in actual operation should disable this
    b1 = ttk.Button(btm_frame, text="Quit GUI", command=root.destroy)
    b1.place(x=QUIT_BUTTON_OFFSET, y=0)

    #time_label = DynamicLabel(center_frame, current_milli_time, 1)
    time_label_frame = ttk.Frame(center_frame, width=CENTER_WIDTH/2, height=SCREEN_HEIGHT - BOTTOM_HEIGHT - CENTER_BOTTOM_HEIGHT)
    time_label_frame.place(x=CENTER_WIDTH / 2, y=CENTER_BOTTOM_HEIGHT)
    time_label = TimeElapsedLabel(time_label_frame, 1)
    time_label_frame.configure(style="Bordered.TFrame")
    
    
    btnCharge = ttk.Frame(center_frame, width=CENTER_WIDTH/2, height= SCREEN_HEIGHT - BOTTOM_HEIGHT - CENTER_BOTTOM_HEIGHT)
    btnCharge.place(x=0, y=CENTER_BOTTOM_HEIGHT)
    btnChargeLabel = DynamicLabel(btnCharge, get_charge, 1)
    #btnChargeLabel.label.configure(style="Bordered.TLabel")
    btnCharge.configure(style="Bordered.TFrame")

    btnSpeed = ttk.Frame(center_frame, width=CENTER_WIDTH, height= CENTER_BOTTOM_HEIGHT)
    btnSpeed.place(x=0, y=0)
    btnSpeedLabel = DynamicLabel(btnSpeed, get_speed, 1)
    btnSpeed.configure(style="Bordered.TFrame")

    COMPONENT_HEIGHT = (SCREEN_HEIGHT - BOTTOM_HEIGHT ) / 4

    btnReady = ttk.Frame(sidebar_l_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnReady.place(x=0,y=0)
    btnReadyLabel = DynamicLabel(btnReady, get_ready, 1, "Arial 8")
    btnReady.configure(style="Bordered.TFrame")
    
    btnRegen = ttk.Frame(sidebar_l_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnRegen.place(x=0,y=COMPONENT_HEIGHT)
    btnRegenLabel = DynamicLabel(btnRegen, get_regen, 1, "Arial 8")
    btnRegen.configure(style="Bordered.TFrame")
    
    btnRegenScale = ttk.Frame(sidebar_l_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnRegenScale.place(x=0,y=COMPONENT_HEIGHT*2)
    btnRegenScaleLabel = DynamicLabel(btnRegenScale, get_regen_scale, 1, "Arial 8")
    btnRegenScale.configure(style="Bordered.TFrame")
    
    btnVoltage = ttk.Frame(sidebar_r_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnVoltage.place(x=0,y=0)
    btnVoltageLabel = DynamicLabel(btnVoltage, get_voltage, 1, "Arial 8")
    btnVoltage.configure(style="Bordered.TFrame")
    
    btnCurrent = ttk.Frame(sidebar_r_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnCurrent.place(x=0,y=COMPONENT_HEIGHT)
    btnCurrentLabel = DynamicLabel(btnCurrent, get_current_draw, 1, "Arial 8")
    btnCurrent.configure(style="Bordered.TFrame")
    
    btnBattery = ttk.Frame(sidebar_r_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnBattery.place(x=0,y=COMPONENT_HEIGHT*2)
    btnBatteryLabel = DynamicLabel(btnBattery, get_temp_here, 1, "Arial 8")
    btnBattery.configure(style="Bordered.TFrame")
    
    btnAccel = ttk.Frame(sidebar_r_frame, width=SIDEBAR_WIDTH, height=COMPONENT_HEIGHT)
    btnAccel.place(x=0,y=COMPONENT_HEIGHT*3)
    btnAccelLabel = DynamicLabel(btnAccel, get_acceleration, 1, "Arial 8")
    btnAccel.configure(style="Bordered.TFrame")

    '''
    #left column widgets
    btnReady = Button(text = "Ready to Drive", width = 10, height = 10)
    btnReady.grid(row = 0, column = 0, padx = 10, pady = 10, sticky = 'ns')

    btnRegen = Button(text = "Regen On", width = 10, height = 10)
    btnRegen.grid(row = 1, column = 0, padx = 10, pady = 10, sticky = 'ns')

    btnRegenScale = Button(text = "Regen Scale", width = 10, height = 10)
    btnRegenScale.grid(row = 2, column = 0, rowspan = 2, padx = 10, pady = 10, sticky = 'ns')

    #right column widgets
    btnVoltage = Button(text = "Voltage: ", width = 10, height = 10)
    btnVoltage.grid(row = 0, column = 3, padx = 10, pady = 10, sticky = 'nsew')

    btnCurrent = Button(text = "Current Draw: ", width = 10, height = 10)
    btnCurrent.grid(row = 1, column = 3, padx = 10, pady = 10, sticky = 'nsew')

    btnBattery = Button(text = "Battery Temp: ", width = 10, height = 10)
    btnBattery.grid(row = 2, column = 3, padx = 10, pady = 10, sticky = 'nsew')

    btnAccel = Button(text = "Acceleration: ", width = 10, height = 10)
    btnAccel.grid(row = 3, column = 3, padx = 10, pady = 10, sticky = 'nsew')
    '''
    root.mainloop()

def signal_handler(sig, frame):
    print("You pressed Ctrl+C!")
    if global_root is not None:
        global_root.destroy()
    sys.exit(0)


def main():
    signal.signal(signal.SIGINT, signal_handler)
    #signal.pause()
    setup_gui()

if __name__ == "__main__":
    main()
