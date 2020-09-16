import serial
import argparse
import math
import sys
import time
   

class flasher:
    # Download protocol defines
    START_BYTE = 0xAA
    END_BYTE   = 0x55
    POLYNOMIAL = 0xB2

    # Commands to c-boot
    CMD_ERASE_FLASH      = 0x03
    CMD_WRITE_PAGE       = 0x04
    CMD_WRITE_PAGE_LAST  = 0x05
    CMD_SET_FLASH_OFFSET = 0x06
    CMD_CTRL_LED         = 0x07

    # Why the hell do I need this...
    def __init__(self):
        pass
    
    def parser(self):
        parser = argparse.ArgumentParser(description="Chip flasher");

        parser.add_argument("-c", "--com_port",
                            help="Specify the com port COMx or /dev/ttySx")

        parser.add_argument("-f", "--file",
                            required=True,
                            help="Specifies the binary")

        args = parser.parse_args()

        self.file = args.file;
        self.com_port = args.com_port;

    # Opens the COM port with the command line option and stores the object in
    # the class
    def serial_open(self):
        try:
            self.com = serial.Serial(port=self.com_port, baudrate=9600, timeout=1);
        except serial.SerialException as e:
            print(e)
            exit()

    # Closes the COM port
    def serial_close(self):
        self.com.close()
    
    # Writes a byte on the serial interface
    def serial_write(self, data):
        self.com.write(bytes(data))

    # Reads the content of the file specified on the command line and returns
    # it as plain text
    def file_read(self):
        f = open(self.file, "rb")
        return f.read()

    # The c-boot frame interface embeds a simple FCS check at the end of each 
    # frame. This functions takes in the frame payload including the command 
    # and the size and returns the 8-bit FCS checksum
    def calculate_fcs(self, data):
        crc = 0
        for i in range(len(data)):
            crc = crc ^ data[i]
            for j in range(8):
                if crc & 0x01:
                    crc = crc ^ self.POLYNOMIAL
                crc = crc >> 1
        return crc

    # Packs and sends one frame to c-boot
    def send_frame(self, cmd, payload):

        payload_size = len(payload)

        # Start fragment consists of a 8-bit start byte, and 8-bit cmd and 
        # a 16-bit size in little-endian
        start_byte = bytearray([self.START_BYTE])
        cmd_byte   = bytearray([cmd])
        size       = bytearray([payload_size & 0xFF, (payload_size >> 8) & 0xFF])
        data       = bytearray(payload)

        # End fragment consist of payload crc
        fcs = bytearray([self.calculate_fcs(cmd_byte + size + payload)])
        end_byte = bytearray([self.END_BYTE])

        self.com.write(start_byte + cmd_byte + size + data + fcs + end_byte)

        # Get the response from the target, this might also cause a timeout
        # and therefore a program exit
        return self.get_response()

    # Returns the response (8-bit) from c-boot
    def get_response(self):
        # Listen for the response
        resp = self.com.read(size = 1)

        if (len(resp) == 0):
            print("Timeout occured")
            sys.exit()
        return resp

    def load_kernel(self):
        # Open the com port
        self.serial_open()

        print("Reading kernel info")

        # Read the binary from file
        kernel_binary = self.file_read()

        print("Instructing the bard to enter c-boot")

        # If the kernel is running we have to make the board enter c-boot first.
        # This is done by sending 0 to the target. This should return OK response
        self.com.write(b'\x00')
        response = self.get_response()
        if response != b'\x00':
            print("Response includes errors: ", response)
            sys.exit()

        # Write the kernel binary to the board in blocks of 512 bytes
        print("c-boot started...")
        
        # Calculate the number of pages to write
        length = len(kernel_binary)
        number_of_block = math.ceil(length / 512)

        print("Downloading kernel...")
        for i in range(number_of_block):
            print("Writing page ", i, " of ", number_of_block)
            binary_fragment = kernel_binary[i*512:(i+1)*512]
            cmd = 0 
            if i == (number_of_block - 1):
                # This is the last block
                cmd = self.CMD_WRITE_PAGE_LAST
            else:
                cmd = self.CMD_WRITE_PAGE
                
            status = self.send_frame(cmd, binary_fragment)

            if response != b'\x00':
                print("Response includes errors: ", response)
                sys.exit()

        print("Kernel download complete!")

# Run the functions
test = flasher()
test.parser()
test.load_kernel()