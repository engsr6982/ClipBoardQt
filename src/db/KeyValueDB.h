#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


class KeyValueDB {
private:
    class KeyValueDBImpl;

    std::unique_ptr<KeyValueDBImpl> impl;

public:
    explicit KeyValueDB(std::filesystem::path const& path);

    explicit KeyValueDB(std::filesystem::path const& path, bool createIfMiss, bool fixIfError, int bloomFilterBit);

    KeyValueDB(KeyValueDB const&) noexcept = delete;

    KeyValueDB& operator=(KeyValueDB const&) noexcept = delete;

    KeyValueDB(KeyValueDB&&) noexcept;

    KeyValueDB& operator=(KeyValueDB&&) noexcept;

    ~KeyValueDB();

    std::optional<std::string> get(std::string_view key) const;

    bool has(std::string_view key) const;

    bool empty() const;

    bool set(std::string_view key, std::string_view val);

    bool del(std::string_view key);

    void iter(std::function<bool(std::string_view, std::string_view)> const& fn) const;
};
