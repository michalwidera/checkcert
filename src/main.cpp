#include <cstdio>
#include <string>
#include <iostream>


// https://serverfault.com/questions/9708/what-is-a-pem-file-and-how-does-it-differ-from-other-openssl-generated-key-file

#include <openssl/bio.h> /* BasicInput/Output streams */
#include <openssl/err.h> /* errors */
#include <openssl/ssl.h> /* core library */
#include <openssl/pem.h> /* pem files support */

std::string pathToCert = "certificate.pem";

int main() {
    SSL_load_error_strings();
    SSL_library_init();

    FILE *fp = fopen(pathToCert.c_str(), "r");
    if (!fp)
    {
        std::cout << "Could not open certificate";
        return 0;
    }
    X509 *cert = PEM_read_X509(fp, NULL, NULL, NULL);
    if (!cert)
    {
        std::cout << "Could not parse certificate";
        return 0;
    }
    fclose(fp);
    // certificate opened successfully
    // do something with `cert`
    // call X509_free(cert); to free the memory and prevent memory leaks

    X509_free(cert);

    return 1;
}
