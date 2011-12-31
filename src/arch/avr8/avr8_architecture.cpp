#include "avr8_architecture.hpp"
#include <sstream>
#include <medusa/medusa.hpp>

char const *Avr8Architecture::m_RegName[] =
  {
    "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
    "X",   "Y",   "Z",   "X+",  "-X",  "Y+",  "-Y",  "Z+",
    "-Z"
  };

bool Avr8Architecture::Translate(Address const& rVirtAddr, TAddress& rPhysAddr)
{
  return true;
}

bool Avr8Architecture::Disassemble(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1;
  bool Result;

  rBinStrm.Read(Address + 1, Opcode1);

  switch (Opcode1 & 0xf0)
    {
    case 0x00:
      Result = Insn_0xxx(rBinStrm, Address, rInsn);  break;
    case 0x10:
      Result = Insn_1xxx(rBinStrm, Address, rInsn);  break;
    case 0x20:
      Result = Insn_2xxx(rBinStrm, Address, rInsn);  break;
    case 0x30: case 0x40: case 0x50: case 0x60: case 0x70:
      Result = Insn_3xxx(rBinStrm, Address, rInsn);  break;
    case 0x80:
      Result = Insn_8xxx(rBinStrm, Address, rInsn);  break;
    case 0x90:
      Result = Insn_9xxx(rBinStrm, Address, rInsn);  break;
    case 0xa0:
      Result = false; break;
    case 0xb0:
      Result = Insn_bxxx(rBinStrm, Address, rInsn);  break;
    case 0xc0:
      Result = Insn_Rjmp(rBinStrm, Address, rInsn);  break;
    case 0xd0:
      Result = Insn_Rcall(rBinStrm, Address, rInsn); break;
    case 0xe0:
      Result = Insn_Ldi(rBinStrm, Address, rInsn);   break;
    case 0xf0:
      Result = Insn_fxxx(rBinStrm, Address, rInsn);  break;
    }

  if (Result == true)
  {
    FormatOperand(rInsn.FirstOperand(),  Address);
    FormatOperand(rInsn.SecondOperand(), Address);
    FormatOperand(rInsn.ThirdOperand(),  Address);
    FormatOperand(rInsn.FourthOperand(), Address);
  }
  return Result;
}

void Avr8Architecture::FormatOperand(Operand& Op, TAddress Address)
{
  std::ostringstream oss;

  if (Op.Type() & O_REG)
    if (Op.Reg() >= 0 && Op.Reg() <= (sizeof(m_RegName) / sizeof(*m_RegName)))
      oss << m_RegName[Op.Reg()];

  if (Op.Type() & O_REL)
    oss << std::hex << std::showpos << static_cast<s16>(Op.Value() & 0xffff);

  if (Op.Type() & O_IMM)
    oss << "0x" << std::hex << Op.GetValue();

  if (Op.Type() & O_MEM  && Op.Type() & O_DISP)
    oss << "+" << std::hex << Op.GetValue();

  Op.SetName(oss.str().c_str());
}

// SKELETON
bool Avr8Architecture::Insn_(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Opcode() = AVR8_Invalid_Insn;
  rInsn.Length() = 2;
  rInsn.SetName("");

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = 0;
  FrstOperand.Value()  = 0;
  FrstOperand.Reg()    = (Opcode1 & 0x01) << 4 | Opcode2 >> 4;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = 0;
  ScdOperand.Value()  = 0;
  ScdOperand.Reg()    = (Opcode1 & 0x02) << 3 | (Opcode2 & 0x0f);

  return true;
}

