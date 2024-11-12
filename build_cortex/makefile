//SDK            := /home/user/Workarea/sama7d65_ea/ea-1.0/buildroot-at91/output/host
SDK            := /home/user/Workarea/buildroot_2024.04/9x60_curiosity/buildroot-at91/output/host

CXX            := $(SDK)/bin/arm-buildroot-linux-gnueabi-g++ -std=c++17
SRC_DIR        := ../src
EGT_CXXFLAGS   := $(shell $(SDK)/bin/pkg-config libegt --cflags)
EGT_LDFLAGS    := $(shell $(SDK)/bin/pkg-config libegt --libs)
#JPEG_CXXFLAGS   := $(shell $(SDK)/bin/pkg-config libjpeg --cflags)
#JPEG_LDFLAGS    := $(shell $(SDK)/bin/pkg-config libjpeg --libs)
LIBS           := -lplanes -ljpeg

SRC            := $(SRC_DIR)/*
TARGET         := egt_t1s

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(EGT_CXXFLAGS) $^ -o $@ $(EGT_LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
