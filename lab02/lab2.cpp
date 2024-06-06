#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip> 

using namespace std;

// Define the structure for the file header
struct binflag_header_t {
    uint64_t magic;     /* 'BINFLAG\x00' */
    uint32_t datasize;  /* in big-endian */
    uint16_t n_blocks;  /* in big-endian */
    uint16_t zeros;
};

// Define the structure for a data block
struct Block {
    uint32_t offset; // in big-endian
    uint16_t cksum;  // XOR'ed results of each 2-byte unit in payload
    uint16_t length; // ranges from 1KB - 3KB, in big-endian
    vector<uint8_t> payload;
};

// Define the flag information structure
struct flag_t {
    uint16_t length;        /* length of the offset array, in big-endian */
    uint32_t offset[0];     /* offset of the flags, in big-endian */
};

//convert a little-endian value to big-endian
template <typename T>
T convertToBigEndian(const T& value) {
    T result = value;
    std::reverse(reinterpret_cast<uint8_t*>(&result), reinterpret_cast<uint8_t*>(&result) + sizeof(T));
    return result;
}

int main() {
    // downloading the challenge file 
    string studentID = "110550039";  
    string downloadURL = "https://inp.zoolab.org/binflag/challenge?id=" + studentID;
    string outputFileName = "challange.bin"; 

    string downloadCommand = "curl -o " + outputFileName + " " + downloadURL;
    int downloadStatus = system(downloadCommand.c_str());

    if (downloadStatus != 0) {
        cerr << "Failed to download the challenge file." << endl;
        return 1;
    }

    ifstream file(outputFileName, ios::binary);
   
    //ifstream file("demo2.bin", ios::binary);

    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    // Read the file header
    struct binflag_header_t header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (file.gcount() != sizeof(header)) {
        cerr << "Failed to read the file header." << endl;
        file.close();
        return 1;
    }

    // Convert the magic value to big-endian
    header.magic = convertToBigEndian(header.magic);

    // Check the magic string
    if (header.magic != 0x42494E464C414700ULL) {
        cerr << "Invalid magic string. This may not be a valid challenge file." << endl;
        file.close();
        return 1;
    }

    // Print the number of data blocks (n_blocks)
    header.n_blocks = convertToBigEndian(header.n_blocks);
    cout << "Number of data blocks (n_blocks): " << header.n_blocks << endl;
    header.datasize = convertToBigEndian(header.datasize);
    cout<< "Datasize"<< header.datasize<<endl;
    // Initialize a vector of Block structures
    vector<Block> blocks(header.n_blocks);
    vector<uint8_t> dictionary(header.datasize, 0);

    for (size_t i = 0; i < header.n_blocks; ++i) {
        // Read and convert to big-endian
        file.read(reinterpret_cast<char*>(&blocks[i].offset), sizeof(blocks[i].offset));
        //cout<<"offset"<<blocks[i].offset<<endl;
        blocks[i].offset = convertToBigEndian(blocks[i].offset);
        //cout<<"offset*****"<<blocks[i].offset<<endl;

        file.read(reinterpret_cast<char*>(&blocks[i].cksum), sizeof(blocks[i].cksum));
        //cout<<"checksum"<<blocks[i].cksum<<endl;
        blocks[i].cksum = convertToBigEndian(blocks[i].cksum);
        //cout<<"checksum*****"<<blocks[i].cksum<<endl;

        file.read(reinterpret_cast<char*>(&blocks[i].length), sizeof(blocks[i].length));
        //cout<<"length"<<blocks[i].length<<endl;
        blocks[i].length = convertToBigEndian(blocks[i].length);
        //cout<<"length"<<blocks[i].length<<endl;
        
        // Resize the payload vector and read the payload
        blocks[i].payload.resize(blocks[i].length);
        file.read(reinterpret_cast<char*>(blocks[i].payload.data()), blocks[i].length);

        // Calculate the XOR checksum for the payload (every 2-byte unit)
        uint16_t calculatedChecksum = 0;
        for (size_t j = 0; j < blocks[i].payload.size(); j += 2) {
            uint16_t payloadUnit = static_cast<uint16_t>(blocks[i].payload[j]) << 8;
            if (j + 1 < blocks[i].payload.size()) {
                payloadUnit |= blocks[i].payload[j + 1];
            }
            calculatedChecksum ^= payloadUnit;
        }

        //cout<<"calculatedChecksum"<<calculatedChecksum<<endl;

        // Compare the calculated checksum with the one in the block
        if (calculatedChecksum != blocks[i].cksum) {
            //cerr << "Block " << i << " has an incorrect checksum." << endl;
            continue;
        }

        if (blocks[i].offset + blocks[i].length <= header.datasize) {
            for (size_t j = 0; j < blocks[i].length; ++j) {
                 dictionary[blocks[i].offset + j] = blocks[i].payload[j];
                // cout << "D[" << blocks[i].offset + j << "] = " << static_cast<int>(dictionary[blocks[i].offset + j])<<" ";
            }
        }
        
        // Print the offset, length, and checksum
        cout << "Block " << i << " - Offset: " << blocks[i].offset << " Length: " << blocks[i].length << " Checksum: " << blocks[i].cksum << endl;
    }
    
    // Read the flag information
    struct flag_t flag_info;
    file.read(reinterpret_cast<char*>(&flag_info), sizeof(uint16_t)); // Read only the length field
    uint16_t flag_length = convertToBigEndian(flag_info.length);

    // Dynamically allocate memory for flag_info structure and offset array
    flag_t* flag_info_with_offsets = reinterpret_cast<flag_t*>(new char[sizeof(uint16_t) + flag_length * sizeof(uint32_t)]);

    // Copy the length field to the allocated structure
    flag_info_with_offsets->length = flag_length;

    // Read the flag offsets
    file.read(reinterpret_cast<char*>(flag_info_with_offsets->offset), flag_length * sizeof(uint32_t));

    // Convert flag information to big-endian
    flag_info_with_offsets->length = convertToBigEndian(flag_info_with_offsets->length);

    // Ensure the flag_info.offset array is dynamically allocated
    uint32_t* flag_offsets = flag_info_with_offsets->offset;
    for (uint16_t i = 0; i < flag_length; ++i) {
        flag_offsets[i] = convertToBigEndian(flag_offsets[i]);
    }

    // Initialize a string to store the flag
    string flag;

    // Process flag offsets
    for (uint16_t i = 0; i < flag_length; ++i) {
        if (flag_offsets[i] < header.datasize && flag_offsets[i] + 1 < header.datasize) {
            // Concatenate two successive flag characters to the flag string
            cout<<"offset[i]" <<flag_offsets[i]<<endl;
            flag += static_cast<char>(dictionary[flag_offsets[i]]);
            flag += static_cast<char>(dictionary[flag_offsets[i] + 1]);
        }
    }
    // Print the retrieved flag
    //cout << "Flag: " << flag << endl;

    // Convert the flag to hexadecimal representation
    string hexadecimalFlag;
    cout << "Hexadecimal Flag: ";
    for (char c : flag) {
        //cout << hex << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
        stringstream ss;
        ss << hex << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
        hexadecimalFlag += ss.str();
    }
    cout << hexadecimalFlag << endl;

    string verificationURL = "https://inp.zoolab.org/binflag/verify?v=" + hexadecimalFlag;
    string verificationCommand = "curl -i " + verificationURL;

    // Use popen to capture the response
    FILE* pipe = popen(verificationCommand.c_str(), "r");

    if (!pipe) {
        cerr << "Failed to submit the flag for verification." << endl;
        return 1;
    }

    string response;
    char buffer[128];

    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            response += buffer;
        }
    }

    pclose(pipe);

    // Print the response
    cout << "Verification Response:\n" << response << endl;

    // Close the file
    file.close();

    return 0;
}