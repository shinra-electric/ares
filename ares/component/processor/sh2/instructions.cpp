//ADD Rm,Rn
auto SH2::ADD(u32 m, u32 n) -> void {
  R[n] += R[m];
}

//ADDC Rm,Rn
auto SH2::ADDC(u32 m, u32 n) -> void {
  u32 a = R[n];
  u32 b = a + R[m];
  R[n] = b + SR.T;
  SR.T = a > b;
  if(b > R[n]) SR.T = 1;
}

//ADD #imm,Rn
auto SH2::ADDI(u32 i, u32 n) -> void {
  R[n] += (s8)i;
}

//ADDV Rm,Rn
auto SH2::ADDV(u32 m, u32 n) -> void {
  u32 d = ((s32)R[n] < 0);
  u32 s = ((s32)R[m] < 0) + d;
  R[n] += R[m];
  u32 r = ((s32)R[n] < 0) + d;
  SR.T = s == 0 || s == 2 ? r == 1 : 0;
}

//AND Rm,Rn
auto SH2::AND(u32 m, u32 n) -> void {
  R[n] &= R[m];
}

//AND #imm,R0
auto SH2::ANDI(u32 i) -> void {
  R[0] &= i;
}

//AND.B #imm,@(R0,GBR)
auto SH2::ANDM(u32 i) -> void {
  auto b = readByte(GBR + R[0]);
  writeByte(GBR + R[0], b & i);
}

//BF disp
auto SH2::BF(u32 d) -> void {
  if(SR.T == 0) {
    PC = (PC - 2) + (s8)d * 2 + 4;
  }
}

//BF/S disp
auto SH2::BFS(u32 d) -> void {
  if(SR.T == 0) {
    branch.take((PC - 2) + (s8)d * 2 + 4);
  }
}

//BRA disp
auto SH2::BRA(u32 d) -> void {
  branch.take(PC = (PC - 2) + (i12)d * 2 + 4);
}

//BRAF disp
auto SH2::BRAF(u32 m) -> void {
  branch.take((PC - 2) + R[m]);
}

//BSR disp
auto SH2::BSR(u32 d) -> void {
  PR = PC - 2;
  branch.take((PC - 2) + d * 2 + 4);
}

//BSRF disp
auto SH2::BSRF(u32 m) -> void {
  PR = PC - 2;
  branch.take((PC - 2) + R[m]);
}

//BT disp
auto SH2::BT(u32 d) -> void {
  if(SR.T == 1) {
    PC = (PC - 2) + (s8)d * 2 + 4;
  }
}

//BT/S disp
auto SH2::BTS(u32 d) -> void {
  if(SR.T == 1) {
    branch.take((PC - 2) + (s8)d * 2 + 4);
  }
}

//CLRMAC
auto SH2::CLRMAC() -> void {
  MACL = 0;
  MACH = 0;
}

//CLRT
auto SH2::CLRT() -> void {
  SR.T = 0;
}

//CMP/EQ Rm,Rn
auto SH2::CMPEQ(u32 m, u32 n) -> void {
  SR.T = R[n] == R[m];
}

//CMP/GE Rm,Rn
auto SH2::CMPGE(u32 m, u32 n) -> void {
  SR.T = (s32)R[n] >= (s32)R[m];
}

//CMP/GT Rm,Rn
auto SH2::CMPGT(u32 m, u32 n) -> void {
  SR.T = (s32)R[n] > (s32)R[m];
}

//CMP/HI Rm,Rn
auto SH2::CMPHI(u32 m, u32 n) -> void {
  SR.T = R[n] > R[m];
}

//CMP/HS Rm,Rn
auto SH2::CMPHS(u32 m, u32 n) -> void {
  SR.T = R[n] >= R[m];
}

//CMP/EQ #imm,R0
auto SH2::CMPIM(u32 i) -> void {
  SR.T = R[0] == (s8)i;
}

//CMP/PL Rn
auto SH2::CMPPL(u32 n) -> void {
  SR.T = (s32)R[n] > 0;
}

