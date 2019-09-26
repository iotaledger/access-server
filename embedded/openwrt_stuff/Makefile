include $(TOPDIR)/rules.mk

PKG_NAME:=xain_embedded
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/xain_embedded
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=XAIN embedded
	DEPENDS:=+curl +libsqlite3 +libpthread
endef

define Package/xain_embedded/description
	XAIN embedded server
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/platform/RaspberryPi/* $(PKG_BUILD_DIR)/
	$(CP) $(PKG_BUILD_DIR)/Makefile.openwrt $(PKG_BUILD_DIR)/Makefile
endef

define Package/xain_embedded/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/serverTest $(1)/usr/sbin/xain_embedded
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/can_simulator $(1)/usr/sbin/xain_can_simulator
endef

$(eval $(call BuildPackage,xain_embedded))
