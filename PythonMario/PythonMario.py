import sys
import asyncio
from bleak import BleakClient
import pygame
from pygame.locals import *
import win32con
import win32gui

pygame.init()
try:
	joystick = pygame.joystick.Joystick(0)
	joystick.init()
	print('Joystick Name:', joystick.get_name())
	print(' button num:', joystick.get_numbuttons())
except pygame.error:
	print('Joystick not found')
	sys.exit()

screen = pygame.display.set_mode((320, 320))
hwnd = pygame.display.get_wm_info()['window']
win32gui.SetWindowPos(hwnd, win32con.HWND_TOPMOST, 0, 0, 0, 0, win32con.SWP_NOMOVE|win32con.SWP_NOSIZE)

#address = "24:62:AB:D9:DD:B2"	# OV2640
address = "44:17:93:89:FE:9E"	# OV3660
UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

async def run(address, loop):
	async with BleakClient(address, loop=loop) as client:
		x = await client.is_connected()
		print('BLE Connected: {0}'.format(x))

		ws = w = b'0'
		lv = rv = 0
		active = True
		while active:
			event = pygame.event.get()
			if event == []:
				pygame.time.wait(100)
			else:
				for e in event:
					if e.type == QUIT:
						active = False
						break
					if e.type == pygame.locals.JOYAXISMOTION:
						ly_axis = joystick.get_axis(1)
						rx_axis = joystick.get_axis(2)
						lv = int(min(abs(ly_axis)*7.0, 7.0))
						rv = int(min(abs(rx_axis)*7.0, 7.0))
						if ly_axis < 0.0:
							lv |= 0x08
						if rx_axis < 0.0:
							rv |= 0x08
						w = ( (lv<<4)|rv ).to_bytes(1,'big')
					if ws != w:
						print(w.hex())
						await client.write_gatt_char(UUID, w)
						ws = w

loop = asyncio.get_event_loop()
loop.run_until_complete(run(address, loop))
