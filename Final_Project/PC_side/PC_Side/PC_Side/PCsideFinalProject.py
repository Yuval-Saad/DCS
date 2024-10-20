import serial as ser
import PySimpleGUI as sg
from enum import Enum
import threading
import os
from queue import Queue


class State(Enum):
    Exit = 0
    Main_Menu = 1
    Manual_Menu = 2
    Rotate = 3
    JoyStick_Control = 4
    Write = 5
    Delete = 6
    Neutral = 7
    Calibration_Menu = 8
    During_Calibration = 9
    Script_Menu = 10


currState = State.Main_Menu

s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS, parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
               timeout=1)

JoyStickMiddleX = 511
JoyStickMiddleY = 511
PB_click = False
Ack = False
StepsValid = False
ReceivedMSG = [0, 0, 0, 0]
ReceivedPOS = 0
posValid = False
ReceivedMiddleAndSteps = [0, 0, 0, 0, 0, 0]
fileNames = ["", "", ""]
numFile = 0
namesValid = False
q = Queue()
s_phi = 1.7
s_numOfSteps = 2043
#change
BeforeCalibration = True


def uart_reader():
    global PB_click, Ack, q, ReceivedMSG, StepsValid, ReceivedMiddleAndSteps
    global fileNames, numFile, namesValid, ReceivedPOS, posValid
    stepsPos = 0
    msgPos = 0
    posPos = 0
    while True:
        if s.in_waiting > 0:
            byte_data = s.read(1)  # Read one byte
            byte_value = byte_data[0]  # Get the integer value of the byte
            if byte_value == 0x01 and (
                    currState == State.Write or currState == State.Delete or currState == State.Neutral):
                PB_click = True
            elif byte_value == 0x02 and currState == State.Script_Menu:
                Ack = True
            elif currState == State.Write or currState == State.Delete or currState == State.Neutral:
                if byte_value == 0x07:
                    msgPos = 0
                else:
                    ReceivedMSG[msgPos] = byte_value
                    msgPos = msgPos + 1
                    if msgPos == 4:
                        x = (ReceivedMSG[0] - 150) * 100 + (ReceivedMSG[1] - 150)
                        y = (ReceivedMSG[2] - 150) * 100 + (ReceivedMSG[3] - 150)
                        q.put((x, y))
            elif currState == State.During_Calibration:
                ReceivedMiddleAndSteps[stepsPos] = byte_value
                stepsPos = stepsPos + 1
                if stepsPos == 6:
                    StepsValid = True
                    stepsPos = 0
            elif currState == State.Script_Menu:
                if byte_value == 0x03:
                    numFile = 0
                elif byte_value == 0x04:
                    numFile = 1
                elif byte_value == 0x05:
                    numFile = 2
                elif byte_value < 0x7B:
                    fileNames[numFile] += chr(byte_value)
                    print(byte_value)
                    print(numFile)
                    if byte_value == 0 and numFile == 2:
                        namesValid = True
                        stepsPos = 0
                else:
                    if posPos == 0:
                        ReceivedPOS = (byte_value - 150) * 100
                        posPos = 1
                    elif posPos == 1:
                        ReceivedPOS = ReceivedPOS + (byte_value - 150)
                        posPos = 0
                        posValid = True
            else:
                pass


def DecodeAndSendFile(values):
    # Send the decoded content via UART
    decoded_content = values['decoded_content']
    encoded_content = decoded_content.encode('utf-8')  # Encode the string as bytes
    for byte in encoded_content:
        SentToMCU(byte)
    SentToMCU(0x03)  # End of content sign


def Opcode(first_word):
    if first_word == "inc_lcd":
        return "01"
    if first_word == "dec_lcd":
        return "02"
    if first_word == "rra_lcd":
        return "03"
    if first_word == "set_delay":
        return "04"
    if first_word == "clear_lcd":
        return "05"
    if first_word == "stepper_deg":
        return "06"
    if first_word == "stepper_scan":
        return "07"
    if first_word == "sleep":
        return "08"
    return "00"  # Default value for unknown opcode


