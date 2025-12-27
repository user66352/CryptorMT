# CryptorMT  

CryptorMT aims to enable the long period (2^19937-1) of Mersenne Twister as a viable option for cryptography.  


## Thoughts  

The main downside of Mersenne Twister as a cipher generator is that its generator state can be calculated if enough consecutive output bits becoming known to an attacker.  
Another point of criticism is its output can be distinguished from a true random stream (given enough enough consecutive output bits being monitored).  
So why use MT?  
Why not.  
I don’t think that the 2nd point is a counter argument in all cases.  
Especially if an attacker already knows that the stream in front of him contains encrypted data, deniability is probably not a concern at this point.  
Remains the fact that the MT generator state can be calculated with enough generated bits becoming available.  
So I had the “genius” idea to even encrypt the stream-cipher used for encryption.  
Therefore CryptorMT uses 2 MT generators creating 2 independent cipher streams which than are xor`ed together into the final cipher used to encrypt the plain text.  
If the encryption cipher is partially revealed, for example if parts of the plain text are known, the attacker is left with a cipher sequence that does not represent any of the actual MT output.  
Thats not an efficient approach to secure the generator state of the PRNG but efficiency is not the goal of that project.  
This project is more a proof of concept to explore an idea.  
By doing so a “secure enough” algorithm might emerge.  
And its fun.  


## Philosophy  

How can an encryption method be “secure enough”?  
Shouldn't all encryption methods strive for maximum security?  
I don’t think that`s always necessary.  
Imho a lot of encrypted data has a maximum meaningful lifespan.  
Lets think of company A forwarding encrypted documents to another branch office over a public net and lets also assume evil competitor B gets hold of these documents.  
Now what?  
How valuable will those data actually be?  
At this point still unknown to company B.  
Throwing all available resources onto brut-force decryption? Unlikely.  
But even if, depending on the encryption method, breaking it might still take millennia.  
In this example 4 years later company A releases its final product.  
No later than this point in time the captured, still encrypted packets, very likely loose their value to company B.  
There is hardly a competitive advantage to be gained by still trying to extract outdated data.  
Under this assumption an encryption method just needs to be “good enough” for its intended purpose.  
Maybe data security is not so much defined by just the hardness of its algorithm but also by not knowing which algorithm was actually used for encryption.  
In such a world everybody would/could use their own and unique algorithm.  
True, that’s unrealistic.  
But what if users could just choose from a plethora of “good enough” algorithms?  
Considering these points I think there is always room for another encryption tool.  


## Installation  

I haven’t bothered mastering make (yet).  
But for compilation basically any modern C++ compiler will do.  
Most Linux distributions should provide g++ for example.  
So make sure g++ is installed (or the C++ compiler of your liking).  
All required non-standard libraries are contained within the package.  
Download the zip file and extract it to a temporary folder.  
Or download it with:  

`git clone https://github.com/user66352/CryptorMT`

From within that folder cd into subfolder ‘code’.  
In here ‘ls’ should show you the file ‘cryptormt.cpp’.  
Inside folder ‘code’ issue the following command:  

`g++ -Wall -std=c++17 cryptormt.cpp ./src/*.cpp ./src/*.c -o cryptormt`  

For clang you can use:  

`clang++ -x c++ -Wall -std=c++17 cryptormt.cpp ./src/*.cpp ./src/*.c -o cryptormt`  

That will create the binary ‘cryptormt’ inside the same folder.  
When compiled with optimizations enabled (-O3) I have seen encryption/decryption times halved.  


## Usage  

CryptorMT can read/write from/to stdin and stdout.  
But in- and outfile can be explicetly named.  

Encrypting a file with a password:  

`cryptormt -p 1234567890 -i alice_in_wonderland.txt -o alice_in_wonderland.txt.cpt`  
`cat alice_in_wonderland.txt | cryptormt -p 1234567890 > alice_in_wonderland.txt.cpt`  
`cryptormt -p 1234567890 < alice_in_wonderland.txt > alice.txt.cpt`  

Encrypting a file with a keyfile: 

`cat alice_in_wonderland.txt | cryptormt -k keyfile > alice_in_wonderland.txt.cpt`  

Compress and encrypt a file:

`gzip -c alice\_in\_wonderland.txt | cryptormt -p 1234 > alice.gz.cpt`  

To Decrypt a file:  

`cryptormt -d -p 1234567890 -i alice.txt.cpt -o alice.txt`  
`cryptormt -d -p 1234567890 < alice.txt.cpt > alice.txt`  

Even encrypted file transfer over networks is possible (with constraints).  
First setup the receiver:  

`nc -nvlp 1234 | cryptormt -d -p 1234 > alice.txt`  

Than the sender:  

`bar alice_in_wonderland.txt | cryptormt -p 1234 | nc <receiver ip> 1234`  

The problem here is at this moment buffering prevents some processes from detecting an end-of-file.  
So once “bar” reports 100% for the sender than just hit Ctrl+C on the sender cli.  
That will start flushing all buffers and also ends the receiver process.  


## Algorithm

CryptorMT can be seeded with a password, a keyfile or both.  
The internal seed has a maximum length of 4992 Byte (2x the MT generator state of 624 32-bit words).  
This state consists of 2 independent seed arrays of 2496 Byte (one per generator).  
That state can be initialized with a keyfile of up to 4992 Byte.  
At least only the first 4992 Byte are considered.  
If a keyfile is being used it should consist of only random bytes.  
Under Linux that can be achieved with dd:  

`dd if=/dev/urandom of=keyfile bs=100 count=1`  

This will create a keyfile of 100 random Bytes.  
If a password is used its translated into an SHA512 hash.  
That’s also the maximum password length by now.  
This hash is added to the first seed array and the hash of this hash becomes the seed for the 2nd generator.  
An additional random salt of 64 byte per generator is than added to both MT`s to assure a different cipher even if the same password/keyfile is being reused.  
That salt is encrypted and stored at the start of the encrypted file (first 128 byte).  


## Mentions  

This project uses additional libraries from other projects:  

Mersenne Twister (permissive software license; see inside source code headers/mt19937ar.h):  
https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/emt19937ar.html  

clipp.h for parsing command line arguments (MIT License):  
https://github.com/muellan/clipp  

SHA512 Algorithm (MIT License):  
https://github.com/pr0f3ss/SHA  
