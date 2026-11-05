# Tetromini - STM Edition (In active development)

This is a refactor of the old Nordic/Zephyr version of the project for the STM32F405/freeRTOS pipeline that is archived in `dev`!

## About
Tetromini is a portable console based on an Adafruit Feather STM32F405 Express running a custom firmware inside freeRTOS that plays a fan-made version of a falling-block puzzle game!

![Tetromini WEBP](./Resources/tetromini.webp)

The project consists of multiple parts. As of writing, the firmware that controls the console itself and the game inside the firmware have a working version!

Future developments may want to explore things such as:
- [ ] Game over screens
- [ ] High score tracking (?)
- [ ] A custom enclosure shaped like a T-Block Tetromino

## Features
- Sprite-based rendering
- 4 directional inputs (move left, move right, soft drop, hard drop)
- Clockwise and counterclockwise rotation
- Dynamic gravity and boosted score based on level
- Combo points per consecutive line clears
- Pseudorandomly chosen pieces
- Dropped pieces and 4 different line clear sound effects
- Multi-channel soundtrack played via synthesized notes with custom notes system
- Multithreaded parallel execution

## Schematics, PCB and BOM
The below KiCad schematic is based on Rev 1, running on a breadboard, and set to run on a PCB under Rev 1a. (last updated Oct 28th 2026)

![KiCAD Schematic](./Resources/schematic_rev_1.png)
![KiCAD PCB](./Resources/pcb_rev_1a.png)
![KiCAD PCB](./Resources/pcb_rev_1a_oshpark.png)

Bill Of Materials for Rev 1a (last updated Oct 28th 2026):
| Reference                                                       | Description                             | MPN      | DigiKey PN                 | Manufacturer |
|-----------------------------------------------------------------|-----------------------------------------|----------|----------------------------|--------------|
| U1                                                              | Feather STM32F405 Express               | 4382     | 1528-4382-ND               | Adafruit     |
| DS1                                                             | Generic 2.42in SSD1309 OLED SPI Display | --       | --                         | HiLetgo      |
| DS2                                                             | I2S Audio Breakout                      | 14809    | 1568-14809-ND              | Sparkfun     |
| DOWN1,LEFT1,RIGHT1,ROT_LEFT1,ROT_RIGHT1,UP1,START_BTN,POWER_BTN | Push button switch, generic, two pins   | MJTP1234 | 679-2442-ND                | APEM Inc.    |
| C1,C2,C3,C4,C5,C6,C7,C8                                         | 100nF Capacitor                         | C320C104K5R5TA | 399-4264-ND          | KEMET        |
| C9                                                              | 100UF Capacitor                         | 16ZLH100MEFC5X11 | 1189-1148-ND       | Rubycon      |
| R1,R2,R5,R6,R9,R10,R13,R14                                      | 10kOhm Resistor                         | ROX05SJ10K | A142728CT-ND             | TE Connectivity Passive Product |
| R3,R4,R7,R8,R11,R12,R15,R16                                     | 100kOhm Resistor                        | WK202070A1003J2200 | 749-WK202070A1003J2200CT-ND | Vishay |
| SW1                                                             | SPDT Switch                             | SS12SDP2 | 360-2133-ND                | NKK Switches |
| --                                                              | 8ohm 700MW Speaker                      | CMS-181325-078L100B-67 | 2223-CMS-181325-078L100B-67-ND | Same Sky |
| --                                                              | Lithium Battery                         | 258      | 1528-1838-ND               | Adafruit     |
| --                                                              | SMD SWD Header for Feather              | 4048     | 1528-2827-ND               | Adafruit     |
| --                                                              | Headers for Feather                     | 2886     | 1528-1560-ND               | Adafruit     |
| --                                                              | Headers for OLED and I2S Breakout       | PPTC071LFBN-RC | S7005-ND             | Sullins Connector Solutions |

## Firmware and Multithreading

The project's firmware is based on FreeRTOS, and is set up by multiple concurrent threads that run in parallel. Their execution and basic behaviours can be seen below:

