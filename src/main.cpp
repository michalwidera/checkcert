#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <chrono>

// https://serverfault.com/questions/9708/what-is-a-pem-file-and-how-does-it-differ-from-other-openssl-generated-key-file

#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
#include <openssl/pem.h> /* pem files support */

#include <openssl/x509_vfy.h>

std::string pathToCert = "../test/data/Michal.pem";

BIO *out = NULL;

int main()
{
    SSL_load_error_strings();
    SSL_library_init();

    FILE *fp = fopen(pathToCert.c_str(), "r");
    if (!fp)
    {
        std::cout << "Could not open certificate" << std::endl;
        return 0;
    }
    X509 *cert = PEM_read_X509(fp, NULL, NULL, NULL);
    if (!cert)
    {
        std::cout << "Could not parse certificate" << std::endl;
        return 0;
    }

    ASN1_TIME *timeNotAfter = X509_get_notAfter(cert);
    ASN1_TIME *timeNotBefore = X509_get_notBefore(cert);

    const int len = 1024;
    char buf[len];
    BIO *bio = BIO_new(BIO_s_mem());
    int write = 0;
    //bio = BIO_new_fp(stdout, BIO_NOCLOSE);
    if (bio)
    {
        if (ASN1_TIME_print(bio, X509_get0_notAfter(cert)))
            write = BIO_read(bio, buf, len - 1);
        BIO_printf(bio, "\n");
        BIO_free(bio);
    }
    else
    {
        std::cout << "no bio" << std::endl;
    }
    buf[write] = '\0';

    printf("%s\n", buf);
    fclose(fp);
    // certificate opened successfully
    // do something with `cert`
    // call X509_free(cert); to free the memory and prevent memory leaks

    X509_free(cert);

    using namespace std::chrono;

    std::tm tm = {};
    //std::stringstream ss("Jan 9 12:35:34 2014");
    std::stringstream ss(buf);
    ss >> std::get_time(&tm, "%b %d %H:%M:%S %Y");
    auto tp = system_clock::from_time_t(std::mktime(&tm));
    system_clock::duration remaining_days = tp - system_clock::now() ;

    // convert to number of days:
    typedef duration<int, std::ratio<60 * 60 * 24>> days_type;
    days_type ndays = duration_cast<days_type>(remaining_days);

    std::cout << "Remaining Days: " << ndays.count() << std::endl;

    return 1;
}
