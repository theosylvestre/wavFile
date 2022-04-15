//
//  main.cpp
//  wavFile_Tutorial
//
//  Created by Théo Sylvestre on 13/04/2022.
//
// Useful links : http://soundfile.sapp.org/doc/WaveFormat/ (1)
//                https://youtu.be/rHqkeLxAsTc              (2)

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

//---------Refer to (1)---------
//Riff Chunk
const string chunk_id = "RIFF"; //Contains the letters "RIFF" in ASCII form (0x52494646 big-endian form).
const string chunk_size = "----";
/*
 36 + SubChunk2Size, or more precisely:
                                4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                                This is the size of the rest of the chunk
                                following this number.  This is the size of the
                                entire file in bytes minus 8 bytes for the
                                two fields not included in this count:
                                ChunkID and ChunkSize.
 */
const string format = "WAVE";
/*
 Contains the letters "WAVE"
(0x57415645 big-endian form).

 The "WAVE" format consists of two subchunks: "fmt " and "data":
 The "fmt " subchunk describes the sound data's format:
 */


//FMT Sub-Chunk
const string subchunk1_id = "fmt "; //inserting a necessary space -> 4bytes
/*
 Contains the letters "fmt "
(0x666d7420 big-endian form).
 */
const int subchunk1_size = 16; //16 for PCM
/*
 16 for PCM.  This is the size of the
rest of the Subchunk which follows this number.
 */
const int audio_format = 1; //1 for PCM linear quantization
/*
 PCM = 1 (i.e. Linear quantization)
Values other than 1 indicate some
form of compression.
 */
const int num_channels = 2; //Mono = 1, Stereo = 2, etc.

const int sample_rate = 44100; //8000, 44100, etc.

const int byte_rate = sample_rate * num_channels * (subchunk1_size/2); //subchunk1_size is bitspersample
//== SampleRate * NumChannels * BitsPerSample/8

const int block_align = num_channels * (subchunk1_size/8);
/*
 == NumChannels * BitsPerSample/8
The number of bytes for one sample including
all channels. I wonder what happens when
this number isn't an integer?
 */

const int bits_per_sample = 16; //8 bits = 8, 16 bits = 16, etc.

//Data Sub-Chunk
const string subchunk2_id = "data";
/*
 Contains the letters "data"
 (0x64617461 big-endian form).
 */
const string subchunk2_size = "----";
/*
 == NumSamples * NumChannels * BitsPerSample/8
This is the number of bytes in the data.
You can also think of this as the size
of the read of the subchunk following this number.
 */

const int duration = 2;
const int max_amplitude = 32760; //2^16 bits divided by 2
const double frequency = 250;

//Refer to (2), (23min37s)
void write_as_bytes(ofstream &file, int value, int byte_size){
    file.write(reinterpret_cast<const char*>(&value), byte_size);
}


int main() {
    
    ofstream wav;
    wav.open("test.wav", ios::binary);

    if(wav.is_open())
    {
        wav << chunk_id;
        wav << chunk_size;
        wav << format;
        wav << subchunk1_id;
        write_as_bytes(wav, subchunk1_size, 4);
        write_as_bytes(wav, audio_format, 2);
        write_as_bytes(wav, num_channels, 2);
        write_as_bytes(wav, sample_rate, 4);
        write_as_bytes(wav, byte_rate, 4);
        write_as_bytes(wav, block_align, 2);
        write_as_bytes(wav, bits_per_sample, 2);
        
        wav << subchunk2_id;
        wav << subchunk2_size;
        
        int start_audio = wav.tellp();

        //for loop is time definition
        for(int i=0; i < sample_rate * duration; i++){
            //amplitude is the amplitude of the sinewave we're creating next line
            double amplitude = (double)i / sample_rate * max_amplitude;
            double sinewave = sin( (2 * 3.14 * i * frequency) / sample_rate);
            
            double channel1 = amplitude * sinewave / 2;
            double channel2 = (max_amplitude - amplitude) * sinewave;
            
            write_as_bytes(wav, channel1, 2);
            write_as_bytes(wav, channel2, 2);
            
        }
        
        int end_audio = wav.tellp();
        
        wav.seekp(start_audio - 4);
        write_as_bytes(wav, end_audio, 4);
        
        wav.seekp(4, ios::beg);
        write_as_bytes(wav, end_audio - 8, 4);
    }
    
    wav.close();
    
    return 0;
}
