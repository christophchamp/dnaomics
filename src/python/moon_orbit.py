from math import *
import tkinter
tk = tkinter.Tk()
w = 1280
h = 800
c = tkinter.Canvas( tk, width=w, height=h )
c.pack()
km = .02 / 6.3

sunRad = 690*km
deltaE = 1.5*pow(10,5)*km
sun = (w/2, h/2- deltaE)
x = sun[0]
y = sun[1] + deltaE
c.create_oval(x-sunRad,y-sunRad,x+sunRad,y+sunRad,outline = "yellow", fill="yellow")


earthY = y + deltaE * sin(0)
earthX = x + deltaE * cos(0)
earthRad = 6.3*km
earth = c.create_oval(earthX - earthRad, earthY - earthRad,earthX + earthRad, earthY + earthRad,outline="blue",fill="blue")

deltaM = 400 * km
moonY = earthY + deltaM * sin(2*pi)
moonX = earthX + deltaM * cos(2*pi)
moonRad = 1.7*km
moon = c.create_oval(moonX - moonRad, moonY - moonRad, moonX + moonRad, moonY + moonRad)

days = 0
moonPos = 0
earthInc = 2*pi / (365.25)
moonInc = 2*pi / (29.5)
while days < 2*pi:
    days += earthInc
    moonPos += moonInc
    earthY = y + deltaE * sin(days)
    earthX = x + deltaE * cos(days)
    moonY = earthY + deltaM * sin(moonPos)
    moonX = earthX + deltaM * cos(moonPos)
    earth = c.create_oval(earthX - earthRad, earthY - earthRad,earthX + earthRad, earthY + earthRad,outline="blue",fill="blue")
    moon = c.create_oval(moonX - moonRad, moonY - moonRad, moonX + moonRad, moonY + moonRad)
    c.update()


tk.mainloop()