bool Avr8Architecture::Insn_fxxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  bool Result;

  rBinStrm.Read(Address    , Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);
  rInsn.Length() = 2;

  if ((Opcode1 & 0x0c) > 0x04)
    {
      Result = false;
      /*
        BLD    1111-100d-dddd-0bbb
        BST    1111-101d-dddd-0bbb
        SBRC   1111-110r-rrrr-0bbb
        SBRS   1111-111r-rrrr-0bbb
      */
    }
  else
    {
      Result = true;
      rInsn.OperationType() = Instruction::OpJump;

      if (!(Opcode1 & 0x0c))
        {
          switch (Opcode2 & 0x07)
            {
            case 0x00: rInsn.Opcode() = AVR8_Brlo; rInsn.SetName("brlo"); rInsn.SetCondition(AVR8_Cond_Lo); break;
            case 0x01: rInsn.Opcode() = AVR8_Breq; rInsn.SetName("breq"); rInsn.SetCondition(AVR8_Cond_Eq); break;
            case 0x02: rInsn.Opcode() = AVR8_Brmi; rInsn.SetName("brmi"); rInsn.SetCondition(AVR8_Cond_Mi); break;
            case 0x03: rInsn.Opcode() = AVR8_Brvs; rInsn.SetName("brvs"); rInsn.SetCondition(AVR8_Cond_Vs); break;
            case 0x04: rInsn.Opcode() = AVR8_Brlt; rInsn.SetName("brlt"); rInsn.SetCondition(AVR8_Cond_Lt); break;
            case 0x05: rInsn.Opcode() = AVR8_Brhs; rInsn.SetName("brhs"); rInsn.SetCondition(AVR8_Cond_Hs); break;
            case 0x06: rInsn.Opcode() = AVR8_Brts; rInsn.SetName("brts"); rInsn.SetCondition(AVR8_Cond_Ts); break;
            case 0x07: rInsn.Opcode() = AVR8_Brie; rInsn.SetName("brie"); rInsn.SetCondition(AVR8_Cond_Ie); break;
            }
        }
      else // ((Opcode1 & 0x0c) == 0x04)
        {
          switch (Opcode2 & 0x07)
            {
            case 0x00: rInsn.Opcode() = AVR8_Brsh; rInsn.SetName("brsh"); rInsn.SetCondition(AVR8_Cond_Sh); break;
            case 0x01: rInsn.Opcode() = AVR8_Brne; rInsn.SetName("brne"); rInsn.SetCondition(AVR8_Cond_Ne); break;
            case 0x02: rInsn.Opcode() = AVR8_Brpl; rInsn.SetName("brpl"); rInsn.SetCondition(AVR8_Cond_Pl); break;
            case 0x03: rInsn.Opcode() = AVR8_Brvc; rInsn.SetName("brvc"); rInsn.SetCondition(AVR8_Cond_Vc); break;
            case 0x04: rInsn.Opcode() = AVR8_Brge; rInsn.SetName("brge"); rInsn.SetCondition(AVR8_Cond_Ge); break;
            case 0x05: rInsn.Opcode() = AVR8_Brhc; rInsn.SetName("brhc"); rInsn.SetCondition(AVR8_Cond_Hc); break;
            case 0x06: rInsn.Opcode() = AVR8_Brtc; rInsn.SetName("brtc"); rInsn.SetCondition(AVR8_Cond_Tc); break;
            case 0x07: rInsn.Opcode() = AVR8_Brid; rInsn.SetName("brid"); rInsn.SetCondition(AVR8_Cond_Id); break;
            }
        }

      Operand& FrstOperand = rInsn.FirstOperand();
      FrstOperand.Type()   = O_REL;
      // Get the 7-bit signed value.
      FrstOperand.Value() = ((Opcode1 & 0x03) << 5 | Opcode2 >> 3);
      // Dirty sign-extension from 7-bit to 8-bit (and then implicitly to sizeof(Value)).
      if ((Opcode1 & 0x02))
        FrstOperand.Value() = (s8) (((u8)FrstOperand.Value()) | (u8)0x80);
      // This instruction uses program memory (not data memory).
      // On avr8, program memory is not byte addressable but word addressable.
      FrstOperand.Value() *= 2;
    }

  return Result;
}