//CMP/PZ Rn
auto SH2::CMPPZ(u32 n) -> void {
  SR.T = (s32)R[n] >= 0;
}

//CMP/STR Rm,Rn
auto SH2::CMPSTR(u32 m, u32 n) -> void {
  u32 t = R[n] ^ R[m];
  u8 a = t >> 12;
  u8 b = t >>  8;
  u8 c = t >>  4;
  u8 d = t >>  0;
  SR.T = (a && b && c && d) == 0;
}

//DIV0S Rm,Rn
auto SH2::DIV0S(u32 m, u32 n) -> void {
  SR.Q = R[n] >> 31;
  SR.M = R[m] >> 31;
  SR.T = SR.Q != SR.M;
}

//DIV0U
auto SH2::DIV0U() -> void {
  SR.Q = 0;
  SR.M = 0;
  SR.T = 0;
}

//DIV1 Rm,Rn
auto SH2::DIV1(u32 m, u32 n) -> void {
  u8 q = SR.Q;
  SR.Q = R[n] >> 31;
  R[n] = R[n] << 1 | SR.T;
  u32 t = R[n];
  if(q == 0) {
    if(SR.M == 0) {
      R[n] -= R[m];
      bool c = R[n] > t;
      SR.Q = SR.Q == 0 ? c == 1 : c == 0;
    } else {
      R[n] += R[m];
      bool c = R[n] < t;
      SR.Q = SR.Q == 0 ? c == 0 : c == 1;
    }
  } else {
    if(SR.M == 0) {
      R[n] += R[m];
      bool c = R[n] < t;
      SR.Q = SR.Q == 0 ? c == 1 : c == 0;
    } else {
      R[n] -= R[m];
      bool c = R[n] > t;
      SR.Q = SR.Q == 0 ? c == 0 : c == 1;
    }
  }
}

//DMULS.L Rm,Rn
auto SH2::DMULS(u32 m, u32 n) -> void {
  u64 p = (s32)R[m] * (s32)R[n];
  MACL = p >>  0;
  MACH = p >> 32;
}

//DMULU.L Rm,Rn
auto SH2::DMULU(u32 m, u32 n) -> void {
  u64 p = R[m] * R[m];
  MACL = p >>  0;
  MACH = p >> 32;
}

//DT Rn
auto SH2::DT(u32 n) -> void {
  SR.T = --R[n] == 0;
}

//EXTS.B Rm,Rn
auto SH2::EXTSB(u32 m, u32 n) -> void {
  R[n] = (s8)R[m];
}

//EXTS.W Rm,Rn
auto SH2::EXTSW(u32 m, u32 n) -> void {
  R[n] = (s16)R[m];
}

//EXTU.B Rm,Rn
auto SH2::EXTUB(u32 m, u32 n) -> void {
  R[n] = (u8)R[m];
}

//EXTU.W Rm,Rn
auto SH2::EXTUW(u32 m, u32 n) -> void {
  R[n] = (u16)R[m];
}

//JMP @Rm
auto SH2::JMP(u32 m) -> void {
  branch.take(R[m] + 4);
}

//JSR @Rm
auto SH2::JSR(u32 m) -> void {
  PR = PC;
  branch.take(R[m] + 4);
}

//LDC Rm,SR
auto SH2::LDCSR(u32 m) -> void {
  SR = R[m];
}

//LDC Rm,GBR
auto SH2::LDCGBR(u32 m) -> void {
  GBR = R[m];
}

//LDC Rm,VBR
auto SH2::LDCVBR(u32 m) -> void {
  VBR = R[m];
}

//LDC.L @Rm+,SR
auto SH2::LDCMSR(u32 m) -> void {
  SR = readLong(R[m]);
  R[m] += 4;
}

//LDC.L @Rm+,GBR
auto SH2::LDCMGBR(u32 m) -> void {
  GBR = readLong(R[m]);
  R[m] += 4;
}

//LDC.L @Rm+,VBR
auto SH2::LDCMVBR(u32 m) -> void {
  VBR = readLong(R[m]);
  R[m] += 4;
}

