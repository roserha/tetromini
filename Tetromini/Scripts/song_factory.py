import mido
from PIL import Image
import shutil
from pathlib import Path

separator = "///////////////////////////////////////////////////////////\n"

# HPP File

original_contents = Path("../App/Controllers/Inc/melody.hpp").read_text(encoding="utf-8").split(separator)

path = Path("melody.hpp")

c_sprites_def = []

sprite_byte_count = 0


main_song_midi = mido.MidiFile('main_song.mid')

# We'll store every single note message in a 2xn array, where column 1 is the real time the message should
# be sent, and column 2 is the message itself. This way, to merge all notes, we can merge the arrays and sort
# based on the real time, and then process the sleeps together as one full thing considering all the notes

ticks_per_beat = main_song_midi.ticks_per_beat
song_array_with_times = []
last_note_length = 0
unique_note_lengths = set()
unique_wait_lengths = set()
for i, track in enumerate(main_song_midi.tracks):
    if not track.name.isnumeric():
        continue
    print(f"track {i} is instrument {track.name} and has {len(track)} messages")
    track_tempo = 500000
    
    
    playing_note = False
    time_to_wait_before_note = 0
    note_length = 0
    note_note = 0
    note_octave = 0
    current_time = 0
    note_volume = 28 if track.name != "2" else 20
    note_waveNumber = int(track.name)
    
    for msg in track:
        if msg.type == 'set_tempo':
            track_tempo = msg.tempo
            print(f"and tempo {track_tempo}")
            
        if msg.type == 'note_on':
            playing_note = True
            time_to_wait_before_note = round(mido.tick2second(msg.time, ticks_per_beat, track_tempo) * 250) * 4 # automatic 4ms rounding!!!
            note_note = msg.note % 12
            note_octave = (msg.note // 12) - 1
            current_time += time_to_wait_before_note
            # if we have to wait before the note, let's add a "phantom note" that plays for that long
            if (time_to_wait_before_note != 0):
                note_message = (note_note & 0x1f) | ((note_octave & 0x7) << 5) | ((time_to_wait_before_note & 0x7fc) << 6) | ((note_waveNumber & 0xff) << 23)
                song_array_with_times.append((current_time - time_to_wait_before_note, note_message))
                unique_wait_lengths.add((time_to_wait_before_note, ((time_to_wait_before_note & 0x7fc) << 6), ((0xFF00 & ((time_to_wait_before_note & 0x7fc) << 6)) >> 6)))
            
        if msg.type == 'note_off':
            note_length = round(mido.tick2second(msg.time, ticks_per_beat, track_tempo) * 250) * 4 # automatic 4ms rounding!!!
            current_time += note_length
            
            # add this note to the list of messages
            note_message = (note_note & 0x1f) | ((note_octave & 0x7) << 5) | ((note_length & 0x7fc) << 6) | ((note_volume & 0x7f) << 16) | ((note_waveNumber & 0xff) << 23)
            song_array_with_times.append((current_time - note_length, note_message))
            unique_note_lengths.add((note_length, ((note_length & 0x7fc) << 6), ((0xFF00 & ((note_length & 0x7fc) << 6)) >> 6)))
            
            last_note_length = note_length
            playing_note = False
            time_to_wait_before_note = 0
            note_length = 0
            note_note = 0
            note_octave = 0
            
# Now that we collected all the notes, let's sort them by time played. Since it's the first element of the tuple, it's pretty easy!
song_array_with_times.sort()

song_array = []

print("note lengths:", unique_note_lengths)
print("wait lengths:", unique_wait_lengths)

last_time = 0
for time, note in song_array_with_times:
    if time != last_time:
        wait_msg = 0x80000000 | (time - last_time)
        song_array.append(wait_msg)
        last_time = time
    song_array.append(note)

# add last note's wait time
wait_msg = 0x80000000 | last_note_length
song_array.append(wait_msg)

# now let's print it all cute-like for the script
song_code = f"\n        static constexpr std::array<uint32_t, {len(song_array)}> main_song = {{"

for i, number in enumerate(song_array):
    if i % 12 == 0:
        song_code += "\n            "
    song_code += f"{hex(number)}"
    
    if i != (len(song_array) - 1):
        song_code += ", "

song_code += "\n        };\n\n        "

with open(path, "w", encoding="utf-8") as file:
    file.write(original_contents[0])
    file.write(separator)
    
    file.write(song_code)
    

    file.write(separator)
    file.write(original_contents[2])
    
# Overriding files
with open(Path("melody.hpp.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(original_contents))
    
shutil.move(Path("melody.hpp"), Path("../App/Controllers/Inc/melody.hpp"))