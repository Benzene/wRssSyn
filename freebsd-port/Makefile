# $FreeBSD$

PORTNAME=			wRssSyn
PORTVERSION=		0.1
CATEGORIES=			www
MASTER_SITES=		http://www.leukensis.org/files/

MAINTAINER=			matthieu.riviere@leukensis.org
COMMENT=			Yet another RSS syndicator

LIB_DEPENDS=		xml++:${PORTSDIR}/textproc/libxml++26 \
					curl:${PORTSDIR}/ftp/curl \
					pqxx:${PORTSDIR}/databases/postgresql-libpqxx

USE_CMAKE=			yes
USE_PGSQL=			yes

PLIST_FILES=		bin/wRssSyn

.include <bsd.port.mk>
