#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*** DATA ***/

const unsigned int START_ADDRESS = 0x200;
/* ADDITIONAL INFO ABOUT THE MEMORY
 0x000-0x1FF: Originally reserved for the CHIP-8 interpreter, but in our modern emulator we will just never write to or read from that area. Except for…
 0x050-0x0A0: Storage space for the 16 built-in characters (0 through F), which we will need to manually put into our memory because ROMs will be looking for those characters.
 0x200-0xFFF: Instructions from the ROM will be stored starting at 0x200, and anything left after the ROM’s space is free to use.
 */


inline uint8_t VIDEO_SCALE = 10;
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

typedef struct CHIP8
{
    unsigned short opcode;

    unsigned char registers[16];
    unsigned char memory[4096]; // 4k memory 1024*4
    unsigned short index; // index register
    unsigned short pc; // program counter
    unsigned short stack[16];
    unsigned short sp; // stack pointer

    // unsigned char video[VIDEO_WIDTH * VIDEO_HEIGHT]; // 64*32 monochrome display size
    unsigned char video[VIDEO_WIDTH * VIDEO_HEIGHT]; // 64*32 monochrome display size
    unsigned char keypad[16]; // Chip 8 had 16 key inputs
    // Keypad       Keyboard
    // +-+-+-+-+    +-+-+-+-+
    // |1|2|3|C|    |1|2|3|4|
    // +-+-+-+-+    +-+-+-+-+
    // |4|5|6|D|    |Q|W|E|R|
    // +-+-+-+-+ => +-+-+-+-+
    // |7|8|9|E|    |A|S|D|F|
    // +-+-+-+-+    +-+-+-+-+
    // |A|0|B|F|    |Z|X|C|V|
    // +-+-+-+-+    +-+-+-+-+

    //audio
    unsigned char delay_timer;
    unsigned char sound_timer;
} CHIP8;

#define FONTSET_SIZE 80
#define FONTSET_START_ADDRESS 0x50

inline uint8_t fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};


/*** OPCODES ***/
/*
// Assume the following:
memory[pc]     == 0xA2
memory[pc + 1] == 0xF0

// OPCODE LIST (35):
// $1nnn
// $2nnn
// $3xkk
// $4xkk
// $5xy0
// $6xkk
// $7xkk
// $9xy0

// $Annn
// $Bnnn
// $Cxkk
// $Dxyn

// $8xy0
// $8xy1
// $8xy2
// $8xy3
// $8xy4
// $8xy5
// $8xy6
// $8xy7
// $8xyE

// $00E0
// $00EE

// $ExA1
// $Ex9E

// $Fx07
// $Fx0A
// $Fx15
// $Fx18
// $Fx1E
// $Fx29
// $Fx33
// $Fx55
// $Fx65
*/

#define VX(x)   ((x & 0x0F00) >> 8)
#define VY(x)   ((x & 0x00F0) >> 4)
#define N(x)   (x & 0x000F)
#define KK(x)  (x & 0x00FF)
#define NNN(x) (x & 0x0FFF)

inline void OP_00E0(CHIP8* chip8)
{
    // 00E0: CLS
    // Clear the display.
    memset(chip8->video, 0, sizeof(chip8->video));
}

inline void OP_00EE(CHIP8* chip8)
{
    // 00EE: RET
    // Return from a subroutine
    --chip8->sp;
    chip8->pc = chip8->stack[chip8->sp];
}

inline void OP_1nnn(CHIP8* chip8)
{
    // 1nnn: JP addr
    // Jump to location nnn.
    // The interpreter sets the program counter to nnn.

    uint16_t address = chip8->opcode & 0x0FFFu;

    chip8->pc = address;
}

inline void OP_2nnn(CHIP8* chip8)
{
    // 2nnn - CALL addr
    // Call subroutine at nnn.

    uint16_t address = chip8->opcode & 0x0FFFu;

    chip8->stack[chip8->sp] = chip8->pc;
    ++chip8->sp;
    chip8->pc = address;
}


inline void OP_3xkk(CHIP8* chip8)
{
    // 3xkk - SE Vx, byte
    // Skip next instruction if Vx = kk.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip8->opcode & 0x00FFu;

    if (chip8->registers[Vx] == byte)
    {
        chip8->pc += 2;
    }
}