bool Avr8Architecture::Insn_1xxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  u8 Operand1, Operand2;

  rBinStrm.Read(Address    , Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);
  rInsn.Length() = 2;

  switch (Opcode1 & 0x0c)
    {
    case 0x00: rInsn.Opcode() = AVR8_Cpse; rInsn.SetName("cpse"); break;
    case 0x04: rInsn.Opcode() = AVR8_Cp;   rInsn.SetName("cp");   break;
    case 0x08: rInsn.Opcode() = AVR8_Sub;  rInsn.SetName("sub");  break;
    case 0x0c: rInsn.Opcode() = AVR8_Adc;  rInsn.SetName("adc");  break;
    }

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = (Opcode1 & 0x01) << 4 | Opcode2 >> 4;

  Operand& ScdOperand  = rInsn.SecondOperand();
  ScdOperand.Type()    = O_REG8;
  ScdOperand.Reg()     = (Opcode1 & 0x02) << 3 | (Opcode2 & 0x0f);

  return true;
}

bool Avr8Architecture::Insn_0xxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  u8 Operand1, Operand2;
  bool Result;

  rBinStrm.Read(Address    , Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);
  rInsn.Length() = 2;

  if (!(Opcode1 & 0x0c))
    {
      switch (Opcode1)
        {
        case 0x00: rInsn.Opcode() = AVR8_Nop;  rInsn.SetName("nop");        break;
        case 0x01: rInsn.Opcode() = AVR8_Movw; rInsn.SetName("movw");
          Operand1 = (Opcode2 >> 4) * 2; Operand2 = (Opcode2 & 0x0f) * 2;   break;
        case 0x02: rInsn.Opcode() = AVR8_Muls; rInsn.SetName("muls");
          Operand1 = (Opcode2 >> 4) + 16; Operand2 = (Opcode2 & 0x0f) + 16; break;
        case 0x03:
          switch (Opcode2 & 0x88)
            {
            case 0x00: rInsn.Opcode() = AVR8_Mulsu;  rInsn.SetName("mulsu");  break;
            case 0x08: rInsn.Opcode() = AVR8_Fmul;   rInsn.SetName("fmul");   break;
            case 0x80: rInsn.Opcode() = AVR8_Fmuls;  rInsn.SetName("fmuls");  break;
            case 0x88: rInsn.Opcode() = AVR8_Fmulsu; rInsn.SetName("fmulsu"); break;
            }
          Opcode2 &= ~0x88;
          Operand1 = 16 + (Opcode2 >> 4);
          Operand2 = 16 + (Opcode2 & 0x0f);
          break;
        }

      if (Opcode1) // Operands except for nop
        {
          Operand& FrstOperand = rInsn.FirstOperand();
          FrstOperand.Type()   = O_REG8;
          FrstOperand.Reg()    = Operand1;

          Operand& ScdOperand = rInsn.SecondOperand();
          ScdOperand.Type()   = O_REG8;
          ScdOperand.Reg()    = Operand2;
        }
      Result = true;
    }
  else
    {
      Result = true;
      switch (Opcode1 & 0x0c)
        {
        case 0x04:
          rInsn.Opcode() = AVR8_Cpc; rInsn.SetName("cpc"); break;
        case 0x08:
          rInsn.Opcode() = AVR8_Sbc; rInsn.SetName("sbc"); break;
        case 0x0c:
          rInsn.Opcode() = AVR8_Add; rInsn.SetName("add"); break;
        }

      Operand& FrstOperand = rInsn.FirstOperand();
      FrstOperand.Type()   = O_REG8;
      FrstOperand.Reg()    = (Opcode1 & 0x01) << 4 | (Opcode2 >> 4);

      Operand& ScdOperand = rInsn.SecondOperand();
      ScdOperand.Type()   = O_REG8;
      ScdOperand.Reg()    = (Opcode1 & 0x02) << 3 | (Opcode2 & 0x0f);
    }

  return Result;
}

