#include "graphics/vbe.h"

int ramIndex = 0;

char* ram[] =
{
    0x10, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F,
    0x72, 0x6C, 0x64, 0x21, 0x00, 0x00, 0x00, 0x00
}; 

char* registers[32];
char instruction[8];

void GetIntructionFromRam(int index, char* instruction) {
    for (int i = 0; i < 8; i++) {
        instruction[i] = ram[index + i];
    }
}

void CORE_FindFunction(char* instruction) {
    switch(instruction[0] % 16) {
        case 0x00:
            EndProgram();
            break;
        case 0x01:
            GotoPointer();
            break;
    }
}

void SetPointerToRegister()
{
    char registerIndex = instruction[1];

    for (int i = 0; i < 4; i++)
    {
        registers[registerIndex * 4 + i] = instruction[i + 2];
    }
}

void GetBytesFromInstruction(char *rawData, int index)
{
            

            for (int i = 0; i < 4; i++)
            {
                rawData[i] = instruction[index + i];
            }

            return rawData;
}

void CopyFromRegister()
{
    K_Print(">run<");
    char registerIndex = instruction[1];
    char rawData[4];
    GetBytesFromInstruction(rawData, 2);
    int pointer = BitToInt(rawData);
    //printf(">%d<", pointer);

    for (int i = 0; i < 4; i++)
    {
         ram[pointer + i] = registers[registerIndex * 4 + i];
    }
}

void CopyToRegister()
{
    for(int i = 0; i < sizeof(instruction); i++) {
        //printf("instruction %d: %x\n", i, instruction[i]);
    }

    char registerIndex = instruction[1];
    char rawData[4];
    GetBytesFromInstruction(rawData, 2);
    for(int i = 0; i < sizeof(rawData); i++) {
        //printf("raw data: %x", rawData[i]);
    }
    int pointer = (int)rawData;

    for (int i = 0; i < 4; i++)
    {
        registers[registerIndex * 4 + i] = ram[pointer + i];
    }
}

void RAM_FindFunction(char* instruction) {
    switch(instruction[0] % 16) {
        case 0x00:
            SetPointerToRegister();
            break;
        case 0x01:
            CopyFromRegister();
            break;
        case 0x02:
            CopyToRegister();
            break;
    }
}

int BitToInt(char rawData[4]) {
    int i = (rawData[3] << 24) | (rawData[2] << 16) | (rawData[1] << 8) | (rawData[0]);
    
    return i;
}

void IntToBytes(char bytes[4], int n) {
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
}

void PrintString() {
    int index = instruction[1];

    char rawData[4];

    GetBytesFromRegister(rawData, index);

    int pointer = BitToInt(rawData);

    K_SetCursor(5, K_GetCursorY());

    for(; ram[pointer] != 0; pointer++) {
        K_Print(&ram[pointer]);
    }

    K_NewLine();
}

void IO_FindFunction(char* instruction) {
    switch(instruction[0] % 16) {
        case 0x00:
            PrintString();
            break;
        case 0x01:
            GotoPointer();
            break;
    }
}

void MATH_FindFunction(char* instruction) {
    switch(instruction[0] % 16) {
        case 0x00:
            CalculateIntegers(); // possibly very broken, not tested
            break;
    }
}

void CalculateIntegers() {
    char bytes1[4];
    char bytes2[4];
    
    GetBytesFromRegister(bytes1, 0);
    GetBytesFromRegister(bytes2, 1);

    int num1 = BitToInt(bytes1);
    int num2 = BitToInt(bytes1);

    switch (instruction[1]) 
    {
        case 0:
            num1 += num2;
            break;
        case 1:
            num1 -= num2;
            break;
        case 2:
            num1 *= num2;
            break;
        case 3:
            num1 /= num2;
            break;
        case 4:
            num1 %= num2;
            break;
    }

    char result[4];

    IntToBytes(result, num1);

    for (int i = 0; i < sizeof(result); i++)
    {
        registers[i] = result[i];
    }
}

void GetBytesFromRegister(char rawData[4], int rIndex)
{
    for (int i = 0; i < 4; i++)
    {
        rawData[i] = registers[rIndex * 4 + i];
    }

    return rawData;
}

void EndProgram() {
    //printf("end");
    ramIndex = sizeof(ram);
}

void GotoPointer() {
    int pointer = instruction[1];

    char rawData[4];

    GetBytesFromRegister(rawData, pointer);

    int index = BitToInt(rawData);

    ramIndex = index;
}

void FindFunction(char* instruction) {
    switch(instruction[0] / 16) {
        case 0x00:
            CORE_FindFunction(instruction);
            break;
        case 0x01:
            RAM_FindFunction(instruction);
            break;
        case 0x02:
            IO_FindFunction(instruction);
            break;
        case 0x03:
            MATH_FindFunction(instruction);
            break;
    }
}

void RunNext() {
    GetIntructionFromRam(ramIndex, instruction);
    FindFunction(instruction);
    ramIndex += 8;
}

void RunAsborbaProgram() {
    for(; ramIndex < sizeof(ram);) {
        RunNext();
    }

    //printf("\n---Registry Map---\n");

    /*for (int i = 0; i < sizeof(registers); i++)
    {
        if (i % 8 == 0 && i > 4)
            //printf("\n");

        char b = registers[i];
        //printf("0x%02X ", b);
    }

    //printf("\n\n---RAM Map---\n");

    for (int i = 0; i < sizeof(ram); i++)
    {
        if (i % 8 == 0 && i > 4)
            //printf("\n");

        char b = ram[i];
        //printf("0x%02X ", b);
    }*/
}