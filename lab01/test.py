import subprocess
import time
import threading
import re
import sys  
import select
from scapy.all import *


# Parameters
user_id = "110550039"
#server = "inp.zoolab.org"
server = "127.0.0.1"
port = "10495"
pcap_file = "captured_packet.pcap"

timeout = 10  # Adjusted timeout

def print_output(pipe):
    while True:
        line = pipe.readline()
        if line:
            print(line.decode().strip())
        else:
            break

# Start tcpdump in the background
tcpdump = subprocess.Popen(['sudo', 'tcpdump', '-i', 'any', '-w', pcap_file, '-vv', 'udp', f'port {port}'],
                           stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE)

# Allow tcpdump to start
time.sleep(2)
threading.Thread(target=print_output, args=(tcpdump.stdout,)).start()

# Start netcat and interact with the server
with subprocess.Popen(['nc', '-u', server, port], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True) as nc:

    # Send hello and get the challenge ID
    nc.stdin.write(f'hello {user_id}\n')
    nc.stdin.flush()
    response = nc.stdout.readline()
    print(response.strip())

    # Extract the challenge ID using regular expressions
    match = re.search(r'OK (\S+)', response)
    if match:
        challenge_id = match.group(1)
        print('Challenge ID:', challenge_id)

        start_time = time.time()
        # Send chals command to request the challenge
        nc.stdin.write(f'chals {challenge_id}\n')
        nc.stdin.flush()

        # Print responses (adjust according to the actual response format)
        while True:
            ready_to_read, _, _ = select.select([nc.stdout], [], [], timeout)
            if ready_to_read:  # If there's data to read
                response = nc.stdout.readline()
                if response:
                    print(response.strip())
                else:
                    print("No respond")
                    break
            else:
                #print("Timeout reached")
                break
# Stop tcpdump after the interaction with the server is done
#print("Terminating tcpdump...")
tcpdump.terminate()
time.sleep(2)  # Give tcpdump a moment to terminate
print("tcpdump terminated.")



packets_read = rdpcap(pcap_file)

payload_length_list = []
ip_options_list = []

payload_list = []
flag_data = []
begin_capture = False


for packet in packets_read:
    if packet.haslayer(UDP):  
        payload = packet[UDP].payload.load.decode(errors='ignore')  

        ip_options_length = len(packet[IP].options)
        udp_payload_length = len(packet[UDP].payload)  
        x_header_length = ip_options_length + udp_payload_length
        
        if "BEGIN FLAG" in payload:
            begin_capture = True
            continue  
            
        elif "END FLAG" in payload:
            begin_capture = False
        
        if begin_capture:            
            try:
                payload_length_list.append(udp_payload_length)
                ip_options_list.append(ip_options_length)
                payload_list.append(payload)
                flag_data.append(chr(x_header_length))
            except ValueError:
                sys.stderr.write(f"Unable to convert {x_header_length} to ASCII character.\n")
                continue

# 輸出
flag = ''.join(flag_data)
print(f"Decoded Flag: {flag}")

