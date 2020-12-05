PHONY += menuconfig silentoldconfig clean distclean

Q            := @
kconfig          := $(CURDIR)/kconfig
SUBDIR       := kconfig
Kconfig      := Kconfig
rm-clean     += test
rm-distclean += include .config

ifeq ($(quiet),silent_)
silent := -s
endif

test:test.c
	gcc $(CFLAGS) $< -o $@

# 依赖 mconf 和 conf 两个工具
menuconfig : $(kconfig)/mconf $(obj)/conf
	$(Q)$< $(Kconfig)
	$(Q)$(MAKE) silentoldconfig

$(kconfig)/mconf:
	$(Q)$(MAKE) -C $(SUBDIR)

silentoldconfig: $(kconfig)/conf
	$(Q)mkdir -p include/generated include/config
	$(Q)$< -s --silentoldconfig $(Kconfig)

clean:
	rm -rf $(rm-clean)

distclean:
	rm -rf $(rm-clean) $(rm-distclean)