bool Avr8Architecture::Insn_8xxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  bool Result = false;

  rInsn.Length() = 2;

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  if (!(Opcode1 & 0x0e))
    {
      rInsn.Opcode() = AVR8_Ld; rInsn.SetName("ld");

      Operand& FrstOperand = rInsn.FirstOperand();
      FrstOperand.Type()   = O_REG8;
      FrstOperand.Reg()  = (Opcode1 & 0x01) << 4 | Opcode2 >> 4;

      Operand& ScdOperand = rInsn.SecondOperand();
      ScdOperand.Type()   = O_MEM | O_REG8;
      if      ((Opcode2 & 0x0f) == 0x0000) { ScdOperand.Reg() = AVR8_RegZ; Result = true; }
      else if ((Opcode2 & 0x0f) == 0x1000) { ScdOperand.Reg() = AVR8_RegY; Result = true; }
    }
  else
    {
      rInsn.Opcode() = AVR8_St; rInsn.SetName("st");

      Operand& FrstOperand = rInsn.FirstOperand();
      FrstOperand.Type()   = O_MEM | O_REG8;
      if      ((Opcode2 & 0x0f) == 0x0000) { FrstOperand.Reg() = AVR8_RegZ; Result = true; }
      else if ((Opcode2 & 0x0f) == 0x1000) { FrstOperand.Reg() = AVR8_RegY; Result = true; }

      Operand& ScdOperand = rInsn.SecondOperand();
      ScdOperand.Type()   = O_REG8;
      ScdOperand.Reg()  = (Opcode1 & 0x01) << 4 | Opcode2 >> 4;
    }

  return Result;
}

bool Avr8Architecture::Insn_3xxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Length() = 2;

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = (Opcode2 >> 4) + 16;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_IMM8;
  ScdOperand.Value()  = (Opcode1 & 0x0f) << 4 | (Opcode2 & 0x0f);

  switch (Opcode1 & 0xf0)
    {
    case 0x30: rInsn.Opcode() = AVR8_Cpi;  rInsn.SetName("cpi");  break;
    case 0x40: rInsn.Opcode() = AVR8_Sbci; rInsn.SetName("sbci"); break;
    case 0x50: rInsn.Opcode() = AVR8_Subi; rInsn.SetName("subi"); break;
    case 0x60: rInsn.Opcode() = AVR8_Ori;  rInsn.SetName("ori");  break;
    case 0x70: rInsn.Opcode() = AVR8_Andi; rInsn.SetName("andi"); break;
    }

  return true;
}

