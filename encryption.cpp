#include "encryption.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

static std::string toHex(const std::string& data) {
    std::ostringstream oss;
    for (unsigned char c : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    return oss.str();
}

static std::string fromHex(const std::string& hex) {
    if (hex.size() % 2 != 0)
        throw std::invalid_argument("Hex string length must be even");
    std::string out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2)
        out += static_cast<char>(std::stoi(hex.substr(i, 2), nullptr, 16));
    return out;
}

std::string Encryption::encrypt(const std::string& data, const std::string& key) {
    if (key.empty()) throw std::invalid_argument("Encryption key cannot be empty");
    std::string xored(data.size(), '\0');
    for (size_t i = 0; i < data.size(); ++i)
        xored[i] = static_cast<char>(data[i] ^ key[i % key.size()]);
    return toHex(xored);
}

std::string Encryption::decrypt(const std::string& hexData, const std::string& key) {
    if (key.empty()) throw std::invalid_argument("Encryption key cannot be empty");
    std::string raw = fromHex(hexData);
    for (size_t i = 0; i < raw.size(); ++i)
        raw[i] = static_cast<char>(raw[i] ^ key[i % key.size()]);
    return raw;
}
