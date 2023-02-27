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

#include <atomic>
#include <condition_variable>
#include <csignal>

#include <date/date.h>

std::mutex cv_m;
std::unique_lock<std::mutex> lock(cv_m);

std::condition_variable exitSignal;

std::atomic<int> exitSignalValue{0};

std::string pathToCert = "../test/data/Michal.pem";

BIO *out = NULL;

static void sigHandler(int signo)
{
	switch (signo)
	{
	case SIGTERM:
        std::cerr << "SIGTERM" << std::endl ;
		exitSignalValue = signo;
		exitSignal.notify_all();
		break;
	case SIGINT:
        std::cerr << "SIGINT" << std::endl ;
		exitSignalValue = signo;
		exitSignal.notify_all();
		break;
	default:
		break;
	}
}

int date_manimpulation_test() {
    std::tm tm = {0};
    //strptime("2023-10-26 16:00", "%Y-%m-%dT %H:%M", &tm);
    strptime("2013-12-04 15:03", "%Y-%m-%d %H:%M", &tm);
    std::cerr << "y:" << tm.tm_year << std::endl ;
    std::cerr << "m:" << tm.tm_mon << std::endl ;
    std::cerr << "d:" << tm.tm_mday << std::endl ;
    std::cerr << "h:" << tm.tm_hour << std::endl ;
    std::cerr << "m:" << tm.tm_min << std::endl ;
    std::cerr << "U:" << tm.tm_gmtoff << std::endl ;

    using namespace std::chrono;
    using namespace date;

    auto tp_old = system_clock::from_time_t(std::mktime(&tm));

    auto mon_correct(tm.tm_mon + 1);
    auto year_correct(tm.tm_year + 1900);

    auto ymd = year(year_correct) / mon_correct / tm.tm_mday;

    auto tp_new = sys_days(ymd) + hours(tm.tm_hour) + minutes(tm.tm_min) + seconds(tm.tm_sec);

    if ( tp_new != tp_old ) {
        std::cerr << "new:" << tp_new << std::endl ;
        std::cerr << "old:" << tp_old << std::endl ;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    int exitCode{EXIT_SUCCESS};

   	std::signal(SIGTERM, sigHandler);
	std::signal(SIGINT, sigHandler);

    SSL_load_error_strings();
    SSL_library_init();

    if (argc == 2 && std::string(argv[1]) == "date" )
    {
        std::cerr << "checkdate path" << std::endl ;
        // Check date - path

        return date_manimpulation_test();
    }

    if (argc == 2)
    {
        std::cout << "Cert loc:" << argv[1] << std::endl ;
        pathToCert = argv[1];
    }

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
    std::tm tm = {};

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
    ASN1_TIME_to_tm(X509_get_notAfter(cert), &tm);

    X509_free(cert);

    using namespace std::chrono;

    auto tp = system_clock::from_time_t(std::mktime(&tm));
    system_clock::duration remaining_days = tp - system_clock::now() ;

    // convert to number of days:
    typedef duration<int, std::ratio<60 * 60 * 24>> days_type;
    days_type ndays = duration_cast<days_type>(remaining_days);

    std::cout << "Remaining Days: " << ndays.count() << std::endl;

    auto tp1 = system_clock::from_time_t(std::mktime(&tm));
    auto nhours = duration_cast<hours>(tp1 - system_clock::now());
    std::cout << "Certificate remaining days(tp):" << nhours.count() / 24 << std::endl ;
/*
    auto expireTimePoint = std::chrono::system_clock::now() + 10s;

    while ( exitSignalValue == 0 )
    {
        if (exitSignal.wait_until(lock,expireTimePoint) == std::cv_status::timeout)
        {
            std::cerr << "ok - timeout hit." << std::endl;
            break;
        }
        std::cerr << "other exit." << std::endl;
        exitCode = EXIT_FAILURE;
    }
*/
    return exitCode;
}
