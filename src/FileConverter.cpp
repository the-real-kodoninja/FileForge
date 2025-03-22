// FileConverter.cpp
#include "FileConverter.hpp"
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>
#include <fstream>

namespace fs = std::filesystem;

bool FileConverter::convertAndCompress(const std::string& input, const std::string& output, 
                                       const std::string& format, int compressionLevel) {
    if (format == ".zip") {
        struct archive *a = archive_write_new();
        archive_write_add_filter_zip(a);
        archive_write_set_format_zip(a);
        archive_write_set_options(a, ("compression-level=" + std::to_string(compressionLevel * 3)).c_str());

        if (archive_write_open_filename(a, output.c_str()) != ARCHIVE_OK) {
            archive_write_free(a);
            return false;
        }

        struct archive_entry *entry = archive_entry_new();
        archive_entry_set_pathname(entry, fs::path(input).filename().string().c_str());
        archive_entry_set_size(entry, fs::file_size(input));
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_write_header(a, entry);

        std::ifstream file(input, std::ios::binary);
        char buffer[8192];
        while (file.read(buffer, sizeof(buffer))) {
            archive_write_data(a, buffer, file.gcount());
        }
        archive_write_data(a, buffer, file.gcount());
        file.close();

        archive_entry_free(entry);
        archive_write_close(a);
        archive_write_free(a);
        return true;
    } else if (format == ".mp3" || format == ".mp4") {
        // Placeholder for ffmpeg (see enhancement below)
        std::string cmd = "ffmpeg -i " + input + " -y " + output;
        return system(cmd.c_str()) == 0;
    } else {
        // Simple copy for unsupported formats
        fs::copy(input, output, fs::copy_options::overwrite_existing);
        return true;
    }
}