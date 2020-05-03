QT -= gui core
QT += sql
CONFIG += c++14
CONFIG -= warn_off warn_on

QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-deprecated -Wno-write-strings -Wno-format-security -Wno-unused-result -Wno-format-truncation

linux-g++* {
	CONFIG -= qt
	CONFIG += staticlib
	CONFIG += static
	LIBS += -lpthread -lcurl -lssh
}

android-g++ {
	DEFINES += NO_MANA_SSH="1"
	DEFINES += MANA_CURL_ANDROID="1"
	DEFINES += MANA_SQLITE="1"
	LIBS += -lgnustl_shared

	LIBS += -L/dalaran/mana/lib/android/ -lcurl
	PRE_TARGETDEPS += /dalaran/mana/lib/android/libcurl.a
}

#QT -= core gui
#CONFIG += c++14
#CONFIG -= warn_off warn_on

#QMAKE_CXXFLAGS += -Wno-deprecated -Wno-write-strings -Wno-unused-result -Wno-format-security
#QMAKE_CXXFLAGS += -static -static-libgcc -static-libstdc++
#QMAKE_CFLAGS += -static -static-libgcc

#linux-g++* {
#	LIBS += -L/dalaran/mana/out/ -lmana
#	PRE_TARGETDEPS += /dalaran/mana/out/libmana.a
#	LIBS += -lcurl -lssh -lssh_threads
#	LIBS += -Wl,-Bstatic -lboost_regex -Wl,-Bdynamic
#}

#INCLUDEPATH += /dalaran/mana/mana
#DEPENDPATH += /dalaran/mana/mana
#DESTDIR     = ../out/
#OBJECTS_DIR = ../out/
#MOC_DIR = ../out/

TARGET = mana
TEMPLATE = lib
DESTDIR     = ../out/
OBJECTS_DIR = ../out/
MOC_DIR = ../out/

include(project.pro)

SOURCES += \
	common.cpp \
	fuzzy.cpp \
	daemon.cpp \
	server.cpp \
	http.cpp \
	smtp.cpp \
	imap.cpp \
	quickmail/quickmail.c \
	quickmail/smtpsocket.c \
	parser.cpp \
	yaml.cpp \
	ssh.cpp \
	database.cpp \
	cypher.cpp \
	polarssl/bignum.c \
	polarssl/base64.c \
	polarssl/sha1.c \
	polarssl/rsa.c \
	polarssl/md.c \
	polarssl/oid.c \
	polarssl/asn1parse.c \
	polarssl/md_wrap.c \
	polarssl/aes.c \
	polarssl/aesni.c \
	polarssl/arc4.c \
	polarssl/asn1write.c \
	polarssl/blowfish.c \
	polarssl/camellia.c \
	polarssl/ccm.c \
	polarssl/certs.c \
	polarssl/cipher_wrap.c \
	polarssl/cipher.c \
	polarssl/ctr_drbg.c \
	polarssl/debug.c \
	polarssl/des.c \
	polarssl/dhm.c \
	polarssl/ecdh.c \
	polarssl/ecdsa.c \
	polarssl/ecp_curves.c \
	polarssl/ecp.c \
	polarssl/entropy_poll.c \
	polarssl/entropy.c \
	polarssl/error.c \
	polarssl/gcm.c \
	polarssl/havege.c \
	polarssl/hmac_drbg.c \
	polarssl/md2.c \
	polarssl/md4.c \
	polarssl/md5.c \
	polarssl/memory_buffer_alloc.c \
	polarssl/net.c \
	polarssl/padlock.c \
	polarssl/pem.c \
	polarssl/pk_wrap.c \
	polarssl/pk.c \
	polarssl/pkcs5.c \
	polarssl/pkcs11.c \
	polarssl/pkcs12.c \
	polarssl/pkparse.c \
	polarssl/pkwrite.c \
	polarssl/platform.c \
	polarssl/ripemd160.c \
	polarssl/sha256.c \
	polarssl/sha512.c \
	polarssl/ssl_cache.c \
	polarssl/ssl_ciphersuites.c \
	polarssl/ssl_cli.c \
	polarssl/ssl_cookie.c \
	polarssl/ssl_srv.c \
	polarssl/ssl_ticket.c \
	polarssl/ssl_tls.c \
	polarssl/threading.c \
	polarssl/timing.c \
	polarssl/version_features.c \
	polarssl/version.c \
	polarssl/xtea.c

