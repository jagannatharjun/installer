#include <Windows.h>
#include <assert.h>
#include <vector>

#ifdef _MSC_VER
#define PACKED_STRUCT(name)                                                    \
  __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#elif defined(__GNUC__)
#define PACKED_STRUCT(name) struct __attribute__((packed)) name
#endif

PACKED_STRUCT(TIcoItemHeader) {
  BYTE Width, Height, Colors, Reserved;
  WORD Planes, BitCount;
  DWORD ImageSize;
};
typedef TIcoItemHeader *PIcoItemHeader;

PACKED_STRUCT(TIcoItem) {
  TIcoItemHeader Header;
  DWORD Offset;
};
typedef TIcoItem *PIcoItem;
constexpr int count = 2046;
PACKED_STRUCT(TIcoHeader) {
  WORD Reserved;
  WORD Typ;
  WORD ItemCount;
  TIcoItem Items[count];
};
typedef TIcoHeader *PIcoHeader;

PACKED_STRUCT(TGroupIconDirItem) {
  TIcoItemHeader Header;
  WORD Id;
};
typedef TGroupIconDirItem *PGroupIconDirItem;

PACKED_STRUCT(TGroupIconDir) {
  WORD Reserved, Typ, ItemCount;
  TGroupIconDirItem Items[count];
};
typedef TGroupIconDir *PGroupIconDir;

PACKED_STRUCT(GROUPICON {
  WORD Reserved1;    // reserved, must be 0
  WORD ResourceType; // type is 1 for icons
  WORD ImageCount;   // number of icons in structure (1)
  BYTE Width;        // icon width (32)
  BYTE Height;       // icon height (32)
  BYTE Colors;       // colors (0 means more than 8 bits per pixel)
  BYTE Reserved2;    // reserved, must be 0
  WORD Planes;       // color planes
  WORD BitsPerPixel; // bit depth
  DWORD ImageSize;   // size of structure
  WORD ResourceID;   // resource ID
});

bool IsValidIcon(void *P, DWORD Size) {
  DWORD ItemCount;
  if (Size < sizeof(WORD) * 3)
    return false;
  auto PChar = [](void *a) { return (char *)a; };
  if (PChar(P)[0] == 'M' && PChar(P)[1] == 'Z') {
    return false;
  }
  ItemCount = ((PIcoHeader)P)->ItemCount;
  if (Size < ((sizeof(WORD) * 3) + (ItemCount * sizeof(TIcoItem))))
    return false;
  P = static_cast<PIcoHeader>(P)->Items;
  while (ItemCount > 0) {
    if ((static_cast<PIcoItem>(P)->Offset +
             static_cast<PIcoItem>(P)->Header.ImageSize <
         static_cast<PIcoItem>(P)->Offset) ||
        (static_cast<PIcoItem>(P)->Offset +
         static_cast<PIcoItem>(P)->Header.ImageSize) > Size) {
      return false;
    }
    P = static_cast<PIcoItem>(P) + 1;
    ItemCount--;
  }
  return true;
}

std::vector<uint8_t> readAll(const char *f) {
  auto file = std::fopen(f, "rb");
  assert(file != nullptr);
  std::vector<uint8_t> buf;
  uint8_t rbuf[10240];
  int64_t readsz = 0;
  while ((readsz = fread(rbuf, 1, sizeof rbuf, file))) {
    for (int i = 0; i < readsz; i++)
      buf.push_back(rbuf[i]);
  }
  return buf;
}

void InjectMainIcon(char *Where, char *What) {

}

int update_icon(const char *FileName, const char *IconFileName) {
  auto icon_file = readAll(IconFileName);
  printf("IsValidIcon = %d\n", IsValidIcon(icon_file.data(), icon_file.size()));


}

int main() {
  update_icon("",
              R"(E:\Inno\Metro\ram\script\Metro-Ui\icon.ico)");
  system("pause");
}