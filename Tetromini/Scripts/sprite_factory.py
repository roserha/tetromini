# This script is responsible for generating the sprites used in sprites.hpp

from PIL import Image
import shutil
from pathlib import Path

separator = "///////////////////////////////////////////////////////////\n"

# HPP File

original_contents = Path("../App/Controllers/Inc/sprites.hpp").read_text(encoding="utf-8").split(separator)

path = Path("sprites.hpp")

c_sprites_def = []

sprite_byte_count = 0

# Sprite transcription
# Essentially we store their horizontal length, and then their pixels top to bottom column first

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
            
        sprite_def = f"            {last_filled_col + 1}, {', '.join(sprite_transcripted_pixels)},"
        c_sprites_def.append(sprite_def)
        sprite_byte_count += len(sprite_transcripted_pixels) + 1;

with open(path, "w", encoding="utf-8") as file:
    file.write(original_contents[0])
    file.write(separator)
    
    file.write(f"\n#define SPRITE_NUM {len(c_sprites_def)}\n")
    file.write(f"// Storage taken by sprites:                {sprite_byte_count}B\n\n")
    

    file.write(separator)
    file.write(original_contents[2])
    file.write(separator)
    file.write(f"\n        static constexpr std::array<uint8_t, {sprite_byte_count}> kSpriteArray = {{\n")
    for line in c_sprites_def:
        file.write(f"{line}\n")
    file.write("        };\n\n        ")

    file.write(separator)
    file.write(original_contents[4])
    
# Overriding files
with open(Path("sprites.hpp.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(original_contents))
    
shutil.move(Path("sprites.hpp"), Path("../App/Controllers/Inc/sprites.hpp"))