bool Avr8Architecture::Insn_94xx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  bool Result;

  rInsn.Length() = 2;
  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  if ((Opcode1 & 0x0f) == 0x04)
    {
      if ((Opcode2 & 0x0f) == 0x0b)
        Result = false;
      else
        switch (Opcode2)
          {
          case 0x08: rInsn.Opcode() = AVR8_Sec;   rInsn.SetName("sec");   Result = true; break;
          case 0x18: rInsn.Opcode() = AVR8_Sez;   rInsn.SetName("sez");   Result = true; break;
          case 0x28: rInsn.Opcode() = AVR8_Sen;   rInsn.SetName("sen");   Result = true; break;
          case 0x38: rInsn.Opcode() = AVR8_Sev;   rInsn.SetName("sev");   Result = true; break;
          case 0x48: rInsn.Opcode() = AVR8_Ses;   rInsn.SetName("ses");   Result = true; break;
          case 0x58: rInsn.Opcode() = AVR8_Seh;   rInsn.SetName("seh");   Result = true; break;
          case 0x68: rInsn.Opcode() = AVR8_Set;   rInsn.SetName("set");   Result = true; break;
          case 0x78: rInsn.Opcode() = AVR8_Sei;   rInsn.SetName("sei");   Result = true; break;
          case 0x88: rInsn.Opcode() = AVR8_Clc;   rInsn.SetName("clc");   Result = true; break;
          case 0x98: rInsn.Opcode() = AVR8_Clz;   rInsn.SetName("clz");   Result = true; break;
          case 0xA8: rInsn.Opcode() = AVR8_Cln;   rInsn.SetName("cln");   Result = true; break;
          case 0xB8: rInsn.Opcode() = AVR8_Clv;   rInsn.SetName("clv");   Result = true; break;
          case 0xC8: rInsn.Opcode() = AVR8_Cls;   rInsn.SetName("cls");   Result = true; break;
          case 0xD8: rInsn.Opcode() = AVR8_Clh;   rInsn.SetName("clh");   Result = true; break;
          case 0xE8: rInsn.Opcode() = AVR8_Clt;   rInsn.SetName("clt");   Result = true; break;
          case 0xF8: rInsn.Opcode() = AVR8_Cli;   rInsn.SetName("cli");   Result = true; break;
          case 0x09: rInsn.Opcode() = AVR8_Ijmp;  rInsn.SetName("ijmp");  Result = true; break;
          case 0x19: rInsn.Opcode() = AVR8_Eijmp; rInsn.SetName("eijmp"); Result = true; break;
          default:
            Result = false; break;
          }
    }
  else // ((Opcode1 & 0x0f) == 0x05)
    {
      switch (Opcode2)
        {
        case 0x08: rInsn.Opcode() = AVR8_Ret;    rInsn.SetName("ret");    Result = true;
                   rInsn.OperationType() = Instruction::OpRet;                           break;
        case 0x09: rInsn.Opcode() = AVR8_Icall;  rInsn.SetName("icall");  Result = true; break;
        case 0x11: rInsn.Opcode() = AVR8_Eicall; rInsn.SetName("eicall"); Result = true; break;
        case 0x18: rInsn.Opcode() = AVR8_Reti;   rInsn.SetName("reti");   Result = true; break;
        case 0x88: rInsn.Opcode() = AVR8_Sleep;  rInsn.SetName("sleep");  Result = true; break;
        case 0x98: rInsn.Opcode() = AVR8_Break;  rInsn.SetName("break");  Result = true; break;
        case 0xA8: rInsn.Opcode() = AVR8_Wdr;    rInsn.SetName("wdr");    Result = true; break;
        case 0xC8: Result = false; break;//rInsn.Opcode() = AVR8_Lpmz0;  rInsn.SetName("lpmz0");  Result = true; break;
        case 0xD8: rInsn.Opcode() = AVR8_Elpm1;  rInsn.SetName("elpm1");  Result = true; break;
          //case 0xE8: rInsn.Opcode() = AVR8_Spm;    rInsn.SetName("spm");    Result = true; break;
        case 0xE8: rInsn.Opcode() = AVR8_Spm2;   rInsn.SetName("spm2");   Result = true; break;
        case 0xF8: rInsn.Opcode() = AVR8_Spm3;   rInsn.SetName("spm3");   Result = true; break;
        default:
          Result = false; break;
        }
    }

  if (Result == false)
    {
      switch (Opcode2 & 0x0f)
        {
        case 0x00: rInsn.Opcode() = AVR8_Com;   rInsn.SetName("com");   Result = true; break;
        case 0x01: rInsn.Opcode() = AVR8_Neg;   rInsn.SetName("neg");   Result = true; break;
        case 0x02: rInsn.Opcode() = AVR8_Swap;  rInsn.SetName("swap");  Result = true; break;
        case 0x03: rInsn.Opcode() = AVR8_Inc;   rInsn.SetName("inc");   Result = true; break;
        case 0x05: rInsn.Opcode() = AVR8_Asr;   rInsn.SetName("asr");   Result = true; break;
        case 0x06: rInsn.Opcode() = AVR8_Lsr;   rInsn.SetName("lsr");   Result = true; break;
        case 0x07: rInsn.Opcode() = AVR8_Ror;   rInsn.SetName("ror");   Result = true; break;
        case 0x0a: rInsn.Opcode() = AVR8_Dec;   rInsn.SetName("dec");   Result = true; break;
        default:
          Result = false; break;
        }

      if (Result == true)
        {
          Operand& FrstOperand = rInsn.FirstOperand();
          FrstOperand.Type()   = O_REG8;
          FrstOperand.Reg()    = (Opcode1 & 0x1) << 4 | (Opcode2 >> 4);
        }
    }

  if (Result == false && (Opcode2 & 0x0e) >= 0x0c)
    {
      if ((Opcode2 & 0x0e) == 0x0c)
        {
          rInsn.Opcode() = AVR8_Jmp;
          rInsn.SetName("jmp");
          rInsn.OperationType() = Instruction::OpJump;
        }
      else if ((Opcode2 & 0x0e) == 0x0e)
        {
          rInsn.Opcode() = AVR8_Call;
          rInsn.SetName("call");
          rInsn.OperationType() = Instruction::OpCall;
        }

      u8 Addr3, Addr4;
      rInsn.Length() = 4;
      Operand& FrstOperand = rInsn.FirstOperand();
      FrstOperand.Type()   = O_ABS;
      rBinStrm.Read(Address + 2, Addr4);
      rBinStrm.Read(Address + 3, Addr3);
      FrstOperand.Value()  = ( ((Opcode1 & 0x01) << 21)
                               | ((Opcode2 & 0xf0) << 13) | ((Opcode2 & 0x01) << 16)
                               | (Addr3 << 8)
                               | Addr4
                               ) << 1;
      Result = true;
    }

  return Result;
}

