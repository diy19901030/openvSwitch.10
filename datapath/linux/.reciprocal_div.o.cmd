cmd_/home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.o := gcc -Wp,-MD,/home/vpls/openvswitch-1.10.0/datapath/linux/.reciprocal_div.o.d  -nostdinc -isystem /usr/lib/gcc/i686-linux-gnu/4.6/include -I/home/vpls/openvswitch-1.10.0/include -I/home/vpls/openvswitch-1.10.0/datapath/linux/compat -I/home/vpls/openvswitch-1.10.0/datapath/linux/compat/include  -I/usr/src/linux-headers-3.2.0-24-generic-pae/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /usr/src/linux-headers-3.2.0-24-generic-pae/include/linux/kconfig.h -Iubuntu/include  -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -m32 -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2 -march=i686 -mtune=generic -maccumulate-outgoing-args -Wa,-mtune=generic32 -ffreestanding -fstack-protector -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=1024 -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -pg -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -DVERSION=\"1.10.0\" -I/home/vpls/openvswitch-1.10.0/datapath/linux/.. -I/home/vpls/openvswitch-1.10.0/datapath/linux/.. -g -include /home/vpls/openvswitch-1.10.0/datapath/linux/kcompat.h  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(reciprocal_div)"  -D"KBUILD_MODNAME=KBUILD_STR(openvswitch)" -c -o /home/vpls/openvswitch-1.10.0/datapath/linux/.tmp_reciprocal_div.o /home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.c

source_/home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.o := /home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.c

deps_/home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.o := \
  /home/vpls/openvswitch-1.10.0/datapath/linux/kcompat.h \
  /usr/src/linux-headers-3.2.0-24-generic-pae/arch/x86/include/asm/div64.h \
    $(wildcard include/config/x86/32.h) \
  /home/vpls/openvswitch-1.10.0/include/linux/types.h \
  /home/vpls/openvswitch-1.10.0/datapath/linux/compat/include/linux/types.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /usr/src/linux-headers-3.2.0-24-generic-pae/arch/x86/include/asm/types.h \
  include/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  /usr/src/linux-headers-3.2.0-24-generic-pae/arch/x86/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  /home/vpls/openvswitch-1.10.0/datapath/linux/compat/include/linux/stddef.h \
  include/linux/stddef.h \
  /home/vpls/openvswitch-1.10.0/datapath/linux/compat/include/linux/compiler.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/vpls/openvswitch-1.10.0/datapath/linux/compat/include/linux/compiler-gcc.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /usr/src/linux-headers-3.2.0-24-generic-pae/arch/x86/include/asm/posix_types.h \
  /usr/src/linux-headers-3.2.0-24-generic-pae/arch/x86/include/asm/posix_types_32.h \
  /home/vpls/openvswitch-1.10.0/datapath/linux/compat/include/linux/reciprocal_div.h \
  include/linux/version.h \
  include/linux/reciprocal_div.h \

/home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.o: $(deps_/home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.o)

$(deps_/home/vpls/openvswitch-1.10.0/datapath/linux/reciprocal_div.o):
