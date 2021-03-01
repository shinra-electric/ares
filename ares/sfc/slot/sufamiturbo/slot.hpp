struct SufamiTurboSlot {
  Node::Port port;
  Pak pak;
  SufamiTurboCartridge cartridge;

  //slot.cpp
  SufamiTurboSlot(string name);
  auto load(Node::Object) -> void;
  auto unload() -> void;

  const string name;
};

extern SufamiTurboSlot sufamiturboSlotA;
extern SufamiTurboSlot sufamiturboSlotB;