inline void OP_4xkk(CHIP8* chip8)
{
    // 4xkk - SNE Vx, byte
    // Skip next instruction if Vx != kk.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip8->opcode & 0x00FFu;

    if (chip8->registers[Vx] != byte)
    {
        chip8->pc += 2;
    }
}

inline void OP_5xy0(CHIP8* chip8)
{
    // 5xy0 - SE Vx, Vy
    // Skip next instruction if Vx = Vy. uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;


    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    if (chip8->registers[Vx] == chip8->registers[Vy])
    {
        chip8->pc += 2;
    }
}

inline void OP_6xkk(CHIP8* chip8)
{
    // 6xkk - LD Vx, byte
    // Set Vx = kk.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip8->opcode & 0x00FFu;

    chip8->registers[Vx] = byte;
}

inline void OP_7xkk(CHIP8* chip8)
{
    // 7xkk - ADD
    // Vx, byte
    // Set Vx = Vx + kk.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip8->opcode & 0x00FFu;

    chip8->registers[Vx] += byte;
}

inline void OP_8xy0(CHIP8* chip8)
{
    // 8xy0 - LD
    // Vx, Vy
    // Set Vx = Vy.uint8_t

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    chip8->registers[Vx] = chip8->registers[Vy];
}

inline void OP_8xy1(CHIP8* chip8)
{
    // 8xy1 - OR Vx, Vy
    // Set Vx = Vx OR Vy.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    chip8->registers[Vx] |= chip8->registers[Vy];
}

inline void OP_8xy2(CHIP8* chip8)
{
    // 8xy2 - AND Vx, Vy
    // Set Vx = Vx AND Vy.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    chip8->registers[Vx] &= chip8->registers[Vy];
}

inline void OP_8xy3(CHIP8* chip8)
{
    // 8xy3 - XOR Vx, Vy
    // Set Vx = Vx XOR Vy.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    chip8->registers[Vx] ^= chip8->registers[Vy];
}


inline void OP_8xy4(CHIP8* chip8)
{
    // 8xy4 - ADD Vx, Vy
    // Set Vx = Vx + Vy, set VF = carry.

    // The values of Vx and Vy are added together.
    // If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
    // Only the lowest 8 bits of the result are kept, and stored in Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    uint16_t sum = chip8->registers[Vx] + chip8->registers[Vy];

    if (sum > 255U)
    {
        chip8->registers[0xF] = 1;
    }
    else
    {
        chip8->registers[0xF] = 0;
    }

    chip8->registers[Vx] = sum & 0xFFu;
}

inline void OP_8xy5(CHIP8* chip8)
{
    // 8xy5 - SUB Vx, Vy
    // Set Vx = Vx - Vy, set VF = NOT borrow.
    // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    if (chip8->registers[Vx] > chip8->registers[Vy])
    {
        chip8->registers[0xF] = 1;
    }
    else
    {
        chip8->registers[0xF] = 0;
    }

    chip8->registers[Vx] -= chip8->registers[Vy];
}

inline void OP_8xy6(CHIP8* chip8)
{
    // 8xy6 - SHR Vx
    // Set Vx = Vx SHR 1.
    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    // Save LSB in VF
    chip8->registers[0xF] = (chip8->registers[Vx] & 0x1u);

    chip8->registers[Vx] >>= 1;
}

inline void OP_8xy7(CHIP8* chip8)
{
    // 8xy7 - SUBN Vx, Vy
    // Set Vx = Vy - Vx, set VF = NOT borrow.
    // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    if (chip8->registers[Vy] > chip8->registers[Vx])
    {
        chip8->registers[0xF] = 1;
    }
    else
    {
        chip8->registers[0xF] = 0;
    }

    chip8->registers[Vx] = chip8->registers[Vy] - chip8->registers[Vx];
}


inline void OP_8xyE(CHIP8* chip8)
{
    // 8xyE - SHL Vx {, Vy}
    // Set Vx = Vx SHL 1.
    // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    // Save MSB in VF
    chip8->registers[0xF] = (chip8->registers[Vx] & 0x80u) >> 7u;

    chip8->registers[Vx] <<= 1;
}

inline void OP_9xy0(CHIP8* chip8)
{
    // 9xy0 - SNE Vx, Vy
    // Skip next instruction if Vx != Vy.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

    if (chip8->registers[Vx] != chip8->registers[Vy])
    {
        chip8->pc += 2;
    }
}

inline void OP_Annn(CHIP8* chip8)
{
    // Annn - LD I, addr
    // Set I = nnn.

    uint16_t address = chip8->opcode & 0x0FFFu;

    chip8->index = address;
}

