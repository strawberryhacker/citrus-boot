import serial
import argparse
import math
import sys
import time

# Note: this is not an example of good python code. This is not the intention 
# either. I only want to touch python until I get something that works, and
# thats it!

class flasher:
    # Protocol defines
    START_BYTE = 0xAA
    END_BYTE   = 0x55
    POLYNOMIAL = 0xB2   # Used in the FCS check

    # Commands to c-boot
    CMD_WRITE_PAGE       = 0x04
    CMD_RESET            = 0x06

    # Progress bar customization. The bar prefix indicates the string which 
    # will be written before the progress bar
    BAR_LENGTH = 40
    BAR_PREFIX = "Downloading kernel "

    # Variables for holding the current download progress
    total = 1
    progress = 0

    # Why the hell do I need this...
    def __init__(self):
        pass
    
    # Ugly parser
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

    # Updates the progress bar on the screen using class variables
    def update_progress(self, suffix, error):
        percent = ("{0:.1f}").format(100 * (self.progress / float(self.total)))
        progress = int((self.progress * self.BAR_LENGTH) // self.total)
        bar = '█' * progress + ' ' * (self.BAR_LENGTH - progress)
        if error:
            print(f'\r{self.BAR_PREFIX} | \033[31m{bar}\033[0m | \033[31m{percent}% {suffix}\033[0m', end ="\r")
        else:
            print(f'\r{self.BAR_PREFIX} | {bar} | {percent}% {suffix}', end ="\r")
        if ((self.progress == self.total) or error):
            print(flush=True)

    # Opens the COM port with the port specified on the command line
    def serial_open(self):
        try:
            self.com = serial.Serial(port=self.com_port, baudrate=230400, timeout=0.2);
        except serial.SerialException as e:
            print(e)
            exit()

    # Closes the COM port
    def serial_close(self):
        self.com.close()

    # Flushes the COM port. This is due to the fact that the board might send 
    # charaters on the serial port during boot sequence. The COM port should 
    # therefore be flushed before starting the download 
    def serial_flush(self):
        self.com.flush();
    
    # Writes a byte on the serial interface
    def serial_write(self, data):
        self.com.write(bytes(data))

    # Reads the content of the file specified on the command line and returns
    # it in a variable. A whole file in a variable...
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
        
        # Get the response from the target, this might cause a timeout and
        # therefore a program exit
        return self.get_response()

    # Returns the response (8-bit) from c-boot. In case of timeout it prints 
    # the error message and quits the application
    def get_response(self):
        # Listen for the response. The timeout is specified in the init function
        resp = self.com.read(size = 1)

        if (len(resp) == 0):
            self.update_progress("Timeout occured", 1)
            sys.exit()
        return resp

    def check_resp(self, resp):
        if resp != b'\x06':
            string = "Response includes errors: " + hex(resp)
            self.update_progress(string, 1)
            sys.exit()

    # The reset of the bootloader involves simply sending the RESET packet with
    # no data. The board will respond with ASCII ACK when ready
    def reset_bootloader(self):
        status = self.send_frame(self.CMD_RESET, bytearray([]))
        self.check_resp(status)
    
    # If the kernel is running we have to make the board enter c-boot first.
    # This is done by sending ACK to the target. This should return ACK response
    def go_to_bootloader(self):
        self.com.write(b'\x06')
        response = self.get_response()
        self.check_resp(response)

    # Download the binary image page by page to the kernel
    def load_kernel(self):
        # Open the com port
        self.serial_open()

        # Read the binary from file
        kernel_binary = self.file_read()
        
        # In case the startup ack from c-boot is still in the buffer
        self.serial_flush()
        self.go_to_bootloader()
        self.reset_bootloader()

        # Write the kernel binary to the board in blocks of 512 bytes
        print("Connecting to c-boot")

        # Calculate the number of pages to write rounded towards inf
        length = len(kernel_binary)
        number_of_block = math.ceil(length / 512)
        self.total = number_of_block

        # Check if we have to send a ZLP on the end
        if (length % 512) == 0:
            self.total += 1
        self.progress = 0

        # Initialize the progress bar
        self.update_progress("Complete", 0)

        for i in range(number_of_block):
            binary_fragment = kernel_binary[i * 512 : (i + 1) * 512]

            status = self.send_frame(self.CMD_WRITE_PAGE, binary_fragment)
            self.check_resp(status)
            self.progress += 1
            self.update_progress("Complete", 0)

            # If the last page was short (not 512 bytes) the bootloader will 
            # automatically detect the end of file and start the kernel. If the
            # last page was exactly 512 bytes a short packet with no data will
            # be issued in order to mark the end of file
            if i == (number_of_block - 1):
                if len(binary_fragment) == 512:
                    print("Forcing short packet")
                    status = self.send_frame(self.CMD_WRITE_PAGE, bytearray([]))
                    self.check_resp(status)
                    self.progress += 1
                    self.update_progress("Complete", 0)


# Run the functions
test = flasher()
test.parser()
test.load_kernel()