```mermaid
---
config:
  look: neo
  theme: redux-color
---
sequenceDiagram
    participant App as Main Application
    participant Game as Game Thread
    participant Render as Render Thread
    participant Battery as Battery Level Thread
    participant ButtonISR as Button Interrupts
    participant Audio as Audio Thread
    participant Melody as Melody Thread
    participant Video as Video Thread
    participant Decoder as Decoder Thread
    participant MCU as STM32F405

    MCU->>App: Spawn Main Application inside FreeRTOS

    App->>Render: Spawn thread
    App->>Battery: Spawn thread
    App->>Audio: Spawn thread
    App->>Melody: Spawn thread
    App->>Game: Spawn thread

    Render->>MCU: Initialize OLED via SPI + DMA
    Audio->>MCU: Initialize amp via I2S + DMA

    par Frame Math
        loop Single Frame (every ~16ms)
            Game->>Game: Process inputs from input queue
            Game->>Game: Process logic dependent on Screen State
            Game->>Audio: Send any SFX to play, if appropriate
            Game->>App: Update framebuffer (mutexed)
            Game->>Render: Notify frame is ready to be rendered
            Game->>Game: Sleep for ~16ms
            Render->>MCU: Transmit updated framebuffer to SSD1309 via DMA (mutexed FB + waits on semaphored DMA state)
            Render->>Render: Sleep until transfer finished or timed out
            MCU->>Render: Notify transfer finished
        end
    and Audio
        loop Every 2.6ms
            Audio->>Audio: Await notifications from DMA
            MCU->>Audio: Send buffer half-processed notification
            Audio->>Audio: Prepare first half of next buffer based on channel info in 1.3ms
            Audio->>Audio: Await notifications from DMA
            MCU->>Audio: Send buffer fully-processed notification
            Audio->>Audio: Prepare second half of next buffer based on channel info in 1.3ms
        end
    and Melody
        App->>Melody: Tell Melody Controller to play song
        loop Per Song Command (or perpetually if song chose to be looped)
            Melody->>Melody: If song is looped, loop. Otherwise, sleep until notification to play a new song
            Melody->>Melody: Read stored melody info
            loop Until commands run out
                Melody->>Audio: Send play audio command if note is played
                Melody->>Melody: Wait for however long melody says to wait
            end
        end
    and Battery
        loop Every 5s
            Battery->>MCU: Query battery voltage from ADC 10 times with 750ms in between
            Battery->>Battery: Calculate voltage and percentage
            Battery->>App: Update voltage and percentage
        end
    and Buttons
        loop Continuously
            ButtonISR->>Game: Send input to buffer upon rising or falling edge
        end
    and Video
      App->>Decoder: Tell Decoder Controller to start Video Playback
      loop Per Video Command
        par Decoding
          Decoder->>Decoder: Get pointer to whichever block we are inflating
          Decoder->>Decoder: Use ZLib Turbo to inflate current block
          Decoder->>Video: Send inflated block to video when ready
        and Playback
          Video->>Video: Wait for decoded buffer
          Video->>App: Update framebuffer with first frame (mutexed)
          loop Per buffer until "EOF"
            Video->>Video: Sleep for ~33ms
            Video->>App: XOR Framebuffer with new i-frame
          end
          Video->>Decoder: Alert ready to receive new buffer
        end
      end
    end
```

## Controllers
Most of the threads' behaviour (specifically the Melody, Audio and Game Threads) are abstracted away inside of **Controller Classes**. This is the overview of what controllers exist inside those threads, and how they interact with each other. It isn't an expressly formal Design Structure, but the closest I can think is that it's akin to singleton decorators, in a way? Controllers can "engulf" others to add their behaviours to their own, and we only should have one controller instance existing at a time.