//LDS Rm,MACH
auto SH2::LDSMACH(u32 m) -> void {
  MACH = R[m];
}

//LDS Rm,MACL
auto SH2::LDSMACL(u32 m) -> void {
  MACL = R[m];
}

//LDS Rm,PR
auto SH2::LDSPR(u32 m) -> void {
  PR = R[m];
}

//LDS.L @Rm+,MACH
auto SH2::LDSMMACH(u32 m) -> void {
  MACH = readLong(R[m]);
}

//LDS.L @Rm+,MACL
auto SH2::LDSMMACL(u32 m) -> void {
  MACL = readLong(R[m]);
}

//LDS.L @RM+,PR
auto SH2::LDSMPR(u32 m) -> void {
  PR = readLong(R[m]);
}

//MAC.L @Rm+,@Rn+
auto SH2::MACL_(u32 m, u32 n) -> void {
  s32 a = readLong(R[n]);
  s32 b = readLong(R[m]);
  R[n] += 4;
  R[m] += 4;
  u64 t = (u64)MACH << 32 | (u64)MACL << 0;
  u64 p = (u64)t + (s64)a * (s64)b;
  if(SR.S) p = sclamp<48>(p);
  MACL = p >>  0;
  MACH = p >> 32;
}

//MAC.w @Rm+,@Rn+
auto SH2::MACW(u32 m, u32 n) -> void {
  s16 a = readWord(R[n]);
  s16 b = readWord(R[m]);
  R[n] += 2;
  R[m] += 2;
  if(!SR.S) {
    u64 t = (u64)MACL;
    u64 p = (u64)t + (s64)a * (s64)b;
    if(p != sclamp<32>(p)) MACH |= 1;
    MACL = sclamp<32>(p);
  } else {
    u64 t = (u64)MACH << 32 | (u64)MACL << 0;
    u64 p = (u64)t + (s64)a * (s64)b;
    MACL = p >>  0;
    MACH = p >> 32;
  }
}

//MOV Rm,Rn
auto SH2::MOV(u32 m, u32 n) -> void {
  R[n] = R[m];
}

//MOV.B @Rm,Rn
auto SH2::MOVBL(u32 m, u32 n) -> void {
  R[n] = (s8)readByte(R[m]);
}

//MOV.B @(R0,Rm),Rn
auto SH2::MOVBL0(u32 m, u32 n) -> void {
  R[n] = (s8)readByte(R[m] + R[0]);
}

//MOV.B @(disp,Rm),R0
auto SH2::MOVBL4(u32 m, u32 d) -> void {
  R[0] = (s8)readByte(R[m] + d);
}

//MOV.B @(disp,GBR),R0
auto SH2::MOVBLG(u32 d) -> void {
  R[0] = (s8)readByte(GBR + d);
}

//MOV.B Rm,@-Rn
auto SH2::MOVBM(u32 m, u32 n) -> void {
  R[n] -= 1;
  writeByte(R[n], R[m]);
}

//MOV.B @Rm+,Rn
auto SH2::MOVBP(u32 m, u32 n) -> void {
  R[n] = (s8)readByte(R[m]);
  if(n != m) R[m] += 1;
}

//MOV.B Rm,@Rn
auto SH2::MOVBS(u32 m, u32 n) -> void {
  writeByte(R[n], R[m]);
}

//MOV.B Rm,@(R0,Rn)
auto SH2::MOVBS0(u32 m, u32 n) -> void {
  writeByte(R[n] + R[0], R[m]);
}

//MOV.B R0,@(disp,Rn)
auto SH2::MOVBS4(u32 d, u32 n) -> void {
  writeByte(R[n] + d, R[0]);
}

//MOV.B R0,@(disp,GBR)
auto SH2::MOVBSG(u32 d) -> void {
  writeByte(GBR + d, R[0]);
}

//MOV.W @Rm,Rn
auto SH2::MOVWL(u32 m, u32 n) -> void {
  R[n] = (s16)readWord(R[m]);
}