HEADERS += mana.h\
	common.h \
	fuzzy.h \
	daemon.h \
	http.h \
	server.h \
	lib.h \
	smtp.h \
	imap.h \
	quickmail/quickmail.h \
	quickmail/smtpsocket.h \
	parser.h \
	yaml.h \
	ssh.h \
	database.h \
	cypher.h \
	polarssl/mbedtls/base64.h \
	polarssl/mbedtls/bignum.h \
	polarssl/mbedtls/config.h \
	polarssl/mbedtls/rsa.h \
	polarssl/mbedtls/sha1.h \
	polarssl/mbedtls/md.h \
	polarssl/mbedtls/md_internal.h \
	polarssl/mbedtls/oid.h \
	polarssl/mbedtls/asn1.h \
	polarssl/mbedtls/aes.h \
	polarssl/mbedtls/aesni.h \
	polarssl/mbedtls/arc4.h \
	polarssl/mbedtls/asn1write.h \
	polarssl/mbedtls/blowfish.h \
	polarssl/mbedtls/bn_mul.h \
	polarssl/mbedtls/camellia.h \
	polarssl/mbedtls/ccm.h \
	polarssl/mbedtls/certs.h \
	polarssl/mbedtls/check_config.h \
	polarssl/mbedtls/cipher_internal.h \
	polarssl/mbedtls/cipher.h \
	polarssl/mbedtls/compat-1.3.h \
	polarssl/mbedtls/ctr_drbg.h \
	polarssl/mbedtls/debug.h \
	polarssl/mbedtls/des.h \
	polarssl/mbedtls/dhm.h \
	polarssl/mbedtls/ecdh.h \
	polarssl/mbedtls/ecdsa.h \
	polarssl/mbedtls/ecp.h \
	polarssl/mbedtls/entropy_poll.h \
	polarssl/mbedtls/entropy.h \
	polarssl/mbedtls/error.h \
	polarssl/mbedtls/gcm.h \
	polarssl/mbedtls/havege.h \
	polarssl/mbedtls/hmac_drbg.h \
	polarssl/mbedtls/md2.h \
	polarssl/mbedtls/md4.h \
	polarssl/mbedtls/md5.h \
	polarssl/mbedtls/memory_buffer_alloc.h \
	polarssl/mbedtls/net.h \
	polarssl/mbedtls/padlock.h \
	polarssl/mbedtls/pem.h \
	polarssl/mbedtls/pk_internal.h \
	polarssl/mbedtls/pk.h \
	polarssl/mbedtls/pkcs5.h \
	polarssl/mbedtls/pkcs11.h \
	polarssl/mbedtls/pkcs12.h \
	polarssl/mbedtls/platform.h \
	polarssl/mbedtls/ripemd160.h \
	polarssl/mbedtls/sha256.h \
	polarssl/mbedtls/sha512.h \
	polarssl/mbedtls/ssl_cache.h \
	polarssl/mbedtls/ssl_ciphersuites.h \
	polarssl/mbedtls/ssl_cookie.h \
	polarssl/mbedtls/ssl_internal.h \
	polarssl/mbedtls/ssl_ticket.h \
	polarssl/mbedtls/ssl.h \
	polarssl/mbedtls/threading.h \
	polarssl/mbedtls/timing.h \
	polarssl/mbedtls/version.h \
	polarssl/mbedtls/xtea.h

unix {
	target.path = /usr/lib
	INSTALLS += target
}