bool Avr8Architecture::Insn_Pop(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Opcode() = AVR8_Pop;
  rInsn.Length() = 2;
  rInsn.SetName("pop");

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = (Opcode1 & 0x1) << 4 | (Opcode2 >> 4);

  return true;
}

bool Avr8Architecture::Insn_Lds(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  u16 Addr;

  rInsn.Opcode() = AVR8_Lds;
  rInsn.Length() = 4;
  rInsn.SetName("lds");

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);
  rBinStrm.Read(Address + 2, Addr);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = (Opcode1 & 0x1) << 4 | (Opcode2 >> 4);

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_MEM16 | O_IMM;
  ScdOperand.Value()  = Addr;

  return true;
}

bool Avr8Architecture::Insn_90xx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode2;
  bool Result;

  rBinStrm.Read(Address, Opcode2);

  switch (Opcode2 & 0x0f) //TODO : check LPMZ LPMZ+ ELPM2 ELPM3 ?
    {
    case 0x00:
      Result = Insn_Lds(rBinStrm, Address, rInsn); break;
    case 0x01: case 0x02: case 0x09: case 0x0a: case 0x0c: case 0x0d: case 0x0e:
      //Result = Insn_Ld(rBinStrm, Address, rInsn); break;
      Result = false; break;
    case 0x0f:
      Result = Insn_Pop(rBinStrm, Address, rInsn); break;
    default:
      Result = false; break;
    }
  return Result;
}

bool Avr8Architecture::Insn_Push(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Opcode() = AVR8_Push;
  rInsn.Length() = 2;
  rInsn.SetName("push");

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = (Opcode1 & 0x1) << 4 | (Opcode2 >> 4);

  return true;
}

bool Avr8Architecture::Insn_St(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  bool Result;

  rInsn.Opcode() = AVR8_St;
  rInsn.Length() = 2;
  rInsn.SetName("st");

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_MEM | O_REG8;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_REG8;
  ScdOperand.Reg()    = (Opcode1 & 0x1) << 4 | (Opcode2 >> 4);

  switch (Opcode2 & 0x0f)
    {
    case 0x01: FrstOperand.Reg() = AVR8_RegZ_PostInc; Result = true; break;
    case 0x02: FrstOperand.Reg() = AVR8_RegZ_PreDec;  Result = true; break;
    case 0x09: FrstOperand.Reg() = AVR8_RegY_PostInc; Result = true; break;
    case 0x0a: FrstOperand.Reg() = AVR8_RegY_PreDec;  Result = true; break;
    case 0x0c: FrstOperand.Reg() = AVR8_RegX;         Result = true; break;
    case 0x0d: FrstOperand.Reg() = AVR8_RegX_PostInc; Result = true; break;
    case 0x0e: FrstOperand.Reg() = AVR8_RegX_PreDec;  Result = true; break;
    default: Result = false; break;
    }

  return Result;
}

