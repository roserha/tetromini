# repurposing bapple to make a super cool main menu!

# animated it myself in blender hehehe


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

num_frames = 72
block_size = 36
num_blocks = num_frames // block_size
blocks_written = 0

# go through each frame, dividing it into 36-frame z-libbed chunks
for i in range(1, num_frames+1):
    print(f"\rProcessing frame {i} of {num_frames}...                                                    ", end="", flush=True)
    
    # add leading zeroes to i
    frame_id = "0000"[0:(4-len(str(i)))]+str(i)
    
    # store current frame's bit array
    frame_barray = BitArray()
    with Image.open(f"mainmenu_frames/{frame_id}.bmp") as im:
        img_data = im.load()
    
        # storing it in the same way that framebuffers are stored
        for outer_y in range(8):
            for x in range(128):
                for inner_y in range(8, 0, -1):
                    frame_barray.append("0b1" if img_data[x, (outer_y * 8) + inner_y - 1] > 128 else "0b0")
        
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
with open('mainmenu.bin', 'wb') as f:
    bapple_bin_barray.tofile(f)

# newline
print()