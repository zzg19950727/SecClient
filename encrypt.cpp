#include "encrypt.h"
#include <stdio.h>
#define BAK "c:\\bak"

bool write_head(FILE* fp)
{
    char buf[1024];
    for(int i=0; i<1024; i++)
        buf[i] = '0';
    int len = fwrite(buf, sizeof(char), 1024, fp);
    if( len != 1024 )
        return false;
    return true;
}

bool read_head(FILE* fp)
{
    char buf[1024] = {0};
    int len = fread(buf, sizeof(char), 1024, fp);
    if( len != 1024 )
        return false;
    return true;
}

bool encrypt_file(const char* filepath)
{
    if( !filepath )
        return false;

    FILE *fp = fopen(filepath, "r");
    FILE *out = fopen(BAK, "w");

    if(!fp || !out)
        return false;

    char buf[512];
    bool ret = false;

    if( !write_head(out) )
        goto quit;

    while( !feof(fp) )
    {
        int len = fread(buf, sizeof(char), 512, fp);

        for(int i=0; i<len; i++)
            buf[i] = buf[i]^0xff;
        len = fwrite(buf, sizeof(char), len, out);
    }
    ret = true;
    fclose(fp);
    fclose(out);

    if(remove(filepath)==0)
    {
        if( rename(BAK,filepath) )
            return false;
        else
            return true;
    }
    else
        return false;
quit:
    fclose(fp);
    fclose(out);
    return ret;
}

bool decrypt_file(const char* filepath)
{
    if( !filepath )
        return false;

    FILE *fp = fopen(filepath, "r");
    FILE *out = fopen(BAK, "w");

    if(!fp || !out)
        return false;

    char buf[512];
    bool ret = false;

    if( !read_head(fp) )
        goto quit;

    while( !feof(fp) )
    {
        int len = fread(buf, sizeof(char), 512, fp);

        for(int i=0; i<len; i++)
            buf[i] = buf[i]^0xff;
        len = fwrite(buf, sizeof(char), len, out);
    }
    ret = true;
    fclose(fp);
    fclose(out);

    if(remove(filepath)==0)
    {
        if( rename(BAK,filepath) )
            return false;
        else
            return true;
    }
    else
        return false;
quit:
    fclose(fp);
    fclose(out);
    return ret;
}
