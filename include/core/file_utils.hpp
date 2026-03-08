#ifndef VAPI_CORE_FILE_UTILS_HPP
#define VAPI_CORE_FILE_UTILS_HPP

/** @file file_utils.hpp
 *  @brief File helpers: path utils, read/write text and binary, FileByteSource/Sink, tempFilePath.
 *
 *  Все функции, возвращающие Result<...>, при ошибке используют коды vapi::errors::file:: (см. error.hpp).
 *  Синтаксис: if (!res) return std::unexpected(res.error()); или VAPI_TRY / VAPI_TRY_VOID. */

#include "core/error.hpp"
#include "core/interfaces/i_byte.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <optional>
#include <span>
#include <chrono>

namespace vapi::file {

namespace fs = std::filesystem;

/** @brief Источник байтов из файла; при ошибке открытия последующие read() возвращают ошибку. */
class FileByteSource : public IByteSource {
public:
    explicit FileByteSource(const fs::path& p);
    [[nodiscard]] Result<std::vector<u8>> read(usize max = kReadAll) override;

private:
    fs::path path_;
    std::ifstream stream_;
    std::optional<Error> err_;
};

/** @brief Приёмник байтов в файл; при ошибке открытия последующие write/flush возвращают ошибку. */
class FileByteSink : public IByteSink {
public:
    explicit FileByteSink(const fs::path& p, bool append = false);
    [[nodiscard]] Result<void> write(std::span<const u8> data) override;
    [[nodiscard]] Result<void> flush() override;

private:
    fs::path path_;
    std::ofstream stream_;
    std::optional<Error> err_;
};

[[nodiscard]] bool fileExists(const fs::path& p) noexcept;
[[nodiscard]] bool isFile(const fs::path& p) noexcept;
[[nodiscard]] bool isDirectory(const fs::path& p) noexcept;
[[nodiscard]] bool isSymlink(const fs::path& p) noexcept;
[[nodiscard]] std::optional<usize> fileSize(const fs::path& p) noexcept;

/** @brief Читает весь файл как текст. Ошибки: NotFound, OpenFailed, ReadFailed. */
[[nodiscard]] Result<std::string> readText(const fs::path& p);
/** @brief Читает файл как бинарный буфер. */
[[nodiscard]] Result<std::vector<u8>> readBinary(const fs::path& p);
[[nodiscard]] Result<std::vector<std::string>> readLines(const fs::path& p);
/** @brief Читает текст с ограничением размера (по умолчанию 64 MiB). При превышении — ReadFailed. */
[[nodiscard]] Result<std::string> readTextLimited(const fs::path& p, usize max = 64*1024*1024);

[[nodiscard]] Result<void> writeText(const fs::path& p, std::string_view c, bool append = false);
[[nodiscard]] Result<void> writeBinary(const fs::path& p, std::span<const u8> d, bool append = false);
[[nodiscard]] Result<void> writeLines(const fs::path& p, std::span<const std::string> lines, bool append = false);
[[nodiscard]] Result<void> appendText(const fs::path& p, std::string_view c);

[[nodiscard]] Result<void> createDirectories(const fs::path& p);
[[nodiscard]] Result<void> copyFile(const fs::path& from, const fs::path& to, bool overwrite = false);
[[nodiscard]] Result<void> removeFile(const fs::path& p);
[[nodiscard]] Result<void> renameFile(const fs::path& from, const fs::path& to);

[[nodiscard]] std::string getExtension(const fs::path& p);
[[nodiscard]] std::string getStem(const fs::path& p);
[[nodiscard]] fs::path getParent(const fs::path& p);
[[nodiscard]] Result<std::vector<fs::path>> listFiles(const fs::path& dir, bool recursive = false);
[[nodiscard]] Result<std::vector<fs::path>> listFilesByExtension(const fs::path& dir, std::string_view ext, bool rec = false);
[[nodiscard]] Result<fs::file_time_type> lastModified(const fs::path& p);
[[nodiscard]] bool isNewerThan(const fs::path& a, const fs::path& b);
[[nodiscard]] Result<fs::path> canonicalize(const fs::path& p);

/** @brief Resolves path and ensures it lies under allowedRoot (jail). Use for safe file access. */
[[nodiscard]] Result<fs::path> resolveSecure(const fs::path& path, const fs::path& allowedRoot);

/** @brief Returns true if key is safe as a path component (no "..", no path separators). For shader cache keys. */
[[nodiscard]] bool isPathKeySafe(std::string_view key) noexcept;

/** @brief Путь к временному файлу в системной temp-директории (уникальное имя по prefix + timestamp + ext). */
[[nodiscard]] fs::path tempFilePath(std::string_view prefix = "vapi_", std::string_view ext = "tmp");

} // namespace vapi::file

#endif
