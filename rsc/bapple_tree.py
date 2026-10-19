# We have a monochrome display and audio. OF COURSE BAD APPLE SHALL BE PLAYED!!!!!!!!!!!!!

# Thing is, the frames of the video itself are 5.4 MB, and our flash is at MOST 1MB, and we are
# using (at time of writing) almost 75 KiB or 77KB of it. So, the goal is to compress all the frames
# into a singular bapple.bin file that we'll load and play and is at MOST 500KB. (Stretch goal: <250KB)

#                (compression approaches) : (bapple.bin size in KB)
# none, just storing all frames like that : 4509KB
#            duplicate consecutive frames : 4300KB
#           base64 LZW + huffman encoding : 1661KB
#                    base64 -> binary LZW : 1520KB
# XOR masking before LZW instead of dupes : 1144KB
#     zlib's deflate to XOR masked frames :  570KB
#    zlib with blocks for frame integrity :  596KB

# aaaaaaaaaaaaaaaaaaaaaaaaaaaand that's why you don't reinvent the wheel.

# file includes header with number of blocks and block offsets from byte 0 of bapple.bin

from PIL import Image                        # for getting frames
from bitstring import BitArray               # for storing frames
import base64                                # for converting to string for lzw
from heapq import heappush, heappop, heapify # for huffman encoding
from collections import Counter              # ditto
import zlib

bapple_bin_barray = BitArray()
bapple_bin_barray_slice = BitArray()
previous_frame_barray_content = BitArray(length=5504)

# # LZW Encoding, tysm stack overflow and rosetta code :clap_emoji:
# def lzw_compress(frame_bitstring):
#     # constructing dict
#     dict_size = 256
#     dictionary = {chr(i): chr(i) for i in range(dict_size)}
    
#     w = ""
#     result = []
    
#     for c in frame_bitstring:
#         # add c to word
#         wc = w + chr(c)
        
#         if wc in dictionary:
#             # always true in first iteration, since it'll be a single char, and we have those
#             # and always false in second iteration, since we don't start off with any double chars
#             w = wc
#         else:
#             # wc is not in dict, but w itself is (given we start with all 1-char long ascii strs)
#             # so add old word to result
#             result.append(dictionary[w] if (type(dictionary[w]) is int) else ord(dictionary[w]))
            
#             # add wc to dictionary
#             dictionary[wc] = dict_size
#             dict_size += 1
            
#             # and lets reset word to be just c
#             w = chr(c)
            
#     if w:
#         result.append(dictionary[w] if (type(dictionary[w]) is int) else ord(dictionary[w]))
        
#     return result

# # Huffman Encoding, tysm rosetta code again
# # Use counters to already get the frequency of symbols
# def derive_huffman_codes(symb2freq: Counter):
#     # generate min heap of symbols
#     heap = [[weight, [symbol, ""]] for symbol, weight in symb2freq.items()]
#     heapify(heap)
    
#     # generate codes for each element of heap
#     while len(heap) > 1:
#         lo = heappop(heap)
#         hi = heappop(heap)
#         for pair in lo[1:]:
#             pair[1] = '0' + pair[1]
#         for pair in hi[1:]:
#             pair[1] = '1' + pair[1]
#         heappush(heap, [lo[0] + hi[0]] + lo[1:] + hi[1:])
#     return sorted(heappop(heap)[1:], key = lambda p: (len(p[-1]), p))

offsets = []
num_blocks = 6516 // 36 # 181
blocks_written = 0

# go through each frame, dividing it into 36-frame z-libbed chunks
for i in range(1, 6517):
    print(f"\rProcessing frame {i} of 6516...                                                    ", end="", flush=True)
    
    # add leading zeroes to i
    frame_id = "0000"[0:(4-len(str(i)))]+str(i)
    
    # store current frame's bit array
    frame_barray = BitArray()
    frame_barray_frame_content = BitArray()
    with Image.open(f"bapple_frames/frame_{frame_id}.bmp") as im:
        img_data = im.load()
    
        # this order improved LZW efficiency somehow. I STUMBLED UPON IT BY LUCK! insane
        for x in range(86):
            for y in range(64):
                frame_barray_frame_content.append("0b1" if img_data[x,y] > 0 else "0b0")
                
    # add frame data xorred into previous frame
    frame_barray.append(frame_barray_frame_content ^ previous_frame_barray_content)
        
    # add frame data to bapple.bin
    bapple_bin_barray_slice.append(frame_barray)
    previous_frame_barray_content = frame_barray_frame_content    
    
    if i % 36 == 0:
        print(f"\rCompressing block {blocks_written + 1} of 181 with DEFLATE w/ zephyr friendly settings   ", end="", flush=True)
        compressor = zlib.compressobj(level=9, method=zlib.DEFLATED, wbits=-12)
        bapple_bin_str = bapple_bin_barray_slice.bytes
        bapple_deflated_raw = compressor.compress(bapple_bin_str) + compressor.flush()
        bapple_deflated = BitArray(bytes=bapple_deflated_raw)
        offsets.append(len(bapple_bin_barray)+725) # 725 for uint8_t of # blocks and uint32_t of offsets
        bapple_bin_barray.append(bapple_deflated)
        blocks_written += 1
        previous_frame_barray_content = BitArray(length=5504)
        bapple_bin_barray_slice = BitArray()

print(f"\rWriting headers                                                                             ", end="", flush=True)

for i in range(181,0,-1):
    bapple_bin_barray.prepend(f'uint32={offsets[i - 1]}')

bapple_bin_barray.prepend(f'uint8={num_blocks}')

# print(f"\rCompressing with LZW                           ", end="", flush=True)
# bapple_bin_lzw = lzw_compress(bapple_bin_str)

# print(f"\rGenerating Huffman Tree", end="", flush=True)
# bapple_bin_lzw_counter = Counter(bapple_bin_lzw)
# bapple_bin_hufftree = derive_huffman_codes(bapple_bin_lzw_counter)

# print(f"\rEncoding Huffman Tree   ", end="", flush=True)
# # Huffman tree will be stored as follows:
# #  6 bits: (n) -> maximum number of bits to represent maximum encoded symbol value in LZW-ified file
# #  32 bits: how many bits the below steps take up
# #  for each node:
# #     n bits: number to be encoded
# #     8 bits: (k) -> length of huffman code
# #     k bits: code


# lzw_max_num_encoded = max(bapple_bin_lzw)
# how_many_bits_lmne_takes = len(bin(lzw_max_num_encoded)) - 2
# huff_tree_bin = BitArray()

# for symbol, code in bapple_bin_hufftree:
#     huff_tree_bin.append(f'uint{how_many_bits_lmne_takes}={symbol}')
#     huff_tree_bin.append(f'uint8={len(code)}')
#     huff_tree_bin.append('0b' + code)
    
# huff_tree_bin.prepend(f'uint32={len(huff_tree_bin)}')
# huff_tree_bin.prepend(f'uint6={how_many_bits_lmne_takes}')

# bapple_bin_barray = huff_tree_bin

# print(f"\rTranslating LZW-ified Bapple into Huffman Codes", end="", flush=True)

# bapple_bin_hufftree = dict(bapple_bin_hufftree)

# for symbol in bapple_bin_lzw:
#     code = bapple_bin_hufftree[symbol]
#     bapple_bin_barray.append('0b' + code)

print(f"\rSaving file!                                                                             ", end="", flush=True)

# write bapple.bin!
with open('bapple.bin', 'wb') as f:
    bapple_bin_barray.tofile(f)

# newline
print()