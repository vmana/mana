#include "smtp.h"

namespace mana
{

smtp::smtp()
{
}

bool smtp::add_attachment(string filename)
{
	if (file::exists(filename))
	{
		attachments += filename + ',';
		return true;
	}
	return false;
}

bool smtp::send()
{
	bool ret = true;
	quickmail mail = quickmail_create(from.c_str(), subject.c_str());
	quickmail_set_localname(mail, localname.c_str());
	quickmail_set_charset(mail, charset.c_str());
	quickmail_set_body(mail, body.c_str(), mime_type.c_str());
	if (verbose) quickmail_set_debug_log(mail, stdout);

	vector<string> tos = explode(",", to);
	for (int i = 0; i < tos.size(); ++i)
	{
		tos[i] = trim(tos[i]);
		quickmail_add_to(mail, tos[i].c_str());
	}

	vector<string> ccs = explode(",", cc);
	for (int i = 0; i < ccs.size(); ++i)
	{
		ccs[i] = trim(ccs[i]);
		quickmail_add_cc(mail, ccs[i].c_str());
	}

	vector<string> xattach = explode(",", attachments);
	for (int i = 0; i < xattach.size(); ++i)
	{
		xattach[i] = trim(xattach[i]);
		quickmail_add_attachment_file(mail, xattach[i].c_str(), NULL);
	}

	const char* errmsg;
	if ((errmsg = quickmail_send(mail, server.c_str(), port, NULL, NULL)) != NULL)
	{
		ret = false;
		error.push_back(string(errmsg));
	}
	else
	{
		mail_id = string(quickmail_get_mail_id(mail));
		vector<string> xmail_id = explode(" ", mail_id);
		if (xmail_id.size() > 0) mail_id = xmail_id.back();
	}

	quickmail_destroy(mail);

	return ret;
}

} // namespace
