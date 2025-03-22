#include "FileConverter.hpp"
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>
#include <fstream>

namespace fs = std::filesystem;

FileConverter::FileConverter() {
    av_register_all(); // Legacy FFmpeg init
}

FileConverter::~FileConverter() {}

bool FileConverter::convertAndCompress(const std::string& input, const std::string& output, 
                                       const std::string& format, int compressionLevel, 
                                       std::string& errorMsg, std::function<void(int)> progressCallback) {
    if (format == ".zip" || format == ".tar.gz") {
        return compressToArchive(input, output, format, compressionLevel, errorMsg, progressCallback);
    } else if (format == ".mp3" || format == ".mp4") {
        return transcodeWithFFmpeg(input, output, format, errorMsg, progressCallback);
    } else {
        try {
            fs::copy(input, output, fs::copy_options::overwrite_existing);
            progressCallback(100);
            return true;
        } catch (const std::exception& e) {
            errorMsg = "Copy failed: " + std::string(e.what());
            return false;
        }
    }
}

bool FileConverter::compressToArchive(const std::string& input, const std::string& output, 
                                      const std::string& format, int compressionLevel, 
                                      std::string& errorMsg, std::function<void(int)> progressCallback) {
    struct archive *a = archive_write_new();
    if (format == ".zip") {
        archive_write_add_filter_zip(a);
        archive_write_set_format_zip(a);
    } else if (format == ".tar.gz") {
        archive_write_add_filter_gzip(a);
        archive_write_set_format_ustar(a);
    }
    archive_write_set_options(a, ("compression-level=" + std::to_string(compressionLevel * 3)).c_str());

    if (archive_write_open_filename(a, output.c_str()) != ARCHIVE_OK) {
        errorMsg = "Failed to open archive: " + std::string(archive_error_string(a));
        archive_write_free(a);
        return false;
    }

    struct archive_entry *entry = archive_entry_new();
    archive_entry_set_pathname(entry, fs::path(input).filename().string().c_str());
    uint64_t totalSize = fs::file_size(input);
    archive_entry_set_size(entry, totalSize);
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0644);
    archive_write_header(a, entry);

    std::ifstream file(input, std::ios::binary);
    char buffer[8192];
    uint64_t bytesWritten = 0;
    while (file.read(buffer, sizeof(buffer))) {
        archive_write_data(a, buffer, file.gcount());
        bytesWritten += file.gcount();
        progressCallback(static_cast<int>(bytesWritten * 100 / totalSize));
    }
    archive_write_data(a, buffer, file.gcount());
    bytesWritten += file.gcount();
    progressCallback(100);

    file.close();
    archive_entry_free(entry);
    archive_write_close(a);
    archive_write_free(a);
    return true;
}

