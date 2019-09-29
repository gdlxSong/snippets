#ifndef GDL_BASE64ENCODE_H
#define GDL_BASE64ENCODE_H


#include "openssl/evp.h"
#include "string"




namespace gdl{

	std::string base64_encodestring(const std::string &text );
	bool Base64Encode(const std::vector<unsigned char > & vchInput, std::string & b64str);
	std::string base64_decodestring(const std::string &text );
	bool Base64Decode(const std::string & b64str, std::vector<unsigned char > & vecResult);
	int32_t Base64Encode(const char *encoded, int encoded_length, char *decoded);
	int32_t Base64Decode(const char *encoded, int encoded_length, char *decoded);
}














#endif