import ctypes
LIBC_PATH='/lib/x86_64-linux-gnu/libc.so.6'
libc = ctypes.CDLL(LIBC_PATH)  # under Linux/Unix
t = libc.time(None)            # equivalent C code: t = time(NULL)
print t
