deps_config := \
	subconfig \
	Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
