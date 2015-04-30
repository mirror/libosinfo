<?xml version="1.0"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="/install-script-config">
# OS id=<xsl:value-of select="os/id"/>
# <xsl:choose>
    <xsl:when test="media != ''">Media id=<xsl:value-of select="media/id"/></xsl:when>
    <xsl:otherwise>Unknown media</xsl:otherwise>
  </xsl:choose>
# Profile: <xsl:value-of select="script/profile"/>
install
text
keyboard <xsl:value-of select="config/l10n-keyboard"/>
lang <xsl:value-of select="config/l10n-language"/>
skipx
network --device eth0 --bootproto dhcp
rootpw <xsl:value-of select="config/admin-password"/>
timezone --utc <xsl:value-of select="config/l10n-timezone"/>
bootloader --location=mbr
zerombr

part biosboot --fstype=biosboot --size=1
part pv.2 --size=1 --grow --ondisk=vda
volgroup VolGroup00 --pesize=32768 pv.2
logvol / --fstype ext4 --name=LogVol00 --vgname=VolGroup00 --size=1024 --grow
reboot

%packages
@base
@core
@hardware-support

%end
  </xsl:template>
</xsl:stylesheet>
