UNAME := $(shell uname)

ifeq ($(UNAME),MINGW64_NT-10.0-19043)
	TARGET_EXEC ?= pkg.dll
else
ifeq ($(UNAME),MINGW32_NT-6.2)
	TARGET_EXEC ?= pkg.dll
else
	TARGET_EXEC ?= pkg.so
endif
endif


BUILD_DIR ?= ./bin
SRC_DIRS ?= ./src

CPP_V ?= 11

SRCS := $(shell find $(SRC_DIRS) -name *.cc -or -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -std=gnu++$(CPP_V) -fPIC
LDFLAGS ?= -shared -fPIC

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# c++ source
$(BUILD_DIR)/%.cc.o: %.cc
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# more c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