```mermaid
---
config:
  look: neo
  theme: redux-color
---
classDiagram
  class GraphicsController {
    +kScreenWidth : uint_fast8_t 
    +kScreenHeight : uint_fast8_t 
    +kFrameBufferSize : uint16_t 
    -_frame_buffer : std::array<uint8_t, kFrameBufferSize>* 
    -_xFrameBufferMutex : SemaphoreHandle_t* 
    +setPixel(const uint_fast8_t x, const uint_fast8_t y, const bool on) : void 
    +unfilledRect(const uint_fast8_t x, const uint_fast8_t y, const uint_fast8_t w, const uint_fast8_t h, const bool on) : void 
    +filledRect(const uint_fast8_t x, const uint_fast8_t y, const uint_fast8_t w, const uint_fast8_t h, const bool on) : void 
  }

  class SpritesController {
    -_sprite_array : std::array<uint8_t, 749> 
    +drawSprite(const uint_fast8_t sprite_id, const uint_fast8_t x, const uint_fast8_t y) : uint8_t 
    +drawBlock(const uint_fast8_t x, const uint_fast8_t y, const Tetromino_t block_type) : bool 
    +drawText(const uint_fast8_t x, const uint_fast8_t y, const char *sentence) : uint_fast8_t 
    +drawNumber(const uint_fast8_t x, const uint_fast8_t y, const int32_t number, const uint_fast8_t base) : uint_fast8_t 
  }

  class TetrominoController {
    +getTetrominoPositions(const uint_fast8_t x, const uint_fast8_t y, const Tetromino_t block_type, const Spin_t spin_state) : void
    +drawTetromino(const uint_fast8_t x, const uint_fast8_t y, const Tetromino_t block_type, const Spin_t spin_state) : void
    +drawPhantomTetromino(const uint_fast8_t x, const uint_fast8_t y, const Tetromino_t block_type, const Spin_t spin_state) : void
  }

  class PlayfieldController {
    -Playfield[22] : uint32_t 
    -Score : uint32_t 
    -Level : uint_fast8_t 
    -TotalLinesCleared : uint32_t 
    -Combo : uint32_t 
    -PieceDropTimeout : uint32_t 
    -seen_pieces : uint_fast8_t 
    -nextPiece : Tetromino_t 
    -currentPiece : Tetromino_t 
    -currentSpinState : Spin_t 
    -pieceXPos : uint_fast8_t 
    -pieceYPos : uint_fast8_t 
    -phantomYPos : uint_fast8_t 
    -stopwatch : int64_t 
    -checkForLineClear : bool 
    -finishedCheck : bool 
    -indexesToShift : std::array<uint_fast8_t, 4> 
    -JLSTZ_Offsets : std::array<int8_t, 40> 
    -I_Offsets : std::array<int8_t, 40> 
    -O_Offsets : std::array<bool, 8> 
    -_graphicsController : GraphicsController* 
    -_spritesController : SpritesController* 
    -_tetrominoController : TetrominoController* 
    -_salt : uint32_t*
    -_audioQueue : QueueHandle_t
    +setBlockState(const uint8_t x, const uint8_t y, const uint8_t block_type) : void 
    +renderPlayfield() : void 
    +renderPlayfieldHeader() : void 
    +pullNewPiece() : Tetromino_t
    +checkForCollision(const Tetromino_t block_type, const Spin_t spin_state, const uint_fast8_t x, const uint_fast8_t y) : bool 
    +rotateCurrentPiece(const bool clockwise) : void 
    +moveCurrentPiece(const bool right) : void 
    +softDropCurrentPiece() : void 
    +hardDropCurrentPiece() : void 
    +setSoftDropStatus(const bool status) : void
    +updatePhantomPiecePosition() : void 
    +initializePlayfield() : void 
    +tickPlayfield(const uint32_t delta_time, const uint32_t elapsed_time) : void 
  }

  class AudioController {
    +playWave(const uint8_t waveId, const uint16_t pitch, const uint16_t time, const uint8_t volume) : void 
    +processAudioCallback(bool secondHalf) : void 
    +sampleToUlValue(const uint32_t waveNumber, const uint32_t note, const uint32_t octave, const uint32_t volume, const uint32_t length) : uint32_t $
    -kChannelNumber : uint32_t $
    -Channels : std::array<SoundChannel_t, kChannelNumber> 
    -lastSample : uint16_t 
    -_audioDMABuffer : std::array<int16_t, 256>*
    -kSineWave : std::array<int16_t, 600> $
    -kAKWF_eguitar_002 : std::array<int16_t, 600> $
    -kAKWF_ebass_061 : std::array<int16_t, 600> $
    -kAKWF_vgame_059 : std::array<int16_t, 600> $
  }

  class MelodyController {
    +playSong(const uint8_t songId, const bool loop) : void 
    +getSongMessage() : uint32_t 
    -currentSong : uint32_t 
    -currentSongMessage : uint32_t 
    -shouldSongLoop : bool 
    -main_song : std::array<uint32_t, 483> $
  }

SpritesController "1" ..> "1" GraphicsController
GraphicsController "1" ..> "1" TetrominoController
PlayfieldController "1" ..> "1" GraphicsController
PlayfieldController "1" ..> "1" SpritesController
PlayfieldController "1" ..> "1" TetrominoController
```