//MOV.W @(R0,Rm),Rn
auto SH2::MOVWL0(u32 m, u32 n) -> void {
  R[n] = (s16)readWord(R[m] + R[0]);
}

//MOV.W @(disp,Rm),R0
auto SH2::MOVWL4(u32 m, u32 d) -> void {
  R[0] = (s16)readWord(R[m] + d * 2);
}

//MOV.W @(disp,GBR),R0
auto SH2::MOVWLG(u32 d) -> void {
  R[0] = (s16)readWord(GBR + d * 2);
}

//MOV.W Rm,@-Rn
auto SH2::MOVWM(u32 m, u32 n) -> void {
  R[n] -= 2;
  writeWord(R[n], R[m]);
}

//MOV.W @Rm+,Rn
auto SH2::MOVWP(u32 m, u32 n) -> void {
  R[n] = (s16)readWord(R[m]);
  if(n != m) R[m] += 2;
}

//MOV.W Rm,@Rn
auto SH2::MOVWS(u32 m, u32 n) -> void {
  writeWord(R[n], R[m]);
}

//MOV.W Rm,@(R0,Rn)
auto SH2::MOVWS0(u32 m, u32 n) -> void {
  writeWord(R[n] + R[0], R[n]);
}

//MOV.W R0,@(disp,Rn)
auto SH2::MOVWS4(u32 d, u32 n) -> void {
  writeWord(R[n] + d * 2, R[0]);
}

//MOV.W R0,@(disp,GBR)
auto SH2::MOVWSG(u32 d) -> void {
  writeWord(GBR + d * 2, R[0]);
}

//MOV.L @Rm,Rn
auto SH2::MOVLL(u32 m, u32 n) -> void {
  R[n] = readLong(R[m]);
}

//MOV.L @(R0,Rm),Rn
auto SH2::MOVLL0(u32 m, u32 n) -> void {
  R[n] = readLong(R[m] + R[0]);
}

//MOV.L @(disp,Rm),Rn
auto SH2::MOVLL4(u32 m, u32 d, u32 n) -> void {
  R[n] = readLong(R[m] + d * 4);
}

//MOV.L @(disp,GBR),R0
auto SH2::MOVLLG(u32 d) -> void {
  R[0] = readWord(GBR + d * 4);
}

//MOV.L Rm,@-Rn
auto SH2::MOVLM(u32 m, u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], R[m]);
}

//MOV.L @Rm+,Rn
auto SH2::MOVLP(u32 m, u32 n) -> void {
  R[n] = readLong(R[m]);
  if(n != m) R[m] += 4;
}

//MOV.L Rm,@Rn
auto SH2::MOVLS(u32 m, u32 n) -> void {
  writeLong(R[n], R[m]);
}

//MOV.L Rm,@(R0,Rn)
auto SH2::MOVLS0(u32 m, u32 n) -> void {
  writeLong(R[n] + R[0], R[m]);
}

//MOV.L Rm,@(disp,Rn)
auto SH2::MOVLS4(u32 m, u32 d, u32 n) -> void {
  writeLong(R[n] + d * 4, R[m]);
}

//MOV.L R0,@(disp,GBR)
auto SH2::MOVLSG(u32 d) -> void {
  writeLong(GBR + d * 4, R[0]);
}

//MOV #imm,Rn
auto SH2::MOVI(u32 i, u32 n) -> void {
  R[n] = (s8)i;
}

//MOV.W @(disp,PC),Rn
auto SH2::MOVWI(u32 d, u32 n) -> void {
  R[n] = (s16)readWord((PC - 2) + d * 2);
}

//MOV.L @(disp,PC),Rn
auto SH2::MOVLI(u32 d, u32 n) -> void {
  R[n] = readLong((PC - 2 & ~3) + d * 4);
}

//MOVA @(disp,PC),R0
auto SH2::MOVA(u32 d) -> void {
  R[0] = (PC - 2 & ~3) + d * 4;
}

