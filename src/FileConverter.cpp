#include "FileConverter.hpp"
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>
#include <fstream>

namespace fs = std::filesystem;

FileConverter::FileConverter() {
    av_register_all(); // FFmpeg init
    Magick::InitializeMagick(nullptr); // ImageMagick init
}

FileConverter::~FileConverter() {}

bool FileConverter::convertAndCompress(const std::string& input, const std::string& output, 
                                       const std::string& format, int compressionLevel, 
                                       std::string& errorMsg, std::function<void(int)> progressCallback) {
    if (format == ".zip" || format == ".tar.gz") {
        return compressToArchive(input, output, format, compressionLevel, errorMsg, progressCallback);
    } else if (format == ".mp3" || format == ".mp4" || format == ".avi" || format == ".mkv" || format == ".flv") {
        return transcodeWithFFmpeg(input, output, format, errorMsg, progressCallback);
    } else if (format == ".pdf") {
        return convertPDF(input, output, format, errorMsg, progressCallback);
    } else if (format == ".png" || format == ".jpg" || format == ".svg") {
        return convertImage(input, output, format, errorMsg, progressCallback);
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

// compressToArchive (unchanged from previous)

bool FileConverter::transcodeWithFFmpeg(const std::string& input, const std::string& output, 
                                        const std::string& format, std::string& errorMsg, 
                                        std::function<void(int)> progressCallback) {
    AVFormatContext *inCtx = nullptr, *outCtx = nullptr;
    AVCodecContext *decCtx = nullptr, *encCtx = nullptr;
    int ret;

    if ((ret = avformat_open_input(&inCtx, input.c_str(), nullptr, nullptr)) < 0) {
        errorMsg = "Could not open input file";
        return false;
    }
    if ((ret = avformat_find_stream_info(inCtx, nullptr)) < 0) {
        errorMsg = "Could not find stream info";
        avformat_close_input(&inCtx);
        return false;
    }

    if ((ret = avformat_alloc_output_context2(&outCtx, nullptr, nullptr, output.c_str())) < 0) {
        errorMsg = "Could not create output context";
        avformat_close_input(&inCtx);
        return false;
    }

    int streamIdx = av_find_best_stream(inCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (streamIdx < 0) streamIdx = av_find_best_stream(inCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
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
    AVCodec *encCodec = nullptr;
    if (format == ".mp3") encCodec = avcodec_find_encoder(AV_CODEC_ID_MP3);
    else if (format == ".mp4" || format == ".mkv") encCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    else if (format == ".avi") encCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    else if (format == ".flv") encCodec = avcodec_find_encoder(AV_CODEC_ID_FLV1);
    
    encCtx = avcodec_alloc_context3(encCodec);
    encCtx->sample_rate = decCtx->sample_rate;
    encCtx->channel_layout = decCtx->channel_layout;
    encCtx->channels = decCtx->channels;
    encCtx->bit_rate = 128000; // Adjustable in future
    encCtx->time_base = {1, 1000};
    if (decCtx->codec_type == AVMEDIA_TYPE_VIDEO) {
        encCtx->width = decCtx->width;
        encCtx->height = decCtx->height;
        encCtx->pix_fmt = AV_PIX_FMT_YUV420P; // Common format
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

bool FileConverter::convertPDF(const std::string& input, const std::string& output, 
                               const std::string& format, std::string& errorMsg, 
                               std::function<void(int)> progressCallback) {
    std::string inputExt = fs::path(input).extension().string();
    if (inputExt == ".pdf") {
        // PDF to image (e.g., PNG)
        poppler::document* doc = poppler::document::load_from_file(input);
        if (!doc || doc->is_locked()) {
            errorMsg = "Could not load PDF";
            delete doc;
            return false;
        }
        poppler::page* pg = doc->create_page(0); // First page only for simplicity
        if (!pg) {
            errorMsg = "Could not load PDF page";
            delete doc;
            return false;
        }
        poppler::page_renderer renderer;
        Magick::Image img;
        auto pix = renderer.render_page(pg);
        img.read(pix.width(), pix.height(), "RGB", Magick::CharPixel, pix.data());
        img.write(output); // Output as PNG or similar
        progressCallback(100);
        delete pg;
        delete doc;
        return true;
    } else {
        // Image/text to PDF
        Magick::Image img;
        try {
            img.read(input);
            img.write("pdf:" + output); // ImageMagick handles PDF output
            progressCallback(100);
            return true;
        } catch (Magick::Exception& e) {
            errorMsg = "Image to PDF failed: " + std::string(e.what());
            return false;
        }
    }
}

bool FileConverter::convertImage(const std::string& input, const std::string& output, 
                                 const std::string& format, std::string& errorMsg, 
                                 std::function<void(int)> progressCallback) {
    try {
        Magick::Image img;
        img.read(input);
        if (format == ".svg") {
            // Raster to SVG via ImageMagick (basic tracing)
            img.monochrome(); // Simplify for tracing
            img.write("svg:" + output);
        } else {
            img.write(output); // Direct conversion (e.g., PNG, JPG)
        }
        progressCallback(100);
        return true;
    } catch (Magick::Exception& e) {
        errorMsg = "Image conversion failed: " + std::string(e.what());
        return false;
    }
}