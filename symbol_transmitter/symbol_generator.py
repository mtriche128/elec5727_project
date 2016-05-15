import cv2 
import cv2 as cv
import numpy as np
import time
import os
import binascii
#linux cv import
#import cv2
#import cv2.cv as cv


#TODO turn these into arguments
#from sys import argv
#reference_dataset, testfile, refresh_rate = argv
reference_dataset = os.path.abspath("../media")
testfile = os.path.abspath("./test_data.txt")
#refresh_rate = 34 #fastest usable framerate
refresh_rate = 2000

#0 filled array for inserting in front of input strings
ZEROS = "0000000000000000000000000000000000000000"
start_symbol = 0
end_symbol = 33



## Read an input file and convert to encoded array
## input: Text file of binary values
## output: Completed list of encoded binary values representing transition images
#def read_file(file_name):
#    image_list = list()

#    # Start of message indicator. Begin transmitting the message
#    image_list.append(0)
    
#    # open the file
#    file = open(file_name)

#    # Parse file
#    for line in file:
#        # obtain the encoding sub-array for current line
#        # from when the input file was encoded as binary as text. (It's now ASCII)
#        image_list = gen_image_list(line.strip(),image_list)
    
#    # End of message indicator. No more data to transmit
#    image_list.append(33)

#    return image_list


# Determines the next symbols in the transition. 
# Takes in a 5 bit binary value represented as a string
# Takes in the list of transition images that have currently been generated 
# Returns the next transition symbol
def bin2symbol(bin_string, img_list):
    # Get the last (most recent) element from the list    
    prev_img_num = img_list[len(img_list)-1]

    # Convert the binary string to a number
    bin_val = int(bin_string,2)

    # previous symbol was not one of the reserved or control symbols
    if (prev_img_num < 33):
        if bin_val < prev_img_num:
            return bin_val
        else:
            return bin_val + 1
    # transition from a reserved or control symbol. Tansition and symbol number are the same. 
    else:
        return bin_val




# parses a binary value represented as a string and returns the transistion image array represented as numbers
# input: Binary value represented as a string. Only string lengths divisible by 5 allowed. 
# input: Current encoded list
# output: Encoded Transition list with added elements
def gen_image_list(bin_string, image_list):

    # Transverse the input binary and translate to symbol array
    # TODO: Handle non-divisible by 5 words
    for five_bits in range(int(len(bin_string)/5)):
        #print (bin_string[five_bits*5:five_bits*5+5])
        image_list.append( bin2symbol( bin_string[five_bits*5:five_bits*5+5] , image_list) )

    return image_list




# Takes in ASCII and convert to binary text. 0 fills high order bits to increments of 40 bits. 
# input: ASCII text
# output: binary in text representation
def parse_text(input_string):

    binary_input = bin(int(binascii.hexlify(input_string),16))

    # Strip off "0b" from the beginning
    binary_input = binary_input[2:]
        
    # restore the cut-off high order 0s of the input string
    mod_result = 8 - (len(binary_input) % 8)
    binary_input = ZEROS[:(mod_result)] + binary_input

    # 0 fill the string to make it divisible by 40
    mod_result = 40 - (len(binary_input) % 40)
    if mod_result != 40:
        binary_input = ZEROS[:(mod_result)] + binary_input

    return binary_input



# Takes in ASCII and displays the associated transition images
# input: ASCII text
def ascii_to_image_display(ascii_val):
        # Convert input to binary representation
        bin_string = parse_text(ascii_val)

        # Output results to console. 
        print (bin_string)
        print (len(bin_string))

        # Start symbol
        image_list = list()
        image_list.append(start_symbol); 

        # Pass the string to the output generator
        image_list = gen_image_list(bin_string, image_list)

        # End symbol
        image_list.append(end_symbol); 

        print("Transition list: ")
        print(image_list)

        # Print images to the screen
        output_image_list(image_list, refresh_rate)


# Run the program to parse console input
def keyboard_input():
    run = True
    while(run):
        user_input = raw_input("Enter the text you wish to transmit: ")
        ascii_to_image_display(user_input)
    


# Run the program to parse input file. Trreats each line as a new message
# input: file location
def file_input(file_name): 

    # open the file
    file = open(file_name)

    # Parse file. End of message sent at end of each line
    for line in file:
        ascii_to_image_display(line.strip())
    


# Outputs the images associated with the image list
# input: List of image names representing encoded transitions
# input: Time in ms for holding each image on the screen
def output_image_list(image_list, period):
    for image in image_list:
        image_name = str(image) + ".png"
        file_location = os.path.join(reference_dataset,image_name)
        image = cv.imread(file_location, 0)   # Read the file
        # Add boarder to the image
        bordered_img = cv2.copyMakeBorder(image,200,200,200,200,cv2.BORDER_CONSTANT,value=[255,255,255])
        cv.imshow("output", bordered_img )
        cv2.waitKey(period)
    cv2.destroyAllWindows()



####################################
######### Program Control ##########
####################################
user_input = 2
while( (user_input != 0) and (user_input != 1) ): 
    user_input = int(raw_input("Keyboard input (0) or File Input (1): "))
   
if (user_input == 0):
    keyboard_input()
    
else:
    file_input(testfile)
