# -----------------------------------------------------------------------------
# Matthew Triche
# ELEC5727
# Final Project
#
# This is the python interface for libtochee_cpu.so
# -----------------------------------------------------------------------------

from ctypes import *
import array
import os

MAX_SYMBOL_LIST = 128

class c_symbol(Structure):
	_fields_ = [("value", c_int),
                    ("corner_row", c_int*4),
                    ("corner_col", c_int*4),
                    ("center_row", c_int),
                    ("center_col", c_int)]

class Tochee:
	
	# ---------------------------------------------------------------------
	# __init__
	#
	# The constructor. A handle to the shared object is created within this
	# constructor.
	#
	# Parameters:
	# _bps - The number of bit-per-symbol used by the decoder.
	
	def __init__(self, _bps):
		
		self.bps     = _bps
		self.sym_num = pow(2,_bps) + 1
		
		lib_filename = os.getcwd() + "/libtochee_cpu.so"
		self.lib = cdll.LoadLibrary(lib_filename) # create handle to libtochee.so
		
		# set arguments passed to each function call within the library
		self.lib.lib_init.argtypes      = [c_int]
		self.lib.symdet_push.argtypes   = [c_void_p, c_int, c_int, POINTER(c_symbol)]
		self.lib.decoder_write.argtypes = [c_int]
		self.lib.decoder_read.argtypes  = [c_void_p, c_int]
		self.lib.encoder_write.argtypes = [c_void_p, c_int]
		self.lib.encoder_read.argtypes  = [c_void_p, c_int]
		
		# set the return type of each funtion within the library
		self.lib.symdet_push.restype     = c_int
		self.lib.decoder_write.restypes  = c_int
		self.lib.decoder_size.restypes   = c_int
		self.lib.decoder_read.restypes   = c_int
		self.lib.encoder_write.restypes  = c_int
		self.lib.encoder_read.restypes   = c_int
		
		self.lib.lib_init(_bps) # initialize the library
		
	# ---------------------------------------------------------------------
	# __del__
	#
	# This is the class destructor. It's called when an instance is garbage
	# collected by python. Before this instance is destroyed, all memory
	# allocated within libtochee.so must be freed.
	#
	# NOTE: This is part of managing the life-cycle of shared objects.
	
	def __del__(self):
		self.lib.lib_free() # ensure all memory is freed
		
	# ---------------------------------------------------------------------
	# symdet_push
	#
	# Push an image frame into the symbol detector. A list of any symbols
	# found within the frame shall be returned.
	#
	# Parameters:
	# frame - The image frame (numpy matrix, RGB image)
	#
	# Return Value:
	# A list of all detected symbols.
	
	def symdet_push(self, frame):
		rows, cols, ch = frame.shape
		sym_array = (c_symbol*MAX_SYMBOL_LIST)() # create symbol array
		ptr = cast(pointer(sym_array),POINTER(c_symbol)) # aquire a pointer to the symbol array
		N = self.lib.symdet_push(frame.ctypes.data_as(POINTER(c_void_p)), \
		                         rows,                                    \
		                         cols,                                    \
		                         ptr)
		return sym_array[0:N]

	# ---------------------------------------------------------------------
	# decoder_push
	#
	# Push a new symbol into the decoder.
	#
	# Parameters:
	# sym - The symbol.
	#
	# Return Value:
	# True if bits were generated. False otherwise.
	
	def decoder_write(self, sym):
		if self.lib.decoder_write(sym) == 1:
			return True
		else:
			return False
	
	# ---------------------------------------------------------------------
	# decoder_size
	#
	# Get the size of the bit-queue maintained by the decoder.
	#
	# Parameters:
	# sym - The symbol.
	#
	# Return Value:
	# The size of the bit-queue.
	
	def decoder_size(self):
		return self.lib.decoder_size()

	# ---------------------------------------------------------------------
	# decoder_read
	#
	# Read bytes from the bit-queue.
	#
	# Parameters:
	# n - The number of bytes to try reading.
	#
	# Return Value:
	# The list of bytes read.
	
	def decoder_read(self,n):
		buff = (c_ubyte*n)() # create buffer
		ptr = cast(pointer(buff),POINTER(c_void_p)) # aquire a pointer to the byte array
		N = self.lib.decoder_read(ptr, n)
		return buff[0:N]
	
	# ---------------------------------------------------------------------
	# decoder_clear
	#
	# Clear the decoder's bit-queue.
	
	def decoder_clear(self):
		self.lib.decoder_clear()
		
	# ---------------------------------------------------------------------
	# encoder_write
	#
	# Write data to the encoder.
	#
	# Parameters:
	# data - A list containing the data. It must contain byte values, only.
	#
	# Return Value:
	# The list of bytes read.
	
	def encoder_write(self,data):
		buff = array.array('B', data)
		addr, size = buff.buffer_info()
		return self.lib.encoder_write(addr, size)
		
	# ---------------------------------------------------------------------
	# encoder_read
	#
	# Read symbols from the encoder.
	#
	# Parameters:
	# n - The number of symbols to try reading.
	#
	# Return Value:
	# The list of symbols read.
	
	def encoder_read(self,n):
		symbols = (c_ubyte*MAX_SYMBOL_LIST)()
		ptr = cast(pointer(symbols),POINTER(c_void_p)) # aquire a pointer to the symbol array
		N = self.lib.encoder_read(ptr,n)
		return symbols[0:N]