bool Avr8Architecture::Insn_Sts(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  u16 Addr;

  rInsn.Opcode() = AVR8_Sts;
  rInsn.Length() = 4;
  rInsn.SetName("sts");

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);
  rBinStrm.Read(Address + 2, Addr);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_MEM16 | O_IMM;
  FrstOperand.Value()  = Addr;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_REG8;
  ScdOperand.Reg()    = (Opcode1 & 0x1) << 4 | (Opcode2 >> 4);

  return true;
}

bool Avr8Architecture::Insn_92xx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode2;
  bool Result;

  rBinStrm.Read(Address, Opcode2);

  switch (Opcode2 & 0x0f)
    {
    case 0x00:
      Result = Insn_Sts(rBinStrm, Address, rInsn); break;
    case 0x01: case 0x02: case 0x09: case 0x0a: case 0x0c: case 0x0d: case 0x0e:
      Result = Insn_St(rBinStrm, Address, rInsn); break;
    case 0x0f:
      Result = Insn_Push(rBinStrm, Address, rInsn); break;
    default:
      Result = false; break;
    }

  return Result;
}

bool Avr8Architecture::Insn_98xx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  bool Result;

  rInsn.Length() = 2;

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_IMM; // Actually it's an address in I/O space.
  FrstOperand.Value()  = Opcode2 >> 3;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_IMM;
  ScdOperand.Value()  = Opcode2 & 0x07;

  switch (Opcode1 & 0x0f)
    {
    case 0x08: rInsn.Opcode() = AVR8_Cbi;  rInsn.SetName("cbi");  Result = true; break;
    case 0x09: rInsn.Opcode() = AVR8_Sbic; rInsn.SetName("sbic"); Result = true; break;
    case 0x0a: rInsn.Opcode() = AVR8_Sbi;  rInsn.SetName("sbi");  Result = true; break;
    case 0x0b: rInsn.Opcode() = AVR8_Sbis; rInsn.SetName("sbis"); Result = true; break;
    default: Result = false; break;
    }

  return Result;
}

bool Avr8Architecture::Insn_96xx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;
  bool Result;

  rInsn.Length() = 2;

  rBinStrm.Read(Address, Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG;
  FrstOperand.Reg()    = ((Opcode2 >> 4) & 0x03) * 2 + 24;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_IMM;
  ScdOperand.Value()  = ((Opcode2 & 0xc0) >> 2) | (Opcode2 & 0x0f);

  switch (Opcode1 & 0x0f)
    {
    case 0x06: rInsn.Opcode() = AVR8_Adiw; rInsn.SetName("adiw"); Result = true; break;
    case 0x07: rInsn.Opcode() = AVR8_Sbiw; rInsn.SetName("sbiw"); Result = true; break;
    }

  return Result;
}

bool Avr8Architecture::Insn_9xxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1;
  bool Result;

  rBinStrm.Read(Address + 1, Opcode1);

  switch (Opcode1 & 0x0e)
    {
    case 0x00:
      Result = Insn_90xx(rBinStrm, Address, rInsn);  break;
    case 0x02:
      Result = Insn_92xx(rBinStrm, Address, rInsn);  break;
    case 0x04:
      Result = Insn_94xx(rBinStrm, Address, rInsn);  break;
    case 0x06:
      Result = Insn_96xx(rBinStrm, Address, rInsn);  break;
    case 0x08: case 0x0a:
      Result = Insn_98xx(rBinStrm, Address, rInsn);  break;
    case 0x0c:
      Result = false; break;
    case 0x0e:
      Result = false; break;
    }
  return Result;
}

bool Avr8Architecture::Insn_Rcall(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.OperationType() = Instruction::OpCall;
  rInsn.Opcode() = AVR8_Rcall;
  rInsn.Length() = 2;
  rInsn.SetName("rcall");

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REL;
  // Get the 12-bit signed value.
  FrstOperand.Value() = ((Opcode1 & 0x0f) << 8 | Opcode2);
  // Dirty sign-extension from 12-bit to 16-bit (and then implicitly to sizeof(Value)).
  if ((Opcode1 & 0x08))
    FrstOperand.Value() = (s16) (((u16)FrstOperand.Value()) | (u16)0xf000);
  // This instruction uses program memory (not data memory).
  // On avr8, program memory is not byte addressable but word addressable.
  FrstOperand.Value() *= 2;

  return true;
}