## Sprite Rendering
Given we only have 1MB of flash and 128KB of RAM, the 6x6 px sprites need to take up as little space as possible. Since the OLED display is monochrome, they are stored in binary pixel format.

Given a source bitmap tilemap of all sprites, for each one, a count of their collumns (and therefore their horizontal resolution) is stored, and then each collumn is stored in a byte, with two leftover bits, for a total of 7 bytes per sprite. The actual arrays are programatically calculated and written down via a Python script run at build time.

When rendering sprites, a caret system is used to allow for interchanging and nested text and number rendering by leveraging the column size parameter.

Each sprite, when rendered, is stored in a framebuffer that gets sent via SPI + DMA to the OLED display through a custom SSD1309 driver. Each pixel drawing and render command protects the buffer via a Mutex.

## Melody Playback

Given that a singular second of stored audio would be 96KB given an audio playback of 48kHz and 16-bit amplitude resolution, it is impractical (to put it lightly) to store full scores. As such, given the programmed behaviour of the Audio Task, songs are stored as several "play note" and "sleep" commands calculated from a given MIDI file.

Commands are arranged as follows: a 32-bit word arranged with masks as follows: `0x1F` for note (N), `0xE0` for octave (C), `0xFF00` for length (L), `0x7F0000` for volume (V), `0x7F800000` for waveId (W) for a final word `0b0WWW WWWW WVVV VVVV LLLL LLLL CCCN NNNN`. The most significant bit is restricted to indicate a wait time on stored songs, followed by a 31-bit integer indicating how many milliseconds to sleep before the next command.

This made it such that, for example, the `main_song.mid` file, whose raw uncompressed wave would occupy **3.686MB** for a 38.4s playback, and MIDI file occupies *2.4KB*, takes up around *1.932KB* in flash! It does depend on the arrangement, however, because a 20.5KB piano arrangement MIDI takes up ~25KB. It still beats its theoretical uncompressed size of ~20MB!

## Audio Synthesization

To generate the sound waves for the different instruments, the raw amplitudes of a single wave are stored as 600 16-bit integers spread equidistant across the single wavelength, and a "jump" floating point number is calculated based on the wave's desired pitch and the 48kHz playback. This means each instrument takes up around 1.2KB. Upon playback, we use that number to "jump" across amplitudes during playback to simulate pitch shifting. Each wave has an attack and decay envelope to avoid crackling on consecutive playbacks. The audio system can support up to `kChannelNumber` different waves to be played at the same time. A 256-integer-long buffer is then sent via I2S + Circular DMA to the I2S Breakout, and each half is filled based on ISR routines sent by the DMA coroutine.

## "Random" Piece Generation
Since RTOSs are to be deterministic machines, true randomness isn't recommended. However, it is wished that games don't look the same across gameplays. As such, a custom pseudorandom number generation system was created in addition to a traditional "bag system" wherein each of the 7 pieces are pulled from a metaphorical "bag" before being reshuffled when all of them are pulled. This bag system ensures that no same piece is ever pulled more than twice in a row.

As soon as gameplay begins, a salt is initialized and incremented by 26489 every frame, and set to wrap at 2147457158. To further randomize initial conditions, a random number of pieces are pulled out and discarded, and two pieces are chosen at random. Whenever it is time to choose a piece at (seemingly) random, this salt is added to 3739 times the current ticks since boot for an internal random variable, and then the salt is incremented by 14929. By this nature, under extremely precise and calculated circumstances, gameplay COULD be fully deterministic, but under normal gameplay, it is seemingly random.

## Debugging
Debugging is achieved live on the board using a Segger J-Link EDU! This project is purely for learning's sake, so yay!


## Disclaimer
Tetris® is a registered trademark of The Tetris Company, LLC. This project is not affiliated with or endorsed by The Tetris Company.