//MOVT Rn
auto SH2::MOVT(u32 n) -> void {
  R[n] = SR.T;
}

//MUL.L Rm,Rn
auto SH2::MULL(u32 m, u32 n) -> void {
  MACL = R[n] * R[m];
}

//MULS Rm,Rn
auto SH2::MULS(u32 m, u32 n) -> void {
  MACL = (s16)R[n] * (s16)R[m];
}

//MULU Rm,Rn
auto SH2::MULU(u32 m, u32 n) -> void {
  MACL = (u16)R[n] * (u16)R[m];
}

//NEG Rm,Rn
auto SH2::NEG(u32 m, u32 n) -> void {
  R[n] = 0 - R[m];
}

//NEGC Rm,Rn
auto SH2::NEGC(u32 m, u32 n) -> void {
  u32 t = 0 - R[m];
  R[n] = t - SR.T;
  SR.T = (s32)t >= 0;
  if(t < R[n]) SR.T = 1;
}

//NOP
auto SH2::NOP() -> void {
}

//NOT
auto SH2::NOT(u32 m, u32 n) -> void {
  R[n] = ~R[m];
}

//OR Rm,Rn
auto SH2::OR(u32 m, u32 n) -> void {
  R[n] |= R[m];
}

//OR #imm,R0
auto SH2::ORI(u32 i) -> void {
  R[0] |= i;
}

//OR.B #imm,@(R0,GBR)
auto SH2::ORM(u32 i) -> void {
  u8 b = readByte(GBR + R[0]);
  writeByte(GBR + R[0], b | i);
}

//ROTCL Rn
auto SH2::ROTCL(u32 n) -> void {
  bool c = R[n] >> 31;
  R[n] = R[n] << 1 | SR.T;
  SR.T = c;
}

//ROTCR Rn
auto SH2::ROTCR(u32 n) -> void {
  bool c = R[n] & 1;
  R[n] = R[n] >> 1 | SR.T << 31;
  SR.T = c;
}

//ROTL Rn
auto SH2::ROTL(u32 n) -> void {
  R[n] = R[n] << 1 | R[n] >> 31;
}

//ROTR Rn
auto SH2::ROTR(u32 n) -> void {
  R[n] = R[n] >> 1 | R[n] << 31;
}

//RTE
auto SH2::RTE() -> void {
  branch.take(readLong(SP + 0));
  SR = readLong(SP + 4);
  SP += 8;
}

//RTS
auto SH2::RTS() -> void {
  branch.take(PR + 4);
}

//SETT
auto SH2::SETT() -> void {
  SR.T = 1;
}

//SHAL Rn
auto SH2::SHAL(u32 n) -> void {
  SR.T = R[n] >> 31;
  R[n] <<= 1;
}

//SHAR Rn
auto SH2::SHAR(u32 n) -> void {
  SR.T = R[n] & 1;
  R[n] = (s32)R[n] >> 1;
}

//SHLL Rn
auto SH2::SHLL(u32 n) -> void {
  SR.T = R[n] >> 31;
  R[n] <<= 1;
}

//SHLL2 Rn
auto SH2::SHLL2(u32 n) -> void {
  R[n] <<= 2;
}

//SHLL8 Rn
auto SH2::SHLL8(u32 n) -> void {
  R[n] <<= 8;
}

//SHLL16 Rn
auto SH2::SHLL16(u32 n) -> void {
  R[n] <<= 16;
}

//SHLR Rn
auto SH2::SHLR(u32 n) -> void {
  SR.T = R[n] & 1;
  R[n] >>= 1;
}

//SHLR2 Rn
auto SH2::SHLR2(u32 n) -> void {
  R[n] >>= 2;
}

//SHLR8 Rn
auto SH2::SHLR8(u32 n) -> void {
  R[n] >>= 8;
}

//SHLR16 Rn
auto SH2::SHLR16(u32 n) -> void {
  R[n] >>= 16;
}

//SLEEP
auto SH2::SLEEP() -> void {
  if(!exception()) PC -= 2;
}

