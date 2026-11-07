import mido
from PIL import Image
import shutil
from pathlib import Path

separator = "///////////////////////////////////////////////////////////\n"

# HPP File

original_contents = Path("../App/Controllers/Inc/melody.hpp").read_text(encoding="utf-8").split(separator)

path = Path("melody.hpp")

def midi_to_array(src, basevol=28):
    main_song_midi = mido.MidiFile(src)

    # We'll store every single note message in a 2xn array, where column 1 is the real time the message should
    # be sent, and column 2 is the message itself. This way, to merge all notes, we can merge the arrays and sort
    # based on the real time, and then process the sleeps together as one full thing considering all the notes

    ticks_per_beat = main_song_midi.ticks_per_beat
    song_array_with_times = []
    last_note_length = 0
    
    # these values are for curiosity
    unique_note_lengths = set()
    unique_wait_lengths = set()
    
    # we'll store the current time notes were turned on so we can keep track of simultaneous notes
    notes_on = dict()
    for i, track in enumerate(main_song_midi.tracks):
        if not track.name.isnumeric():
            continue
        print(f"track {i} is instrument {track.name} and has {len(track)} messages")
        track_tempo = 500000
        
        current_time = 0
        note_volume = basevol 
        if track.name == "2" or track.name == 4:
            note_volume = int(basevol // 1.5)
            
        note_waveNumber = int(track.name)
        
        for msg in track:
            if msg.type == 'set_tempo':
                track_tempo = msg.tempo
                print(f"and tempo {track_tempo}")
                
            delta_seconds = mido.tick2second(msg.time, ticks_per_beat, track_tempo)
            delta_rounded = round(delta_seconds * 1000) # 1ms rounding
            current_time += delta_rounded
                
            if msg.type == 'note_on':
                time_to_wait_before_note = delta_rounded
                note_note = msg.note % 12
                note_octave = (msg.note // 12) - 1
                
                # if we have to wait before the note, let's add a "phantom note" that plays for that long
                while (time_to_wait_before_note != 0):
                    phantom_note_duration = min(time_to_wait_before_note, 1020)
                    note_message = (note_note & 0x1f) | ((note_octave & 0x7) << 5) | ((phantom_note_duration & 0xfff) << 8) | ((note_waveNumber & 0xf) << 27)
                    song_array_with_times.append((current_time - phantom_note_duration, note_message))
                    unique_wait_lengths.add((phantom_note_duration, ((phantom_note_duration & 0xfff) << 8), ((0xFFF00 & ((phantom_note_duration & 0xfff) << 8)) >> 8)))
                    time_to_wait_before_note -= phantom_note_duration
                
                notes_on[msg.note] = current_time
                
            if msg.type == 'note_off':
                note_length = current_time - notes_on[msg.note]
                if (note_length >= 4096):
                    print("note too large at time", current_time / 1000, "seconds:", note_length)
                    assert(note_length < 4096)
                if (note_length < 0):
                    print("note length invalid at time", current_time / 1000, "seconds:", note_length)
                    assert(note_length >= 0)
                # current_time += note_length
                
                note_note = msg.note % 12
                note_octave = (msg.note // 12) - 1
                
                # add this note to the list of messages
                if note_length > 0:
                    note_message = (note_note & 0x1f) | ((note_octave & 0x7) << 5) | ((note_length & 0xfff) << 8) | ((note_volume & 0x7f) << 20) | ((note_waveNumber & 0xf) << 27)
                    song_array_with_times.append((current_time - note_length, note_message))
                    unique_note_lengths.add((note_length, ((note_length & 0xfff) << 8), ((0xFF00 & ((note_length & 0xfff) << 8)) >> 8)))
                
                last_note_length = note_length
                
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
    song_code = f"\n        static constexpr std::array<uint32_t, {len(song_array)}> {src.split(".")[0]} = {{"

    for i, number in enumerate(song_array):
        if i % 12 == 0:
            song_code += "\n            "
        song_code += f"{hex(number)}"
        
        if i != (len(song_array) - 1):
            song_code += ", "

    song_code += "\n        };\n\n        "

    return song_code

songs = midi_to_array("main_song.mid")
songs += midi_to_array("bapple_song.mid", 20)

with open(path, "w", encoding="utf-8") as file:
    file.write(original_contents[0])
    file.write(separator)
    
    file.write(songs)
    

    file.write(separator)
    file.write(original_contents[2])
    
# Overriding files
with open(Path("melody.hpp.bak"), "w", encoding="utf-8") as file:
    file.write(separator.join(original_contents))
    
shutil.move(Path("melody.hpp"), Path("../App/Controllers/Inc/melody.hpp"))