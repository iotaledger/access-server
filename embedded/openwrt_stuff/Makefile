############################################################################
# This file is part of the Frost distribution
# (https://github.com/xainag/frost)
#
# Copyright (c) 2019 XAIN AG.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
############################################################################

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
