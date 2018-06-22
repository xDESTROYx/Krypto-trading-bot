#ifndef K_FN_H_
#define K_FN_H_

#define _numsAz_ "0123456789"                 \
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                 "abcdefghijklmnopqrstuvwxyz"

namespace K {
  class FN {
    public:
      static string strX(const double &d, const unsigned int &X) {
        stringstream ss;
        ss << setprecision(X)
           << fixed << d;
        return ss.str();
      };
      static string str8(const double &d) { return strX(d, 8); };
      static double d8(const double &d) { return stod(str8(d)); };
      static string strL(string s) { transform(s.begin(), s.end(), s.begin(), ::tolower); return s; };
      static string strU(string s) { transform(s.begin(), s.end(), s.begin(), ::toupper); return s; };
      static bool trueOnce(bool *const k) { return *k ? !(*k = !*k) : *k; };
      static unsigned long long int64() {
        static random_device rd;
        static mt19937_64 gen(rd());
        return uniform_int_distribution<unsigned long long>()(gen);
      };
      static string int45Id() { return to_string(int64()).substr(0, 10); };
      static string int32Id() { return to_string(int64()).substr(0,  8); };
      static string char16Id() {
        char s[16];
        for (unsigned int i = 0; i < 16; ++i) s[i] = _numsAz_[int64() % (sizeof(_numsAz_) - 1)];
        return string(s, 16);
      };
      static string uuid36Id() {
        string uuid = string(36, ' ');
        unsigned long long rnd = int64();
        unsigned long long rnd_ = int64();
        uuid[8] = '-';
        uuid[13] = '-';
        uuid[18] = '-';
        uuid[23] = '-';
        uuid[14] = '4';
        for (unsigned int i=0;i<36;i++)
          if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
            if (rnd <= 0x02) rnd = 0x2000000 + (rnd_ * 0x1000000) | 0;
            rnd >>= 4;
            uuid[i] = _numsAz_[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
          }
        return strL(uuid);
      };
      static string uuid32Id() {
        string uuid = uuid36Id();
        uuid.erase(remove(uuid.begin(), uuid.end(), '-'), uuid.end());
        return uuid;
      }
      static string oZip(string k) {
        z_stream zs;
        if (inflateInit2(&zs, -15) != Z_OK) return "";
        zs.next_in = (Bytef*)k.data();
        zs.avail_in = k.size();
        int ret;
        char outbuffer[32768];
        string k_;
        do {
          zs.avail_out = 32768;
          zs.next_out = (Bytef*)outbuffer;
          ret = inflate(&zs, Z_SYNC_FLUSH);
          if (k_.size() < zs.total_out)
            k_.append(outbuffer, zs.total_out - k_.size());
        } while (ret == Z_OK);
        inflateEnd(&zs);
        if (ret != Z_STREAM_END) return "";
        return k_;
      };
      static string oHex(string k) {
       unsigned int len = k.length();
        string k_;
        for (unsigned int i=0; i < len; i+=2) {
          string byte = k.substr(i, 2);
          char chr = (char)(int)strtol(byte.data(), NULL, 16);
          k_.push_back(chr);
        }
        return k_;
      };
      static string oB64(string k) {
        BIO *bio, *b64;
        BUF_MEM *bufferPtr;
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_write(bio, k.data(), k.length());
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);
        BIO_set_close(bio, BIO_NOCLOSE);
        BIO_free_all(bio);
        return string(bufferPtr->data, bufferPtr->length);
      };
      static string oB64decode(string k) {
        BIO *bio, *b64;
        char buffer[k.length()];
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_mem_buf(k.data(), k.length());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_set_close(bio, BIO_NOCLOSE);
        int len = BIO_read(bio, buffer, k.length());
        BIO_free_all(bio);
        return string(buffer, len);
      };
      static string oMd5(string k) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5((unsigned char*)k.data(), k.length(), (unsigned char*)&digest);
        char k_[16*2+1];
        for (unsigned int i = 0; i < 16; i++) sprintf(&k_[i*2], "%02x", (unsigned int)digest[i]);
        return strU(k_);
      };
      static string oSha256(string k) {
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)k.data(), k.length(), (unsigned char*)&digest);
        char k_[SHA256_DIGEST_LENGTH*2+1];
        for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++) sprintf(&k_[i*2], "%02x", (unsigned int)digest[i]);
        return k_;
      };
      static string oSha512(string k) {
        unsigned char digest[SHA512_DIGEST_LENGTH];
        SHA512((unsigned char*)k.data(), k.length(), (unsigned char*)&digest);
        char k_[SHA512_DIGEST_LENGTH*2+1];
        for (unsigned int i = 0; i < SHA512_DIGEST_LENGTH; i++) sprintf(&k_[i*2], "%02x", (unsigned int)digest[i]);
        return k_;
      };
      static string oHmac256(string p, string s, bool hex = false) {
        unsigned char* digest;
        digest = HMAC(EVP_sha256(), s.data(), s.length(), (unsigned char*)p.data(), p.length(), NULL, NULL);
        char k_[SHA256_DIGEST_LENGTH*2+1];
        for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++) sprintf(&k_[i*2], "%02x", (unsigned int)digest[i]);
        return hex ? oHex(k_) : k_;
      };
      static string oHmac512(string p, string s) {
        unsigned char* digest;
        digest = HMAC(EVP_sha512(), s.data(), s.length(), (unsigned char*)p.data(), p.length(), NULL, NULL);
        char k_[SHA512_DIGEST_LENGTH*2+1];
        for (unsigned int i = 0; i < SHA512_DIGEST_LENGTH; i++) sprintf(&k_[i*2], "%02x", (unsigned int)digest[i]);
        return k_;
      };
      static string oHmac384(string p, string s) {
        unsigned char* digest;
        digest = HMAC(EVP_sha384(), s.data(), s.length(), (unsigned char*)p.data(), p.length(), NULL, NULL);
        char k_[SHA384_DIGEST_LENGTH*2+1];
        for (unsigned int i = 0; i < SHA384_DIGEST_LENGTH; i++) sprintf(&k_[i*2], "%02x", (unsigned int)digest[i]);
        return k_;
      };
      static string output(const string &cmd) {
        string data;
        FILE *stream = popen((cmd + " 2>&1").data(), "r");
        if (stream) {
          const int max_buffer = 256;
          char buffer[max_buffer];
          while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL)
              data += buffer;
          pclose(stream);
        }
        return data;
      };
      static string changelog() {
        return output("test -d .git && git --no-pager log --graph --oneline @..@{u}");
      };
      static void stunnel(const bool &reboot) {
        system("pkill stunnel || :");
        if (reboot) system("stunnel etc/stunnel.conf");
      };
      static json wJet(const string &url, long timeout = 13) {
        return curl_perform(url, [&](CURL *curl) {
          curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        }, timeout == 13);
      };
      static json wJet(const string &url, string p) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, "Content-Type: application/x-www-form-urlencoded");
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p.data());
        });
      };
      static json wJet(const string &url, string t, bool auth) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          if (!t.empty()) h_ = curl_slist_append(h_, ("Authorization: Bearer " + t).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
        });
      };
      static json wJet(const string &url, bool p, string a, string s, string n) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, ("API-Key: " + a).data());
          h_ = curl_slist_append(h_, ("API-Sign: " + s).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, n.data());
        });
      };
      static json wJet(const string &url, bool a, string p, string s) {
        return curl_perform(url, [&](CURL *curl) {
          if (a) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, s.data());
          }
          curl_easy_setopt(curl, CURLOPT_USERPWD, p.data());
        });
      };
      static json wJet(const string &url, string p, string s, bool post) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, ("X-Signature: " + s).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p.data());
        });
      };
      static json wJet(const string &url, string p, string a, string s) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, "Content-Type: application/x-www-form-urlencoded");
          h_ = curl_slist_append(h_, ("Key: " + a).data());
          h_ = curl_slist_append(h_, ("Sign: " + s).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p.data());
        });
      };
      static json wJet(const string &url, string p, string a, string s, bool post) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, ("X-BFX-APIKEY: " + a).data());
          h_ = curl_slist_append(h_, ("X-BFX-PAYLOAD: " + p).data());
          h_ = curl_slist_append(h_, ("X-BFX-SIGNATURE: " + s).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p.data());
        });
      };
      static json wJet(const string &url, string p, string a, string s, bool post, bool auth) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, "Content-Type: application/x-www-form-urlencoded");
          if (!a.empty()) h_ = curl_slist_append(h_, ("Authorization: Bearer " + a).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p.data());
        });
      };
      static json wJet(const string &url, string t, string a, string s, string p, bool d = false) {
        return curl_perform(url, [&](CURL *curl) {
          struct curl_slist *h_ = NULL;
          h_ = curl_slist_append(h_, ("CB-ACCESS-KEY: " + a).data());
          h_ = curl_slist_append(h_, ("CB-ACCESS-SIGN: " + s).data());
          h_ = curl_slist_append(h_, ("CB-ACCESS-TIMESTAMP: " + t).data());
          h_ = curl_slist_append(h_, ("CB-ACCESS-PASSPHRASE: " + p).data());
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, h_);
          if (d) curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        });
      };
      static json curl_perform(const string &url, function<void(CURL *curl)> curl_setopt, bool debug = true) {
        string reply;
        CURL *curl = curl_easy_init();
        if (curl) {
          curl_setopt(curl);
          curl_easy_setopt(curl, CURLOPT_USERAGENT, "K");
          curl_easy_setopt(curl, CURLOPT_CAINFO, "etc/cabundle.pem");
          curl_easy_setopt(curl, CURLOPT_INTERFACE, args.inet);
          curl_easy_setopt(curl, CURLOPT_URL, url.data());
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reply);
          CURLcode r = curl_easy_perform(curl);
          if (debug and r != CURLE_OK)
            reply = string("{\"error\":\"CURL Error: ") + curl_easy_strerror(r) + "\"}";
          curl_easy_cleanup(curl);
        }
        if (reply.empty() or (reply[0] != '{' and reply[0] != '[')) reply = "{}";
        return json::parse(reply);
      };
      static size_t curl_write(void *buf, size_t size, size_t nmemb, void *up) {
        ((string*)up)->append((char*)buf, size * nmemb);
        return size * nmemb;
      };
  };
}

#endif
