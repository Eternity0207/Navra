# CXX=g++
# CXXFLAGS=-std=c++17 -Wall -Iinclude
# TARGET=optimizer.exe
# SRCDIR=src
# OBJDIR=obj
# SOURCES=$(wildcard $(SRCDIR)/*.cpp) main_api.cpp
# OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
# all: directories $(TARGET)
# directories:
# 	@if not exist $(OBJDIR) mkdir $(OBJDIR)
# 	@if not exist $(OBJDIR)\src mkdir $(OBJDIR)\src
# $(TARGET): $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) -o $@ $^
# $(OBJDIR)/main_api.o: main_api.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@
# $(OBJDIR)/src/%.o: src/%.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@
# clean:
# 	@if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
# 	@if exist $(TARGET) del /q $(TARGET)
# run: $(TARGET)
# 	.\$(TARGET)
# .PHONY: all clean run directories

# For Linux

CXX=g++
CXXFLAGS=-std=c++17 -Wall -Iinclude
TARGET=optimizer
SRCDIR=src
OBJDIR=obj

SOURCES=$(wildcard $(SRCDIR)/*.cpp) main_api.cpp
OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))

all: directories $(TARGET)

directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/src

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/main_api.o: main_api.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run directories
