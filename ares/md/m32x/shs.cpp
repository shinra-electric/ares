auto M32X::SHS::load(Node::Object parent) -> void {
  node = parent->append<Node::Object>("SHS");
  bootROM.allocate(1_KiB >> 1);
  if(auto fp = system.pak->read("sh2.boot.srom")) {
    for(auto address : range(1_KiB >> 1)) bootROM.program(address, fp->readm(2L));
  }
  debugger.load(node);
}

auto M32X::SHS::unload() -> void {
  debugger = {};
  bootROM.reset();
  node.reset();
}

auto M32X::SHS::main() -> void {
  debugger.instruction();
  instruction();
  step(1);
}

auto M32X::SHS::step(u32 clocks) -> void {
  Thread::step(clocks);
  Thread::synchronize(cpu);
}

auto M32X::SHS::exception() -> bool {
  return false;
}

auto M32X::SHS::power(bool reset) -> void {
  Thread::create(23'000'000, {&M32X::SHS::main, this});
  SH2::power();
  SH2::PC    = bootROM[0] << 16 | bootROM[1] << 0;
  SH2::R[15] = bootROM[2] << 16 | bootROM[3] << 0;
}

auto M32X::SHS::readByte(u32 address) -> u32 {
  if(address & 1) {
    return m32x.readInternal(0, 1, address & ~1).byte(0);
  } else {
    return m32x.readInternal(1, 0, address & ~1).byte(1);
  }
}

auto M32X::SHS::readWord(u32 address) -> u32 {
  return m32x.readInternal(1, 1, address & ~1);
}

auto M32X::SHS::readLong(u32 address) -> u32 {
  u32    data = m32x.readInternal(1, 1, address & ~3 | 0) << 16;
  return data | m32x.readInternal(1, 1, address & ~3 | 2) <<  0;
}

auto M32X::SHS::writeByte(u32 address, u32 data) -> void {
  if(address & 1) {
    m32x.writeInternal(0, 1, address & ~1, data << 8 | (u8)data << 0);
  } else {
    m32x.writeInternal(1, 0, address & ~1, data << 8 | (u8)data << 0);
  }
}

auto M32X::SHS::writeWord(u32 address, u32 data) -> void {
  m32x.writeInternal(1, 1, address & ~1, data);
}

auto M32X::SHS::writeLong(u32 address, u32 data) -> void {
  m32x.writeInternal(1, 1, address & ~3 | 0, data >> 16);
  m32x.writeInternal(1, 1, address & ~3 | 2, data >>  0);
}