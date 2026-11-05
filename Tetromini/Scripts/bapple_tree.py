# We have a monochrome display and audio. OF COURSE BAD APPLE SHALL BE PLAYED!!!!!!!!!!!!!

# I took the original footage and recropped-resized it to 86x64 px, and kept it at 30fps

# Thing is, the frames of the video itself are 5.4 MB, and our flash is at MOST 1MB, and we are
# using (at time of writing) almost 75 KiB or 77KB of it. So, the goal is to compress all the frames
# into a singular bapple.bin file that we'll load and play and is at MOST 500KB. (Stretch goal: <250KB)

#                (compression approaches) : (bapple.bin size in KB)
# none, just storing all frames like that : 4509KB
#            duplicate consecutive frames : 4300KB
#           base64 LZW + huffman encoding : 1661KB
#                    base64 -> binary LZW : 1520KB
# XOR masking before LZW instead of dupes : 1144KB
# aaaaaaaaaaaaaaaaaaaaaaaaaaaand that's why you don't reinvent the wheel.
#     zlib's deflate to XOR masked frames :  570KB
#    zlib with blocks for frame integrity :  596KB
#  zlib with blocks and framebuffer order :  539KB (HUH!)
#            zlib with blocks WITHOUT XOR :  478KB (WHAT?!?!?!? LEGIT HOW)


# file includes header with number of blocks and block offsets from byte 0 of bapple.bin

# so to read file, you read the header as follows:
# first 8 bits tell you how many blocks we have, for B blocks
# the next 32 * B bits are all 32-bit nums telling you the size of the next block
# so, with all blocks read until you reach the next block's bit or EOF

from PIL import Image                        # for getting frames
from bitstring import BitArray               # for storing frames
import zlib

bapple_bin_barray = BitArray()
bapple_bin_barray_slice = BitArray()

num_frames = 6516
block_size = 36
num_blocks = num_frames // block_size
blocks_written = 0

# go through each frame, dividing it into 36-frame z-libbed chunks
for i in range(1, 6517):
    print(f"\rProcessing frame {i} of 6516...                                                    ", end="", flush=True)
    
    # add leading zeroes to i
    frame_id = "0000"[0:(4-len(str(i)))]+str(i)
    
    # store current frame's bit array
    frame_barray = BitArray()
    with Image.open(f"bapple_frames/frame_{frame_id}.bmp") as im:
        img_data = im.load()
    
        # storing it in the same way that framebuffers are stored
        for outer_y in range(8):
            for x in range(128):
                if x < 21 or x >= 107:
                    frame_barray.append("0b00000000")
                    continue
                for inner_y in range(8, 0, -1):
                    frame_barray.append("0b1" if img_data[x - 21,(outer_y * 8) + inner_y - 1] > 0 else "0b0")
        
        assert(len(frame_barray) == 8192)
        
    # add frame data to bapple.bin
    bapple_bin_barray_slice.append(frame_barray) 
    
    if i % block_size == 0:
        print(f"\rCompressing block {blocks_written + 1} of 181 with DEFLATE w/ zlibturbo friendly settings   ", end="", flush=True)
        compressor = zlib.compressobj(level=9, method=zlib.DEFLATED, wbits=-12)
        bapple_bin_str = bapple_bin_barray_slice.bytes
        bapple_deflated_raw = compressor.compress(bapple_bin_str) + compressor.flush()
        bapple_deflated = BitArray.from_bytes(bapple_deflated_raw)
        assert(len(bapple_deflated)//8 <= 65535)
        bapple_bin_barray.append(f'uintle:16={len(bapple_deflated)//8}') # add length
        bapple_bin_barray.append(bapple_deflated)
        blocks_written += 1
        bapple_bin_barray_slice = BitArray()

print(f"\rWriting headers                                                                             ", end="", flush=True)

bapple_bin_barray.prepend(f'uint8={blocks_written}')

print(f"\rSaving file! Total Size: {len(bapple_bin_barray)//8000}KB                                                                            ", end="", flush=True)

# write bapple.bin!
with open('bapple.bin', 'wb') as f:
    bapple_bin_barray.tofile(f)

# newline
print()