#include"base64encode.hpp"

/******************************************************************************
    base64
******************************************************************************/
std::string gdl::base64_encodestring(const std::string &text ){


    //在使用openssl的时候，用户有责任保证out缓冲区不溢出.
    EVP_ENCODE_CTX ectx;
    //set out bufer size.
    int size = text.size()*2;
    size = size > 64 ? size : 64;
    unsigned char* out = (unsigned char*)malloc( size );
    int outlen = 0;
    int tlen = 0;
    //printf("text.size = %d\n", text.size());
    EVP_EncodeInit( &ectx );
    EVP_EncodeUpdate( &ectx, out, &outlen, (const unsigned char*)text.c_str(), text.size() );
    tlen += outlen;
    EVP_EncodeFinal( &ectx, out+tlen, &outlen );
    tlen += outlen;

    std::string str( (char*)out, tlen );
    free( out );
    return str;
}


bool gdl::Base64Encode(const std::vector<unsigned char > & vchInput, std::string & b64str)
{
    //从参数返回的接口更加优良.
    EVP_ENCODE_CTX  ctx;
    EVP_EncodeInit(&ctx);

    int destbuflen = vchInput.size() * 2 + 1; // enough

    char * destbuf = (char *)malloc(destbuflen);

    int outl = 0;
    EVP_EncodeUpdate(&ctx, (unsigned char *)destbuf, &outl, (unsigned char *)vchInput.date(), vchInput.size());

    int tmplen = 0;
    EVP_EncodeFinal(&ctx, (unsigned char *)destbuf+outl, &tmplen);
    outl += tmplen;

    b64str.append(destbuf, outl);

    free(destbuf); destbuf = NULL;

    return !b64str.empty();
}


std::string gdl::base64_decodestring(const std::string &text ){
    EVP_ENCODE_CTX ectx;
    unsigned char* out = (unsigned char*)malloc( text.size() );
    int outlen = 0;
    int tlen = 0;

    EVP_DecodeInit( &ectx );
    EVP_DecodeUpdate( &ectx, out, &outlen, (const unsigned char*)text.c_str(), text.size() );
    tlen += outlen;
    EVP_DecodeFinal( &ectx, out+tlen, &outlen );
    tlen += outlen;

    std::string data( (char*)out, tlen );
    free( out );
    return data;
}

bool gdl::Base64Decode(const std::string & b64str, std::vector<unsigned char > & vecResult)
{
    EVP_ENCODE_CTX  ctx;
    int srclen = (((b64str.length()+2)/4)*3)+1;
    int destlen = 0;

    vecResult.resize(srclen);
    unsigned char * destbuf = vecResult.data();
    EVP_DecodeInit(&ctx);

    EVP_DecodeUpdate(&ctx, destbuf, &destlen, (unsigned char *)b64str.c_str(), b64str.length());

    int tmplen = 0;

    EVP_DecodeFinal(&ctx, destbuf+destlen, &tmplen);
    destlen += tmplen;

    vecResult.resize(destlen);

    return !vecResult.empty();
}

int32_t gdl::Base64Encode(const char *encoded, int encoded_length, char *decoded){
    return EVP_EncodeBlock((unsigned char*)decoded, (const unsigned char*)encoded, encoded_length);
}

int32_t gdl::Base64Decode(const char *encoded, int encoded_length, char *decoded) {
    return EVP_DecodeBlock((unsigned char*)decoded, (const unsigned char*)encoded, encoded_length);
}