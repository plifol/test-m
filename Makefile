
name_prj = test-m_v3
pwd      = $(shell pwd)
src_dir  = $(pwd)
bin_dir  = $(pwd)/bin
obj_dir  = $(bin_dir)/obj
target   = $(bin_dir)/$(name_prj)

inc_path_tmp = \
	/usr/include

inc_path = $(addprefix -I, $(inc_path_tmp))

libs_path_tmp = \
	$(bin_dir) \
	/usr/lib64

libs_path = $(addprefix -I, $(libs_path_tmp))

libs = \
	-lX11 \
	-lXaw \
	-lXt

ld	= gcc
gcc	= gcc

gcc_flags = \
	-Wall

ld_flags = \
	-Wall

sources   = $(wildcard $(addsuffix /*.c, $(src_dir)))
objects   = $(notdir $(patsubst %.c, %.o, $(sources)))
obj_files = $(addprefix $(obj_dir)/, $(objects))

all: clean mk_dir $(target)

$(target): $(obj_files)
	@echo
	@echo linker start
	@echo file: $(notdir $@) 
	@echo path: $(dir $@) 
	$(ld) $(ld_flags) -o $(target) $(obj_files) $(libs_path) $(libs)
	@echo $(notdir $(target)) ... Ok

$(obj_dir)/%.o: $(src_dir)/%.c
	@echo
	@echo compiler start
	@echo file: $(notdir $@) 
	@echo path: $(dir $@) 
	$(gcc) $(gcc_flags) -c $(inc_path) -o $@ $<
	@echo $(notdir $<) ... Ok

mk_dir:
	mkdir -p $(bin_dir)
	mkdir -p $(obj_dir)
	@echo mk_dir ... Done

rm_obj:
	rm -f $(obj_files)
	@echo rm_obj ... Done

rm_target:
	rm -f $(target)
	@echo rm_target ... Done

clean: rm_obj rm_target
	-rm -frd $(bin_dir)
	@echo clean ... Done
