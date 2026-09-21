# This script is responsible for generating the sprites used in sprites.h and sprites.c

from PIL import Image
import shutil
from pathlib import Path

separator = "///////////////////////////////////////////////////////////\n"

# C File

c_original_contents = Path("../src/sprites.c").read_text(encoding="utf-8").split(separator)

c_path = Path("sprites.c")

c_sprites_def = []

sprite_byte_count = 0

# Sprite transcription

with Image.open("sprites.bmp") as im:
    for i in range(441):
        # Choosing sprite sub-region
        box = (6*(i % 21), 6*(i // 21), 6+6*(i % 21), 6+6*(i // 21))
        sprite = im.crop(box).copy()

        sprite_pixels = sprite.load()
        sprite_transcripted_pixels = []

        # Transcription
        is_sprite_empty = True
        last_filled_col = 0
        for col in range(6):
            sprite_column = ""
            for row in range(6):
                sprite_column = f"{sprite_pixels[col,row] // 128}{sprite_column}"

            sprite_transcripted_pixels.append(sprite_column)
            sprite_transcripted_pixels[col] = f"0b{sprite_transcripted_pixels[col]}"
            is_col_empty = (sprite_transcripted_pixels[col] == "0b000000")
            is_sprite_empty = is_sprite_empty and is_col_empty
            if not is_col_empty:
                last_filled_col = col
            
        # If we have an empty cell, we finished transcribing all sprites!
        if is_sprite_empty:
            c_sprites_def[-1] = c_sprites_def[-1][:-1]
            break;
            
        sprite_def = f"    {last_filled_col + 1}, {', '.join(sprite_transcripted_pixels)},"
        c_sprites_def.append(sprite_def)
        sprite_byte_count += len(sprite_transcripted_pixels) + 1;

with open(c_path, "w", encoding="utf-8") as file:
    file.write(c_original_contents[0])
    file.write(separator)
    file.write(f"\nconst uint8_t sprite_array[{sprite_byte_count}] = {{\n")
    for line in c_sprites_def:
        file.write(f"{line}\n")
    file.write("};\n\n")

    file.write(separator)
    file.write(c_original_contents[2])

# HEADER

h_original_contents = Path("../src/inc/sprites.h").read_text(encoding="utf-8").split(separator)
h_path = Path("sprites.h")

with open(h_path, "w", encoding="utf-8") as file:
    file.write(h_original_contents[0])
    file.write(separator)
    
    file.write(f"\n#define SPRITE_NUM {len(c_sprites_def)}\n")
    file.write(f"// Storage taken by sprites:                {sprite_byte_count}B\n\n")
    

    file.write(separator)
    file.write(h_original_contents[2])
    
# Overriding files
with open(Path("sprites.c.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(c_original_contents))
    
with open(Path("sprites.h.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(h_original_contents))
    
shutil.move(Path("sprites.c"), Path("../src/sprites.c"))
shutil.move(Path("sprites.h"), Path("../src/inc/sprites.h"))