inline void OP_Bnnn(CHIP8* chip8)
{
    // Bnnn - JP V0, addr
    // Jump to location nnn + V0.

    uint16_t address = chip8->opcode & 0x0FFFu;

    chip8->pc = chip8->registers[0] + address;
}

inline void OP_Cxkk(CHIP8* chip8)
{
    // Cxkk - RND Vx, byte
    // Set Vx = random byte AND kk.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip8->opcode & 0x00FFu;

    //TODO: use a different rand on the bytes
    chip8->registers[Vx] = rand() & byte;
}

inline void OP_Dxyn(CHIP8* chip8)
{
    // Dxyn - DRW Vx, Vy, nibble
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;
    uint8_t height = chip8->opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = chip8->registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = chip8->registers[Vy] % VIDEO_HEIGHT;

    chip8->registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = chip8->memory[chip8->index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint8_t* screenPixel = &chip8->video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            // Sprite pixel is on
            if (spritePixel)
            {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFF)
                {
                    chip8->registers[0xF] = 1;
                }

                // XOR with the sprite pixel
                *screenPixel ^= 0xFF;
            }
        }
    }

    for (int i = 0; i < VIDEO_WIDTH*VIDEO_HEIGHT; i++) {
        chip8->video[i] = chip8->video[i] ? 255 : 0;
    }


    // uint8_t target_v_reg_x = (chip8->opcode & 0x0F00) >> 8;
    // uint8_t target_v_reg_y = (chip8->opcode & 0x00F0) >> 4;
    // uint8_t sprite_height = chip8->opcode & 0x000F;
    // uint8_t x_location = chip8->registers[target_v_reg_x];
    // uint8_t y_location = chip8->registers[target_v_reg_y];
    // uint8_t pixel;
    //
    // // Reset collision register to FALSE
    // chip8->registers[0xF] = 0;
    // for (int y_coordinate = 0; y_coordinate < sprite_height; y_coordinate++) {
    //     pixel = chip8->memory[chip8->index + y_coordinate];
    //     for (int x_coordinate = 0; x_coordinate < 8; x_coordinate++) {
    //         if ((pixel & (0x80 >> x_coordinate)) != 0) {
    //             if (chip8->video[((y_location + y_coordinate) * VIDEO_WIDTH) +  (x_location + x_coordinate)] == 1) {
    //                 chip8->registers[0xF] = 1;
    //             }
    //             chip8->video[((y_location + y_coordinate) * VIDEO_WIDTH) +  (x_location + x_coordinate)] ^= 1;
    //         }
    //     }
    // }
}

inline void OP_Ex9E(CHIP8* chip8)
{
    // Ex9E - SKP Vx
    // Skip next instruction if key with the value of Vx is pressed.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    uint8_t key = chip8->registers[Vx];

    if (chip8->keypad[key])
    {
        chip8->pc += 2;
    }
}

inline void OP_ExA1(CHIP8* chip8)
{
    // ExA1 - SKNP Vx
    // Skip next instruction if key with the value of Vx is not pressed.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    uint8_t key = chip8->registers[Vx];

    if (!chip8->keypad[key])
    {
        chip8->pc += 2;
    }
}

inline void OP_Fx07(CHIP8* chip8)
{
    // Fx07 - LD Vx, DT
    // Set Vx = delay timer value.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    chip8->registers[Vx] = chip8->delay_timer;
}

inline void OP_Fx0A(CHIP8* chip8)
{
    // Fx0A - LD Vx, K
    // Wait for a key press, store the value of the key in Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    if (chip8->keypad[0])
    {
        chip8->registers[Vx] = 0;
    }
    else if (chip8->keypad[1])
    {
        chip8->registers[Vx] = 1;
    }
    else if (chip8->keypad[2])
    {
        chip8->registers[Vx] = 2;
    }
    else if (chip8->keypad[3])
    {
        chip8->registers[Vx] = 3;
    }
    else if (chip8->keypad[4])
    {
        chip8->registers[Vx] = 4;
    }
    else if (chip8->keypad[5])
    {
        chip8->registers[Vx] = 5;
    }
    else if (chip8->keypad[6])
    {
        chip8->registers[Vx] = 6;
    }
    else if (chip8->keypad[7])
    {
        chip8->registers[Vx] = 7;
    }
    else if (chip8->keypad[8])
    {
        chip8->registers[Vx] = 8;
    }
    else if (chip8->keypad[9])
    {
        chip8->registers[Vx] = 9;
    }
    else if (chip8->keypad[10])
    {
        chip8->registers[Vx] = 10;
    }
    else if (chip8->keypad[11])
    {
        chip8->registers[Vx] = 11;
    }
    else if (chip8->keypad[12])
    {
        chip8->registers[Vx] = 12;
    }
    else if (chip8->keypad[13])
    {
        chip8->registers[Vx] = 13;
    }
    else if (chip8->keypad[14])
    {
        chip8->registers[Vx] = 14;
    }
    else if (chip8->keypad[15])
    {
        chip8->registers[Vx] = 15;
    }
    else
    {
        chip8->pc -= 2;
    }
}

