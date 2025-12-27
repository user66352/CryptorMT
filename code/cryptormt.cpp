#include <iostream>

#include "headers/global.h"
#include "headers/clipp.h"
#include "headers/encrypt.h"
#include "headers/decrypt.h"

std::string cryptormt_ver = "0.3";

bool argparse(int argc, char* argv[], Options *o);
bool testArgs(Options *o);

int main(int argc, char* argv[])
{
    Options args;

    if (!argparse(argc, argv, &args))
    {
        return 1;
    }

    if(args.version)
    {
        std::cout << "CryptorMT Version: " << cryptormt_ver << std::endl;
        return 0;
    }

    if(!testArgs(&args))
    {
        std::cerr << "Program stopped due to a argument parsing error." << std::endl;
        return 1;
    }

    if(args.encrypt)
    {
        ENCRYPT en(&args);
        en.startEncryption();
    }

    if(args.decrypt)
    {
        DECRYPT dc(&args);
        return dc.startDecryption();
    }

    return 0;
}

bool testArgs(Options *o)
{
    if(o->pw.length() > maxPasswordLength)
    {
        std::cerr << "Password length should not exceed 64 characters.";
        return false;
    }

    if(o->decrypt && o->encrypt)
    {
        std::cerr << "Only -d or -e can be used. Not both at the same time." << std::endl;
        return false;
    }

    if(o->pw.empty() && o->keyfile.empty())
    {
        std::cerr << "CryptorMT requires either a password, a keyfile or both. None was provided." << std::endl;
        return false;
    }

    if(!(o->decrypt || o->encrypt)) o->encrypt = true;

    if(o->infile.empty()) o->stdi = true;

    if(o->outfile.empty()) o->stdo = true;

    return true;
}

bool argparse(int argc, char* argv[], Options *o)
{
    bool inputState = true;

    clipp::group cli = (
        clipp::option("-p").doc("password used for de-/encryption") & clipp::value("password", o->pw),
        clipp::option("-k").doc("optional keyfile, min keyfile length is 2 byte, max length is 4992 byte, only the first 4992 byte of a keyfile will be used, those should be random bytes as well") & clipp::value("", o->keyfile),
        clipp::option("-i").doc("input file, if omitted cryptormt will read from stdin") & clipp::value("", o->infile),
        clipp::option("-o").doc("output file, if omitted cryptormt will write to stdout") & clipp::value("", o->outfile),
        clipp::option("-e").set(o->encrypt).doc("encrypt file, if omitted cryptormt defaults to encryption mode"),
        clipp::option("-d").set(o->decrypt).doc("decrypt file"),
        clipp::option("-v").set(o->version).doc("print version, all other options will be ignored"),
        clipp::option("-h").set(o->help).doc("print help")
    );

    if((!clipp::parse(argc, argv, cli) || o->help) && !o->version)
    {
        std::cerr << clipp::make_man_page(cli, argv[0]);
        inputState = false;
    }

    return inputState;
}
