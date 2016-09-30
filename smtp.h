#ifndef SMTP_H
#define SMTP_H

#include "common.h"
#include "quickmail/quickmail.h"

namespace mana
{

class smtp
{
	protected :
	public :
		smtp();
		string localname = "localhost";
		string server = "127.0.0.1";
		int port = 25;
		string from = "";
		string to = "";
		string cc = "";
		string attachments;
		string subject = "";
		string body = "";
		string charset = "";
		string mime_type = "text/plain";
		string mail_id = ""; // value is set after a successful sent mail
		bool verbose = false;
		vector<string> error;

		bool add_attachment(string filename);
		bool send();
};

} // namespace

#endif // SMTP_H
