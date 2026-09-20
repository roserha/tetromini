# This script is responsible for generating the sprites used in sprites.h and sprites.c

from PIL import Image
import shutil
from pathlib import Path

separator = "///////////////////////////////////////////////////////////\n"

# C File

c_original_contents = Path("../src/sprites.c").read_text(encoding="utf-8").split(separator)

c_path = Path("sprites.c")

c_sprites_def = []
c_spritearray_def = ["uint8_t *spriteArray[] = {"]

# Sprite transcription

with Image.open("sprites.bmp") as im:
    for i in range(441):
        # Choosing sprite sub-region
        box = (6*(i % 21), 6*(i // 21), 6+6*(i % 21), 6+6*(i // 21))
        sprite = im.crop(box).copy()

        sprite_pixels = sprite.load()
        sprite_transcripted_pixels = []

        # Transcription
        for col in range(6):
            sprite_column = ""
            for row in range(6):
                sprite_column = f"{sprite_pixels[col,row] // 128}{sprite_column}"

            sprite_transcripted_pixels.append(sprite_column)
            sprite_transcripted_pixels[col] = f"0b{sprite_transcripted_pixels[col]}"
            
        # Column pruning (deleting all last columns that are empty)
        for col in range(6):
            if sprite_transcripted_pixels[-1] == "0b000000":
                sprite_transcripted_pixels.pop()
            
        # If we have an empty cell, we finished transcribing all sprites!
        if len(sprite_transcripted_pixels) == 0:
            c_spritearray_def[-1] = c_spritearray_def[-1][:-1]
            break;
            
        sprite_def = f"uint8_t sprite{i}[{len(sprite_transcripted_pixels) + 1}] = {{ {len(sprite_transcripted_pixels)}, {', '.join(sprite_transcripted_pixels)}}};"
        c_sprites_def.append(sprite_def)
        c_spritearray_def.append(f"    sprite{i},")

c_spritearray_def.append("};")

with open(c_path, "w", encoding="utf-8") as file:
    file.write(c_original_contents[0])
    file.write(separator)
    for line in c_sprites_def:
        file.write(f"{line}\n")

    file.write("\n")

    for line in c_spritearray_def:
        file.write(f"{line}\n")

    file.write(separator)
    file.write(c_original_contents[2])

# HEADER

h_original_contents = Path("../src/inc/sprites.h").read_text(encoding="utf-8").split(separator)
h_path = Path("sprites.h")

with open(h_path, "w", encoding="utf-8") as file:
    file.write(h_original_contents[0])
    file.write(separator)
    
    file.write(f"\n#define SPRITE_NUM {len(c_sprites_def)}\n\n")

    file.write(separator)
    file.write(h_original_contents[2])
    
# Overriding files
with open(Path("sprites.c.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(c_original_contents))
    
with open(Path("sprites.h.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(h_original_contents))
    
shutil.move(Path("sprites.c"), Path("../src/sprites.c"))
shutil.move(Path("sprites.h"), Path("../src/inc/sprites.h"))