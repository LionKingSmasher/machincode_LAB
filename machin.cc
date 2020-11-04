#include <iostream>
#include <cstdio>
#include <cinttypes>

typedef uint_least8_t  u8;
typedef uint_least16_t u16;
typedef uint_least32_t u32;

struct CPU
{
    u16 A, B, C; //A, B, C register
    void init() {A=0; B=0; C=0;}
    void Execute(u8* code);
};

void CPU::Execute(u8* code)
{
    bool chk = false;
    int op = 0;
    u16 *one, *two;
    #define s(o, code) if(o) code;
    #define CHOOSE(u, op, v) \
            switch((u-op)%3)\
            {\
            case 0:\
                one = &v;\
                break;\
            case 1:\
                two = &v;\
                break;\
            default:\
                break;\
            }
    #define LIST(o, in, u, op) \
        o(0x02==in, CHOOSE(u, op, A)) /* one or two <- A register */ \
        o(0x03==in, CHOOSE(u, op, B)) /* one or two <- B register */ \
        o(0x04==in, CHOOSE(u, op, C)) /* one or two <- C register */ \
        o(0x29==in, one = !one; op--)\
        o(0x30==in, *one += *two) /* same as add a, b */ \
        o(0x31==in, *one -= *two) /* same as sub a, b */ \
        o(0x32==in, *one |= *two) /* same as or a, b */  \
        o(0x33==in, *one &= *two) /* same as and a, b */ \
        o(0x34==in, *one ^= *two) /* same as xor a, b */ \
        o(0x80==in, *one = *two)  /* same as mov a, b */ \
    for(u32 i = 0; code[i]!=0; i++)
    {
        if(code[i] == 0x01) { chk = true; op++; }
        if(chk)
        {
            switch((i - op) % 3)
            {
            case 0:
                *one = code[++i];
                break;
            case 1:
                *two = code[++i];
                break;
            default:
                break;
            }
            chk = false;
        }
        else
        {
            LIST(s, code[i], i, op);
        }
    }
}

int main()
{
    CPU cpu;
    cpu.init();
    u8 run[] = {0x02, 0x01, 0x11, 0x80, /* mov A, 11 */
                0x03, 0x02, 0x80, /* mov B, A */
                0x03, 0x02, 0x31, /* sub B, A */
                0x04, 0x02, 0x32, /* or C, A  */
                0x02, 0x02, 0x34};
    cpu.Execute(run);
    printf(" A = %x\n B = %x\n C = %x\n", cpu.A, cpu.B, cpu.C);
}