bool Avr8Architecture::Insn_Rjmp(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.OperationType() = Instruction::OpJump;
  rInsn.Opcode() = AVR8_Rjmp;
  rInsn.Length() = 2;
  rInsn.SetName("rjmp");

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REL;
  // Get the 12-bit signed value.
  FrstOperand.Value() = ((Opcode1 & 0x0f) << 8 | Opcode2);
  // Dirty sign-extension from 12-bit to 16-bit (and then implicitly to sizeof(Value)).
  if ((Opcode1 & 0x08))
    FrstOperand.Value() = (s16) (((u16)FrstOperand.Value()) | (u16)0xf000);
  // This instruction uses program memory (not data memory).
  // On avr8, program memory is not byte addressable but word addressable.
  FrstOperand.Value() *= 2;

  return true;
}

bool Avr8Architecture::Insn_Ldi(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Opcode() = AVR8_Ldi;
  rInsn.Length() = 2;
  rInsn.SetName("ldi");

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = (Opcode2 >> 4) + 16;

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_IMM;
  ScdOperand.Value()  = (Opcode1 & 0x0f) << 4 | Opcode2 & 0x0f;

  return true;
}

bool Avr8Architecture::Insn_bxxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Length() = 2;

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);
  Operand& FrstOperand = rInsn.FirstOperand();
  Operand& ScdOperand = rInsn.SecondOperand();

  if (Opcode1 & 0x08)
    {
      rInsn.Opcode() = AVR8_Out;
      rInsn.SetName("out");
      FrstOperand.Type()  = O_IMM;
      FrstOperand.Value() = (Opcode1 & 0x06) << 3 | (Opcode2 & 0x0f);
      ScdOperand.Type()   = O_REG8;
      ScdOperand.Reg()    = ((Opcode1 & 0x01) << 4 | (Opcode2 & 0xf0) >> 4);
    }
  else
    {
      rInsn.Opcode() = AVR8_In;
      rInsn.SetName("in");
      FrstOperand.Type() = O_REG8;
      FrstOperand.Reg()  = ((Opcode1 & 0x01) << 4 | (Opcode2 & 0xf0) >> 4);
      ScdOperand.Type()  = O_IMM;
      ScdOperand.Value() = (Opcode1 & 0x06) << 3 | (Opcode2 & 0x0f);
    }

  return true;
}

bool Avr8Architecture::Insn_2xxx(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  u8 Opcode1, Opcode2;

  rInsn.Length() = 2;

  rBinStrm.Read(Address,     Opcode2);
  rBinStrm.Read(Address + 1, Opcode1);

  switch (Opcode1 & 0x0c)
    {
    case 0x00:  rInsn.Opcode() = AVR8_And;  rInsn.SetName("and");  break;
    case 0x04:  rInsn.Opcode() = AVR8_Eor;  rInsn.SetName("eor");  break;
    case 0x08:  rInsn.Opcode() = AVR8_Or;   rInsn.SetName("or");   break;
    case 0x0c:  rInsn.Opcode() = AVR8_Mov;  rInsn.SetName("mov");  break;
    }

  Operand& FrstOperand = rInsn.FirstOperand();
  FrstOperand.Type()   = O_REG8;
  FrstOperand.Reg()    = ((Opcode1 & 0x01) << 4 | (Opcode2 & 0xf0) >> 4);

  Operand& ScdOperand = rInsn.SecondOperand();
  ScdOperand.Type()   = O_REG8;
  ScdOperand.Reg()    = ((Opcode1 & 0x02) << 3 | Opcode2 & 0x0f);

  return true;
}

bool Avr8Architecture::Insn_Invalid(BinaryStream const& rBinStrm, TAddress Address, Instruction& rInsn)
{
  rInsn.SetName("invalid");
  rInsn.Opcode() = AVR8_Invalid_Insn;
  return false;
}
