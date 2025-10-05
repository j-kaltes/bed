# New ports collection makefile for:    bed
# Date created:         2001/05/07
# Whom:                 binaryeditor@gmx.com
#
# $FreeBSD$
#

PORTNAME=	bed
PORTVERSION=	@VERSION@
CATEGORIES=	editors
MASTER_SITES=		${MASTER_SITE_SUNSITE}
MASTER_SITE_SUBDIR=	apps/editors/terminal/
EXTRACT_SUFX=		.src.tgz

MAINTAINER=	binaryeditor@gmx.com

COMMENT=	pkg-comment

GNU_CONFIGURE=	yes

USE_GMAKE=	yes

MAN1=  bed.1
MANCOMPRESSED= no


post-configure:
	${ECHO} 'CONFIG_DEBUG=n' >> ${WRKSRC}/config
	${ECHO} 'CONFIG_MMAP=y' >> ${WRKSRC}/config
	${GMAKE} -C ${WRKSRC} dep

.include <bsd.port.mk>
pkg-descr: ../description
	${SED} 's/^[ 	]*\(.*\)$$/\1/' < ../description > pkg-descr

config: pkg-plist distinfo
DISTSRC=${PORTSDIR}/distfiles/bed-$(PORTVERSION)$(EXTRACT_SUFX)
ORSRC=../../html/download/bed-$(PORTVERSION)$(EXTRACT_SUFX)
distinfo: ${PORTSDIR}/distfiles/bed-$(PORTVERSION)$(EXTRACT_SUFX)
	PWDHERE=`pwd`;cd ${PORTSDIR}/distfiles/; ${MD5} bed-$(PORTVERSION)$(EXTRACT_SUFX) > $$PWDHERE/distinfo
pkg-plist: Makefile
	${GMAKE} -C ../ pkg/pkg-plist
${PORTSDIR}/distfiles/bed-$(PORTVERSION)$(EXTRACT_SUFX): ../../html/download/bed-$(PORTVERSION)$(EXTRACT_SUFX)
	${CP} $(ORSRC)  ${PORTSDIR}/distfiles/bed-$(PORTVERSION)$(EXTRACT_SUFX)
