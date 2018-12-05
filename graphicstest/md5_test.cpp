#include "md5.h"
#include <iostream>
 
using namespace std;
 
void PrintMD5(const string &str, MD5 &md5) {
        cout << "MD5(\"" << str << "\") = " << md5.toString() << endl;

}
 
string FileDigest(const string &file) {
 
        ifstream in(file.c_str(), ios::binary);
        if (!in)
                return "";
 
        MD5 md5;
        std::streamsize length;
        char buffer[1024];
        while (!in.eof()) {
                in.read(buffer, 1024);
                length = in.gcount();
                if (length > 0)
                        md5.update(buffer, length);
        }
        in.close();
        return md5.toString();
}
 
int md5_test_main() {
 
//        cout << MD5("abc").toString() << endl;
//        cout << MD5(ifstream("D:\\test.txt")).toString() << endl;
//        cout << MD5(ifstream("D:\\test.exe", ios::binary)).toString() << endl;
//        cout << FileDigest("D:\\test.exe") << endl;
 
        MD5 md5;
        md5.update("");
        PrintMD5("", md5);
 
        md5.reset();
        md5.update("a");
        PrintMD5("a", md5);
 
        md5.reset();
        md5.update("abc");
        PrintMD5("abc", md5);
 
        md5.reset();
        md5.update("abcdefghijklmnopqrstuvwxyz");
        PrintMD5("abcdefghijklmnopqrstuvwxyz", md5);

        md5.reset();
        md5.update("message digest");
        PrintMD5("message digest", md5);
 
        int size = 640*480*sizeof(char);
        char *buf = (char*)malloc(size);
        memset(buf, 0, size);
        md5.reset();
        md5.update(buf, size);
        PrintMD5("buf", md5);

        MD5 md5_2(buf, size);
        PrintMD5("buf_md5_2", md5_2);

        free(buf);
//        md5.reset();
//        md5.update(ifstream("D:\\test.txt"));
//        PrintMD5("D:\\test.txt", md5);
        return 1;
}
