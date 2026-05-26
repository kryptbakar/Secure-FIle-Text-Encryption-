#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

// XOR-based symmetric encryption. Encrypt and decrypt are identical operations.
// Output is hex-encoded so it is safe to store as text.
class Encryption {
public:
    // Encrypt plaintext with key; returns hex-encoded ciphertext.
    static std::string encrypt(const std::string& data, const std::string& key);

    // Decrypt hex-encoded ciphertext with key; returns original plaintext.
    static std::string decrypt(const std::string& hexData, const std::string& key);
};

#endif // ENCRYPTION_H