def MakeHex(num):
    hex_string = hex(int(num))[2:]
    return hex_string.zfill(2).upper()  # Pad with zero if needed and return in uppercase


def DecodeLine(line):
    words = line.split(maxsplit=1)  # Split only on the first space
    HexNums = []
    if not words:
        return "", []
    first_word = words[0]
    rest_of_words = []
    if len(words) > 1:
        rest_of_words = words[1].split(',')
    Op = Opcode(first_word)
    for num in rest_of_words:
        HexNums.append(MakeHex(num))
    return Op, "".join(HexNums)


def decodeFile(file_content):
    decoded_lines = []
    for line in file_content.splitlines():
        opcode, hexnums = DecodeLine(line)
        decoded_lines.append(f"{opcode}{hexnums}")
    return "\n".join(decoded_lines)


def SentToMCU(charValToSend):
    s.write(bytes([charValToSend]))


def Main_Menu():
    global currState, fileNames, namesValid, BeforeCalibration
    layout = [
        [sg.Push(),
         sg.Text('\u00A9 2024 Arama Yehonatan\nSaid Yaacov Yuval', text_color='#D3D3D3', font=("Helvetica", 8),
                 justification='right', pad=(10, 0))],
        [sg.Text('Main Menu', justification='center', font=("Helvetica", 16), expand_x=True, pad=(0, 10))],
        [sg.Text('Please choose an operation', justification='center', expand_x=True, font=("Helvetica", 12))],
        [sg.Push(), sg.Button('Manual Control', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12), disabled=BeforeCalibration),
         sg.Push()],
        [sg.Push(), sg.Button('Painter', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12), disabled=BeforeCalibration), sg.Push()],
        [sg.Push(), sg.Button('Calibration', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12)),
         sg.Push()],
        [sg.Push(), sg.Button('Script', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12), disabled=BeforeCalibration), sg.Push()],
        [sg.VPush()],
        [sg.Push(), sg.Button('Exit', size=(10, 1), font=("Helvetica", 12), pad=((0, 10), (10, 10)))]
    ]
    window = sg.Window('Final Project DCS', layout, size=(600, 400))
    event, values = window.read()
    if event == sg.WINDOW_CLOSED or event == 'Exit':
        currState = State.Exit
    if event == 'Manual Control':
        currState = State.Manual_Menu
        SentToMCU(0x02)
    if event == 'Painter':
        currState = State.Write
        SentToMCU(0x09)
    if event == 'Calibration':
        BeforeCalibration = False
        currState = State.Calibration_Menu
        SentToMCU(0x04)
    if event == 'Script':
        namesValid = False
        fileNames = ["", "", ""]
        currState = State.Script_Menu
        SentToMCU(0x05)
    # Close the window
    window.close()


def Manual():
    global currState
    layout = [
        [sg.Push(),
         sg.Text('\u00A9 2024 Arama Yehonatan\nSaid Yaacov Yuval', text_color='#D3D3D3', font=("Helvetica", 8),
                 justification='right', pad=(10, 0))],
        [sg.Text('Manual Menu', justification='center', font=("Helvetica", 16), expand_x=True, pad=(0, 10))],
        [sg.Text('Please choose an operation', justification='center', expand_x=True, font=("Helvetica", 12))],
        [sg.Push(), sg.Button('Rotate', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12),
                              button_color=('white', 'green')), sg.Push()],
        [sg.Push(), sg.Button('JoyStick Control', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12),
                              button_color=('white', 'green')),
         sg.Push()],
        [sg.VPush()],
        [sg.Push(), sg.Button('Back', size=(10, 1), font=("Helvetica", 12), pad=((0, 10), (10, 10)))]
    ]
    window = sg.Window('Final Project DCS', layout, size=(600, 400))
    while True:
        event, values = window.read()
        if event == sg.WINDOW_CLOSED:
            currState = State.Exit
            break
        if event == 'JoyStick Control':
            if currState == State.Manual_Menu:
                SentToMCU(0x06)
                window['JoyStick Control'].update(button_color=('white', 'blue'))
                window['Rotate'].update(button_color=('white', 'red'), disabled=True)
                currState = State.JoyStick_Control
            elif currState == State.JoyStick_Control:
                SentToMCU(0x08)
                window['JoyStick Control'].update(button_color=('white', 'green'))
                window['Rotate'].update(button_color=('white', 'green'), disabled=False)
                currState = State.Manual_Menu
        elif event == 'Rotate':
            if currState == State.Manual_Menu:
                SentToMCU(0x07)
                window['JoyStick Control'].update(button_color=('white', 'red'), disabled=True)
                window['Rotate'].update(button_color=('white', 'blue'))
                currState = State.Rotate
            elif currState == State.Rotate:
                SentToMCU(0x08)
                window['JoyStick Control'].update(button_color=('white', 'green'), disabled=False)
                window['Rotate'].update(button_color=('white', 'green'))
                currState = State.Manual_Menu
        elif event == 'Back':
            SentToMCU(0x01)
            currState = State.Main_Menu
            break

        window.refresh()  # Force GUI to redraw

    window.close()