inline void OP_Fx15(CHIP8* chip8)
{
    // Fx15 - LD DT, Vx
    // Set delay timer = Vx.


    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    chip8->delay_timer = chip8->registers[Vx];
}

inline void OP_Fx18(CHIP8* chip8)
{
    // Fx18 - LD ST, Vx
    // Set sound timer = Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    chip8->sound_timer = chip8->registers[Vx];
}

inline void OP_Fx1E(CHIP8* chip8)
{
    // Fx1E - ADD I, Vx
    // Set I = I + Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    chip8->index += chip8->registers[Vx];
}

inline void OP_Fx29(CHIP8* chip8)
{
    // Fx29 - LD F, Vx
    // Set I = location of sprite for digit Vx.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t digit = chip8->registers[Vx];

    chip8->index = FONTSET_START_ADDRESS + (5 * digit);
}

inline void OP_Fx33(CHIP8* chip8)
{
    // Store BCD representation of Vx in memory locations I, I+1, and I+2.
    // The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
    // the tens digit at location I+1, and the ones digit at location I+2.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
    uint8_t value = chip8->registers[Vx];

    // Ones-place
    chip8->memory[chip8->index + 2] = value % 10;
    value /= 10;

    // Tens-place
    chip8->memory[chip8->index + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    chip8->memory[chip8->index] = value % 10;
}

inline void OP_Fx55(CHIP8* chip8)
{
    // Fx55 - LD [I], Vx
    // Store registers V0 through Vx in memory starting at location I.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        chip8->memory[chip8->index + i] = chip8->registers[i];
    }
}

inline void OP_Fx65(CHIP8* chip8)
{
    // Fx65 - LD Vx, [I]
    // Read registers V0 through Vx from memory starting at location I.

    uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        chip8->registers[i] = chip8->memory[chip8->index + i];
    }
}


/*** FUNCTION ***/