bool FileConverter::transcodeWithFFmpeg(const std::string& input, const std::string& output, 
                                        const std::string& format, std::string& errorMsg, 
                                        std::function<void(int)> progressCallback) {
    AVFormatContext *inCtx = nullptr, *outCtx = nullptr;
    AVCodecContext *decCtx = nullptr, *encCtx = nullptr;
    int ret;

    // Open input
    if ((ret = avformat_open_input(&inCtx, input.c_str(), nullptr, nullptr)) < 0) {
        errorMsg = "Could not open input file";
        return false;
    }
    if ((ret = avformat_find_stream_info(inCtx, nullptr)) < 0) {
        errorMsg = "Could not find stream info";
        avformat_close_input(&inCtx);
        return false;
    }

    // Setup output
    if ((ret = avformat_alloc_output_context2(&outCtx, nullptr, nullptr, output.c_str())) < 0) {
        errorMsg = "Could not create output context";
        avformat_close_input(&inCtx);
        return false;
    }

    // Find video/audio stream and setup codec
    int streamIdx = av_find_best_stream(inCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (streamIdx < 0) streamIdx = av_find_best_stream(inCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (streamIdx < 0) {
        errorMsg = "No audio/video stream found";
        avformat_close_input(&inCtx);
        avformat_free_context(outCtx);
        return false;
    }

    AVStream *inStream = inCtx->streams[streamIdx];
    AVCodec *decCodec = avcodec_find_decoder(inStream->codecpar->codec_id);
    decCtx = avcodec_alloc_context3(decCodec);
    avcodec_parameters_to_context(decCtx, inStream->codecpar);
    if ((ret = avcodec_open2(decCtx, decCodec, nullptr)) < 0) {
        errorMsg = "Could not open decoder";
        goto cleanup;
    }

    AVStream *outStream = avformat_new_stream(outCtx, nullptr);
    AVCodec *encCodec = avcodec_find_encoder(format == ".mp3" ? AV_CODEC_ID_MP3 : AV_CODEC_ID_H264);
    encCtx = avcodec_alloc_context3(encCodec);
    encCtx->sample_rate = decCtx->sample_rate;
    encCtx->channel_layout = decCtx->channel_layout;
    encCtx->channels = decCtx->channels;
    encCtx->bit_rate = 128000; // Example: 128 kbps for audio
    encCtx->time_base = {1, 1000};
    if (decCtx->codec_type == AVMEDIA_TYPE_VIDEO) {
        encCtx->width = decCtx->width;
        encCtx->height = decCtx->height;
        encCtx->pix_fmt = decCtx->pix_fmt;
    }
    if ((ret = avcodec_open2(encCtx, encCodec, nullptr)) < 0) {
        errorMsg = "Could not open encoder";
        goto cleanup;
    }
    avcodec_parameters_from_context(outStream->codecpar, encCtx);

    if (!(outCtx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&outCtx->pb, output.c_str(), AVIO_FLAG_WRITE)) < 0) {
            errorMsg = "Could not open output file";
            goto cleanup;
        }
    }
    if ((ret = avformat_write_header(outCtx, nullptr)) < 0) {
        errorMsg = "Error writing header";
        goto cleanup;
    }

    // Transcode
    AVPacket pkt;
    AVFrame *frame = av_frame_alloc();
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    int64_t totalDuration = inCtx->duration / AV_TIME_BASE;
    int64_t processed = 0;

    while (av_read_frame(inCtx, &pkt) >= 0) {
        if (pkt.stream_index == streamIdx) {
            avcodec_send_packet(decCtx, &pkt);
            while (avcodec_receive_frame(decCtx, frame) >= 0) {
                avcodec_send_frame(encCtx, frame);
                while (avcodec_receive_packet(encCtx, &pkt) >= 0) {
                    pkt.stream_index = 0;
                    pkt.pts = av_rescale_q(pkt.pts, encCtx->time_base, outStream->time_base);
                    pkt.dts = av_rescale_q(pkt.dts, encCtx->time_base, outStream->time_base);
                    av_interleaved_write_frame(outCtx, &pkt);
                    av_packet_unref(&pkt);
                }
                processed += frame->nb_samples;
                if (totalDuration > 0) progressCallback(static_cast<int>(processed * 100 / totalDuration));
            }
        }
        av_packet_unref(&pkt);
    }

    // Flush encoder
    avcodec_send_frame(encCtx, nullptr);
    while (avcodec_receive_packet(encCtx, &pkt) >= 0) {
        pkt.stream_index = 0;
        av_interleaved_write_frame(outCtx, &pkt);
        av_packet_unref(&pkt);
    }

    av_write_trailer(outCtx);
    progressCallback(100);

cleanup:
    if (!(outCtx->oformat->flags & AVFMT_NOFILE)) avio_closep(&outCtx->pb);
    avcodec_free_context(&decCtx);
    avcodec_free_context(&encCtx);
    av_frame_free(&frame);
    avformat_free_context(outCtx);
    avformat_close_input(&inCtx);
    return ret >= 0;
}