def Painter():
    global currState
    global PB_click, Ack, q, ReceivedMSG
    global JoyStickMiddleX, JoyStickMiddleY
    previous_point = None
    resolutionX = JoyStickMiddleX * 2
    resolutionY = JoyStickMiddleY * 2
    cursor = None
    continuousPosX = JoyStickMiddleX
    continuousPosY = JoyStickMiddleY

    scale_x = 400 / resolutionX
    scale_y = 400 / resolutionY

    layout = [
        [sg.Text('Painter', justification='center', font=("Helvetica", 16))],
        [sg.Button('Continuous Mode', key='-CONTINUOUS-', size=(15, 1), font=("Helvetica", 12))],
        [sg.Graph(
            canvas_size=(400, 400),
            graph_bottom_left=(0, 400),
            graph_top_right=(400, 0),
            key='-GRAPH-',
            background_color='white'
        )],
        [sg.Text('Mode: Neutral', key='-MODE-', font=("Helvetica", 12))],
        [sg.Button('Back', size=(10, 1), font=("Helvetica", 12))]
    ]

    window = sg.Window('Final Project DCS', layout, finalize=True)
    graph = window['-GRAPH-']
    mode_text = window['-MODE-']
    mode_text.update('Mode: Write')

    continuous_mode = False  # Variable to track the continuous mode

    while True:
        event, values = window.read(timeout=1)  # Add a timeout to periodically check the queue
        if event == sg.WINDOW_CLOSED or event == 'Back':
            SentToMCU(0x01)
            currState = State.Main_Menu
            break
        elif event == '-CONTINUOUS-':
            continuous_mode = not continuous_mode  # Toggle the continuous mode
            if continuous_mode:
                window['-CONTINUOUS-'].update('Continuous Mode: ON')
                graph.erase()
                q = Queue()
                continuousPosX = JoyStickMiddleX
                continuousPosY = JoyStickMiddleY
                previous_point = None
            else:
                window['-CONTINUOUS-'].update('Continuous Mode: OFF')
                graph.erase()
                q = Queue()
                previous_point = None

        # Check for UART messages
        if PB_click:
            PB_click = False
            if currState == State.Write:
                mode_text.update('Mode: Delete')
                currState = State.Delete
            elif currState == State.Delete:
                mode_text.update('Mode: Neutral')
                currState = State.Neutral
            else:  # currState == State.Delete:
                mode_text.update('Mode: Write')
                currState = State.Write
        while not q.empty():
            (x, y) = q.get()
            if not continuous_mode:
                scaled_x = x * scale_x
                scaled_y = y * scale_y
            if continuous_mode:
                if x > JoyStickMiddleX and continuousPosX < JoyStickMiddleX * 2:
                    if (x - JoyStickMiddleX) > 300:
                        continuousPosX += 3
                    elif (x - JoyStickMiddleX) > 230:
                        continuousPosX += 2.7
                    elif (x - JoyStickMiddleX) > 160:
                        continuousPosX += 2.3
                    elif (x - JoyStickMiddleX) > 90:
                        continuousPosX += 2
                    elif (x - JoyStickMiddleX) > 20:
                        continuousPosX += 1
                elif x < JoyStickMiddleX and continuousPosX > 0:
                    if(JoyStickMiddleX - x) > 300:
                        continuousPosX -= 3
                    elif(JoyStickMiddleX - x) > 230:
                        continuousPosX -= 2.7
                    elif (JoyStickMiddleX - x) > 160:
                        continuousPosX -= 2.3
                    elif (JoyStickMiddleX - x) > 160:
                        continuousPosX -= 2
                    elif (JoyStickMiddleX - x) > 20:
                        continuousPosX -= 1
                if y > JoyStickMiddleY and continuousPosY < JoyStickMiddleY * 2:
                    if (y - JoyStickMiddleY) > 300:
                        continuousPosY += 3
                    elif (y - JoyStickMiddleY) > 230:
                        continuousPosY += 2.7
                    elif (y - JoyStickMiddleY) > 160:
                        continuousPosY += 2.3
                    elif (y - JoyStickMiddleY) > 90:
                        continuousPosY += 2
                    elif (y - JoyStickMiddleY) > 20:
                        continuousPosY += 1
                elif y < JoyStickMiddleY and continuousPosY > 0:
                    if (JoyStickMiddleY - y) > 300:
                        continuousPosY -= 3
                    elif (JoyStickMiddleY - y) > 230:
                        continuousPosY -= 2.7
                    elif (JoyStickMiddleY - y) > 160:
                        continuousPosY -= 2.3
                    elif (JoyStickMiddleY - y) > 90:
                        continuousPosY -= 2
                    elif (JoyStickMiddleY - y) > 20:
                        continuousPosY -= 1
                scaled_x = continuousPosX * scale_x
                scaled_y = continuousPosY * scale_y
            if previous_point is not None:
                if currState == State.Write:
                    graph.draw_line(previous_point, (scaled_x, scaled_y), color='black', width=3)
                elif currState == State.Delete:
                    graph.draw_line(previous_point, (scaled_x, scaled_y), color='white', width=20)
                else:  # currState == State.Delete:
                    pass
            if cursor is not None:
                graph.delete_figure(cursor)
            cursor = graph.draw_circle((scaled_x, scaled_y), radius=3, fill_color='red', line_color='red')
            previous_point = (scaled_x, scaled_y)

    window.close()