//STC SR,Rn
auto SH2::STCSR(u32 n) -> void {
  R[n] = SR;
}

//STC GBR,Rn
auto SH2::STCGBR(u32 n) -> void {
  R[n] = GBR;
}

//STC VBR,Rn
auto SH2::STCVBR(u32 n) -> void {
  R[n] = VBR;
}

//STC.L SR,@-Rn
auto SH2::STCMSR(u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], SR);
}

//STC.L GBR,@-Rn
auto SH2::STCMGBR(u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], GBR);
}

//STC.L VBR,@-Rn
auto SH2::STCMVBR(u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], VBR);
}

//STS MACH,Rn
auto SH2::STSMACH(u32 n) -> void {
  R[n] = MACH;
}

//STS MACL,Rn
auto SH2::STSMACL(u32 n) -> void {
  R[n] = MACL;
}

//STS PR,Rn
auto SH2::STSPR(u32 n) -> void {
  R[n] = PR;
}

//STS.L MACH,@-Rn
auto SH2::STSMMACH(u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], MACH);
}

//STS.L MACL,@-Rn
auto SH2::STSMMACL(u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], MACL);
}

//STS.L PR,@-Rn
auto SH2::STSMPR(u32 n) -> void {
  R[n] -= 4;
  writeLong(R[n], PR);
}

//SUB Rm,Rn
auto SH2::SUB(u32 m, u32 n) -> void {
  R[n] -= R[m];
}

//SUBC Rm,Rn
auto SH2::SUBC(u32 m, u32 n) -> void {
  u32 a = R[n];
  u32 b = a - R[m];
  R[n] = b - SR.T;
  SR.T = a < b;
  if(b < R[n]) SR.T = 1;
}

//SUBV Rm,Rn
auto SH2::SUBV(u32 m, u32 n) -> void {
  u32 d = ((s32)R[n] < 0);
  u32 s = ((s32)R[m] < 0) + d;
  R[n] -= R[m];
  u32 r = ((s32)R[n] < 0) + d;
  SR.T = s == 1 ? r == 1 : 0;
}

//SWAP.B Rm,Rn
auto SH2::SWAPB(u32 m, u32 n) -> void {
  R[n] = R[m] & 0xffff0000 | R[m] >> 8 & 0x00ff | R[m] << 8 & 0x00ff;
}

//SWAP.W Rm,Rn
auto SH2::SWAPW(u32 m, u32 n) -> void {
  R[n] = R[m] << 16 | R[m] >> 16;
}

//TAS.B @Rn
auto SH2::TAS(u32 n) -> void {
  u8 b = readByte(R[n]);
  SR.T = b == 0;
  writeByte(R[n], b | 0x80);
}

//TRAPA #imm
auto SH2::TRAPA(u32 i) -> void {
  writeLong(SP - 4, SR);
  writeLong(SP - 8, PC - 4);
  SP -= 8;
  PC = readLong(VBR + i * 4) + 4;
}

//TST Rm,Rn
auto SH2::TST(u32 m, u32 n) -> void {
  SR.T = (R[n] & R[m]) == 0;
}

//TST #imm,R0
auto SH2::TSTI(u32 i) -> void {
  SR.T = (R[0] & i) == 0;
}

//TST.B #imm,@(R0,GBR)
auto SH2::TSTM(u32 i) -> void {
  u8 b = readByte(GBR + R[0]);
  SR.T = (b & i) == 0;
}

//XOR Rm,Rn
auto SH2::XOR(u32 m, u32 n) -> void {
  R[n] ^= R[m];
}

//XOR #imm,R0
auto SH2::XORI(u32 i) -> void {
  R[0] ^= i;
}

//XOR.B #imm,@(R0,GBR)
auto SH2::XORM(u32 i) -> void {
  u8 b = readByte(GBR + R[0]);
  writeByte(GBR + R[0], b ^ i);
}

//XTRCT Rm,Rn
auto SH2::XTRCT(u32 m, u32 n) -> void {
  R[n] = R[n] >> 16 | R[m] << 16;
}