inline CHIP8* chip8_init()
{
    // Initialize registers and memory once
    CHIP8* chip8 = (CHIP8 *) malloc(sizeof(CHIP8));

    //init program counter
    chip8->pc = START_ADDRESS;
    //zero index anx stack pointer
    chip8->index = 0;
    chip8->sp = 0;

    //zero the display
    memset(chip8->video, 0, sizeof(chip8->video));

    //load font into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; i++)
    {
        chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
    return chip8;
}

inline void chip8_free(CHIP8* chip8)
{
    free(chip8);
}

inline bool chip8_load_rom(CHIP8* chip8, const char* filename)
{
    //open and read in the binary data from the file

    FILE* rom_file = fopen(filename, "rb"); // read binary flag
    if (rom_file)
    {
        //go to the end of the file
        fseek(rom_file, 0L, SEEK_END);

        //get the location/size of the file
        unsigned int rom_size = ftell(rom_file);

        //go back to the start
        //fseek(rom_file, 0L, SEEK_SET);
        // better way of doing it
        rewind(rom_file);

        //read file into the buffer
        //buffer, size, count, file pointer
        unsigned char buffer[rom_size];

        fread(&buffer, rom_size, 1, rom_file);

        //close file
        fclose(rom_file);

        //put buffer memory into the chip8's memory
        for (int i = 0; i < rom_size; i++)
        {
            chip8->memory[START_ADDRESS + i] = buffer[i];
        }
        printf("LOADED ROM FILE SUCCESSFUL\n");

        return true;
    }


    printf("ERROR CANNOT READ ROM FILE\n");
    fclose(rom_file);
    return false;
}

inline void chip8_cycle(CHIP8* chip8)
{
    /***  Fetch Opcode ***/
    chip8->opcode = (chip8->memory[chip8->pc] << 8u) | chip8->memory[chip8->pc + 1];
    // Increment the PC before we execute anything
    chip8->pc += 2;

    printf("Opcode: %x \n", chip8->opcode);
    printf("Program Counter: %x \n", chip8->pc);
    printf("I: %x \n", chip8->index);

    /***  Decode Opcode and Execute Opcode ***/
    switch (chip8->opcode & 0xF000)
    {
        case 0x0000:
            switch (chip8->opcode & 0x00FF)
            {
                case 0x00E0:
                    OP_00E0(chip8);
                    break;
                case 0x00EE:
                    OP_00EE(chip8);
                    break;
                default:
                    printf("Unknown opcode 0x0: 0x%X\n", chip8->opcode);
                    break;
            }
            break;
        case 0x1000: //1nnn
            OP_1nnn(chip8);
            break;
        case 0x2000:
            OP_2nnn(chip8);
            break;
        case 0x3000:
            OP_3xkk(chip8);
            break;
        case 0x4000:
            OP_4xkk(chip8);
            break;
        case 0x5000:
            OP_5xy0(chip8);
            break;
        case 0x6000:
            OP_6xkk(chip8);
            break;
        case 0x7000:
            OP_7xkk(chip8);
            break;
        case 0x8000:
            switch (chip8->opcode & 0x000F)
            {
                case 0x0000:
                    OP_8xy0(chip8);
                    break;
                case 0x0001:
                    OP_8xy1(chip8);
                    break;
                case 0x0002:
                    OP_8xy2(chip8);
                    break;
                case 0x0003:
                    OP_8xy3(chip8);
                    break;
                case 0x0004:
                    OP_8xy4(chip8);
                    break;
                case 0x0005:
                    OP_8xy5(chip8);
                    break;
                case 0x0006:
                    OP_8xy6(chip8);
                    break;
                case 0x0007:
                    OP_8xy7(chip8);
                    break;
                case 0x000E:
                    OP_8xyE(chip8);
                    break;
                default:
                    printf("Unknown opcode 0x8: 0x%X\n", chip8->opcode);
                    break;
            }
            break;

        case 0x9000:
            OP_9xy0(chip8);
            break;
        case 0xA000:
            OP_Annn(chip8);
            break;
        case 0xB000:
            OP_Bnnn(chip8);
            break;
        case 0xC000:
            OP_Cxkk(chip8);
            break;
        case 0xD000:
            OP_Dxyn(chip8);
            break;
        case 0xE000: //Exkk
            switch (chip8->opcode & 0x00FF)
            {
                case 0x009E: //Ex9E
                    OP_Ex9E(chip8);
                    break;
                case 0x00A1: //ExA1
                    OP_ExA1(chip8);
                    break;
                default:
                    printf("Unknown opcode E: 0x%X\n", chip8->opcode);
                    break;
            }
            break;
        case 0xF000:
            switch (chip8->opcode & 0x00FF)
            {
                case 0x0007: //$Fx07
                    OP_Fx07(chip8);
                    break;
                case 0x000A: // $Fx0A
                    OP_Fx0A(chip8);
                    break;
                case 0x0015: // $Fx15
                    OP_Fx15(chip8);
                    break;
                case 0x0018: // $Fx18
                    OP_Fx18(chip8);
                    break;
                case 0x001E: // $Fx1E
                    OP_Fx1E(chip8);
                    break;
                case 0x0029: // $Fx29
                    OP_Fx29(chip8);
                    break;
                case 0x0033: // $Fx33
                    OP_Fx33(chip8);
                    break;
                case 0x0055: // $Fx55
                    OP_Fx55(chip8);
                    break;
                case 0x0065: // $Fx65
                    OP_Fx65(chip8);
                    break;
                default:
                    printf("Unknown opcode F: 0x%X\n", chip8->opcode);
                    break;
            }
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", chip8->opcode);
            break;
    }


    /*** Update timers ***/


    // Decrement the delay timer if it's been set
    if (chip8->delay_timer > 0)
    {
        //pre decrement
        --chip8->delay_timer;
    }

    // Decrement the sound timer if it's been set
    if (chip8->sound_timer > 0)
    {
        //TODO: testing for now, replace with audio
        if (chip8->sound_timer == 1)
            printf("BEEP!\n");
        --chip8->sound_timer;
    }
}


#endif //CHIP8_H