def Calibration():
    global JoyStickMiddleX, JoyStickMiddleY
    global currState, s_phi, s_numOfSteps
    global StepsValid, ReceivedMiddleAndSteps
    phi = None
    numOfSteps = None

    layout = [
        [sg.Push(),
         sg.Text('\u00A9 2024 Arama Yehonatan\nSaid Yaacov Yuval', text_color='#D3D3D3', font=("Helvetica", 8),
                 justification='right', pad=(10, 0))],
        [sg.Text('Calibration Menu', justification='center', font=("Helvetica", 16), expand_x=True, pad=(0, 10))],
        [sg.Text('Note - when calibrating make sure joystick is on middle', size=(40, 1), text_color='#CCCCCC',
                 font=("Helvetica", 7))],
        [sg.Push(), sg.Button('Start', size=(20, 1), pad=((10, 10), (10, 10)), font=("Helvetica", 12),
                              button_color=('white', 'green')), sg.Push()],
        [sg.Text('Number of Steps:', size=(20, 1)), sg.Text('', key='numOfSteps', size=(10, 1))],
        [sg.Text('Phi:', size=(20, 1)), sg.Text('', key='phi', size=(10, 1))],
        [sg.VPush()],
        [sg.Push(), sg.Button('Back', size=(10, 1), font=("Helvetica", 12), pad=((0, 10), (10, 10)))]
    ]
    window = sg.Window('Final Project DCS', layout, finalize=True)

    while True:
        event, values = window.read(timeout=10)  # Add a timeout to periodically check the queue
        if event == sg.WINDOW_CLOSED or event == 'Back':
            SentToMCU(0x01)
            currState = State.Main_Menu
            break
        # Check for UART messages
        if event == 'Start':
            currState = State.During_Calibration
            window['Start'].update(button_color=('white', 'blue'), disabled=True)
            SentToMCU(0x0B)
        if StepsValid:
            JoyStickMiddleX = (ReceivedMiddleAndSteps[0] - 150) * 100 + (ReceivedMiddleAndSteps[1] - 150)
            JoyStickMiddleY = (ReceivedMiddleAndSteps[2] - 150) * 100 + (ReceivedMiddleAndSteps[3] - 150)
            currState = State.Calibration_Menu
            numOfSteps = (ReceivedMiddleAndSteps[4] - 150) * 100 + (ReceivedMiddleAndSteps[5] - 150)
            phi = 360 / numOfSteps
            s_numOfSteps = numOfSteps
            s_phi = phi
            StepsValid = False
            window['Start'].update(button_color=('white', 'green'), disabled=False)
            window['numOfSteps'].update(numOfSteps)
            window['phi'].update(phi)

    window.close()


