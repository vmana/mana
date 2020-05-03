#ifndef MANA_H
#define MANA_H
// apt-get install libssh-dev libcurl4-openssl-dev
// apt-get install freetds-dev

//#include "thread.h"
#include "server.h"
#include "http.h"
#include "smtp.h"
#include "imap.h"
#include "ssh.h"
#include "daemon.h"
#include "parser.h"
#include "cypher.h"
#include "database.h"
#include "yaml.h"
#include "fuzzy.h"
#ifdef MANA_WT
#include "wt/wt.h"
#endif // MANA_WT
#endif // MANA_H
