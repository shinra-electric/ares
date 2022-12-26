auto CPU::readIO(n16 address) -> n8 {
  n8 data;

  switch(address) {

  case 0x0040 ... 0x0042:  //DMA_SRC
    data = dma.source.byte(address - 0x0040);
    break;

  case 0x0044 ... 0x0045:  //DMA_DST
    data = dma.target.byte(address - 0x0044);
    break;

  case 0x0046 ... 0x0047:  //DMA_LEN
    data = dma.length.byte(address - 0x0046);
    break;

  case 0x0048:  //DMA_CTRL
    data.bit(0) = dma.direction;
    data.bit(7) = dma.enable;
    break;

  case 0x0062:  //WSC_SYSTEM
    data.bit(7) = SoC::SPHINX2();
    break;

  case 0x00a0:  //HW_FLAGS
    data.bit(0) = io.cartridgeEnable;
    data.bit(1) = !SoC::ASWAN();
    data.bit(2) = io.cartridgeRomWidth;
    data.bit(3) = io.cartridgeRomWait;
    data.bit(7) = 1;  //1 = built-in self-test passed
    break;

  case 0x00b0:  //INT_BASE
    data  = io.interruptBase;
    data |= SoC::ASWAN() ? 3 : 0;
    break;

  case 0x00b2:  //INT_ENABLE
    data = io.interruptEnable;
    break;

  case 0x00b4:  //INT_STATUS
    data = io.interruptStatus;
    break;

  case 0x00b5:  //KEYPAD
    data.bit(0,3) = keypad.read();
    data.bit(4,6) = keypad.matrix;
    break;

  case 0x00b7:  //NMI
    data.bit(4) = io.nmiOnLowBattery;
    break;

  }

  return data;
}

auto CPU::writeIO(n16 address, n8 data) -> void {
  switch(address) {

  case 0x0040 ... 0x0042:  //DMA_SRC
    dma.source.byte(address - 0x0040) = data;
    dma.source &= ~1;
    break;

  case 0x0044 ... 0x0045:  //DMA_DST
    dma.target.byte(address - 0x0044) = data;
    dma.target &= ~1;
    break;

  case 0x0046 ... 0x0047:  //DMA_LEN
    dma.length.byte(address - 0x0046) = data;
    dma.length &= ~1;
    break;

  case 0x0048:  //DMA_CTRL
    dma.direction = data.bit(6);
    dma.enable    = data.bit(7);
    if(dma.enable) dma.transfer();
    break;

  case 0x0062:  //WSC_SYSTEM
    if(!SoC::ASWAN()) {
      if(data.bit(0)) scheduler.exit(Event::Power);
    }
    break;

  case 0x00a0:  //HW_FLAGS
    io.cartridgeEnable |= data.bit(0);  //bit can never be unset (boot ROM lockout)
    io.cartridgeRomWidth = data.bit(2);
    io.cartridgeRomWait = data.bit(3);
    break;

  case 0x00b0:  //INT_BASE
    io.interruptBase = SoC::ASWAN() ? data & ~7 : data & ~1;
    break;

  case 0x00b2:  //INT_ENABLE
    //disabling an interrupt that is pending will *not* clear its pending flag
    io.interruptEnable = data;
    break;

  case 0x00b5:  //KEYPAD
    keypad.matrix = data.bit(4,6);
    break;

  case 0x00b6:  //INT_ACK
    //acknowledge only edge-sensitive interrupts
    io.interruptStatus &= ~(data & 0b11110010);
    break;

  case 0x00b7:  //NMI
    io.nmiOnLowBattery = data.bit(4);
    break;

  }

  return;
}