def Script():
    global currState, s_numOfSteps
    global Ack, namesValid, fileNames, ReceivedPOS, posValid

    # Initialize the list with a placeholder to avoid empty list error
    opened_files = ['No files opened yet']
    file_paths = {}  # Dictionary to store file paths with their names

    layout = [
        [sg.Text(''), sg.Input(default_text='Open File', size=(40, 1), key='file_input'),
         sg.FileBrowse(key='file_browse'),
         sg.Button('Open', size=(10, 1), font=("Helvetica", 12), pad=((0, 10), (0, 0))),
         sg.Text('Degree:', size=(15, 1), font=("Helvetica", 25, "bold"), text_color='#38CB82', key='degree_text', visible=False)],  # Degree text
        [sg.Text('Opened Files:'),
         sg.OptionMenu(values=opened_files, size=(40, 1), key='file_list', enable_events=True)],
        [sg.Text('Slot 0:', size=(10, 1)), sg.Text('', size=(30, 1), key='slot0'),
         sg.Button('Load 0', size=(10, 1), font=("Helvetica", 12), key='load_slot0'),
         sg.Button('Execute 0', size=(10, 1), font=("Helvetica", 12), key='execute_slot0')],
        [sg.Text('Slot 1:', size=(10, 1)), sg.Text('', size=(30, 1), key='slot1'),
         sg.Button('Load 1', size=(10, 1), font=("Helvetica", 12), key='load_slot1'),
         sg.Button('Execute 1', size=(10, 1), font=("Helvetica", 12), key='execute_slot1')],
        [sg.Text('Slot 2:', size=(10, 1)), sg.Text('', size=(30, 1), key='slot2'),
         sg.Button('Load 2', size=(10, 1), font=("Helvetica", 12), key='load_slot2'),
         sg.Button('Execute 2', size=(10, 1), font=("Helvetica", 12), key='execute_slot2')],
        [sg.Column([
            [sg.Text('File Content')],
            [sg.Multiline(size=(60, 20), key='file_content')]
        ]),
            sg.Column([
                [sg.Text('Decoded Content')],
                [sg.Multiline(size=(60, 20), key='decoded_content')]
            ])],
        [sg.Text('', size=(30, 1), key='status_message', font=("Helvetica", 12))],  # Status message
        [sg.VPush()],
        [sg.Push(), sg.Button('Back', size=(10, 1), font=("Helvetica", 12), pad=((0, 10), (10, 10)))]
    ]
    window = sg.Window('Final Project DCS', layout)
    slots = {i: '' for i in range(3)}
    current_file = None

    while True:
        event, values = window.read(timeout=1)  # Add a timeout to periodically check the queue
        if event == sg.WINDOW_CLOSED or event == 'Back':
            SentToMCU(0x01)
            currState = State.Main_Menu
            break
        elif event == 'Open':
            # When the Open button is clicked, add the file to the list and update the OptionMenu
            filepath = values['file_input']
            if filepath:
                filename = os.path.basename(filepath)
                # Remove the placeholder if it's still there
                if 'No files opened yet' in opened_files:
                    opened_files.remove('No files opened yet')
                opened_files.append(filename)
                file_paths[filename] = filepath  # Store the file path with its name
                window['file_list'].update(values=opened_files)
                current_file = filename  # Set the current file to be loaded later
        elif event == 'file_list':
            # When a file is selected from the OptionMenu, display its content in the Multiline
            selected_file = values['file_list']
            if selected_file in file_paths:
                current_file = selected_file
                with open(file_paths[selected_file], 'r') as file:
                    file_content = file.read()
                    decodedTxt = decodeFile(file_content)
                window['file_content'].update(file_content)
                window['decoded_content'].update(decodedTxt)
        elif event == 'load_slot0':
            # Load the current file into slot 1
            if current_file:
                window['status_message'].update('Loading file', text_color='red')
                slots[0] = current_file
                window['slot0'].update(current_file)
                decoded_content = values['decoded_content']
                encoded_content = decoded_content.encode('ascii')  # Encode the string as bytes
                file_name_bytes = current_file.encode('ascii')
                SentToMCU(0x0C)
                for byte in file_name_bytes:
                    SentToMCU(byte)
                SentToMCU(0x00)
                for byte in encoded_content:
                    SentToMCU(byte)
                SentToMCU(0x03)  # End of content sign
        elif event == 'load_slot1':
            # Load the current file into slot 2
            if current_file:
                window['status_message'].update('Loading file', text_color='red')
                slots[1] = current_file
                window['slot1'].update(current_file)
                decoded_content = values['decoded_content']
                encoded_content = decoded_content.encode('ascii')  # Encode the string as bytes
                file_name_bytes = current_file.encode('ascii')
                SentToMCU(0x0D)
                for byte in file_name_bytes:
                    SentToMCU(byte)
                SentToMCU(0x00)
                for byte in encoded_content:
                    SentToMCU(byte)
                SentToMCU(0x03)  # End of content sign
        elif event == 'load_slot2':
            # Load the current file into slot 3
            if current_file:
                window['status_message'].update('Loading file', text_color='red')
                slots[2] = current_file
                window['slot2'].update(current_file)
                decoded_content = values['decoded_content']
                encoded_content = decoded_content.encode('ascii')  # Encode the string as bytes
                file_name_bytes = current_file.encode('ascii')
                SentToMCU(0x0E)
                for byte in file_name_bytes:
                    SentToMCU(byte)
                SentToMCU(0x00)
                for byte in encoded_content:
                    SentToMCU(byte)
                SentToMCU(0x03)  # End of content sign
        elif event == 'execute_slot0':
            SentToMCU(0x0F)
        elif event == 'execute_slot1':
            SentToMCU(0x10)
        elif event == 'execute_slot2':
            SentToMCU(0x11)
        if Ack:
            Ack = False
            window['status_message'].update('File loaded successfully', text_color='green')
        if namesValid:
            namesValid = False
            window['slot0'].update(fileNames[0])
            window['slot1'].update(fileNames[1])
            window['slot2'].update(fileNames[2])
        if posValid:
            posValid = False
            degree_value = ((s_numOfSteps-ReceivedPOS)/s_numOfSteps)*360  # Insert the value you want to display
            window['degree_text'].update(f'Degree: {degree_value}', visible=True)

    window.close()



def main():
    global currState
    uart_thread = threading.Thread(target=uart_reader, args=(), daemon=True)
    uart_thread.start()
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()

    while 1:
        if currState == State.Main_Menu:
            Main_Menu()
        elif currState == State.Manual_Menu:
            Manual()
        elif currState == State.Write:
            Painter()
        elif currState == State.Calibration_Menu:
            Calibration()
        elif currState == State.Script_Menu:
            Script()
        elif currState == State.Exit:
            break
        else:
            print("Should not be here")


'''        
    enableTX = True
    pos4 = 0
    while 1:
#        while (s.in_waiting > 0):
#            lineByte = s.read_until(terminator='\n')
#            print(lineByte.decode("ascii"))
#            time.sleep(0.25)


'''
if __name__ == '__main__':
    main()
