// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "ThriveVideoPlayer.h"

#include "Generation/TextureHelper.h"

#include <string>

constexpr auto DEFAULT_READ_BUFFER = 32000;

constexpr auto INPUT_BUFFER_SIZE = 4096;

//constexpr auto UE4_SIDE_FORMAT = PF_B8G8R8A8;
constexpr auto UE4_SIDE_FORMAT = PF_R8G8B8A8;
// This must match the above definition
//constexpr AVPixelFormat FFMPEG_DECODE_TARGET = AV_PIX_FMT_BGRA;
constexpr AVPixelFormat FFMPEG_DECODE_TARGET = AV_PIX_FMT_RGBA;

// Bits per pixel in the decode target format and UE4_SIDE_FORMAT
constexpr auto FORMAT_BPP = 32;

// ffmpeg avio context helpers
int ReadHelper_Read(void *user_data, uint8_t *buf, int buf_size);

int ReadHelper_Write(void *user_data, uint8_t *buf, int buf_size);

int64_t ReadHelper_Seek(void *user_data, int64_t offset, int whence);

static int PaCallbackHelper(const void *input, void *output, unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void *userData);


// Sets default values
AThriveVideoPlayer::AThriveVideoPlayer()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasePlayerMaterialFinder(
        TEXT("/Game/Common/UITextureParamBase"));

    if(BasePlayerMaterialFinder.Object)
        BasePlayerMaterial = BasePlayerMaterialFinder.Object;
}

AThriveVideoPlayer::~AThriveVideoPlayer(){

    // Ensure all FFMPEG resources are closed
    Close();

    // Close port audio
    if(bIsPortAudioInitialized){

        auto err = Pa_Terminate();
        
        if(err != paNoError){

            UE_LOG(ThriveLog, Error, TEXT("Error shutting down PortAudio: %s"),
                ANSI_TO_TCHAR(Pa_GetErrorText(err)));
        }
        
        bIsPortAudioInitialized = false;
    }
}

// Called when the game starts or when spawned
void AThriveVideoPlayer::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void AThriveVideoPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(!bIsPlaying)
        return;

    if(!IsStreamValid()){

        LOG_WARNING("Stream is invalid, closing playback");
        OnStreamEndReached();
        return;
    }

    const auto now = ClockType::now();

    const auto elapsed = now - LastUpdateTime;
    LastUpdateTime = now;

    PassedTimeSeconds += std::chrono::duration_cast<
        std::chrono::duration<float>>(elapsed).count();

    // Start playing audio. Hopefully at the same time as the first frame of the
    // video is decoded
    if(!bIsPlayingAudio && AudioStream && AudioCodec){

        LOG_LOG("Starting audio playback from the video...");

        auto err = Pa_StartStream(AudioStream);

        if(err != paNoError){

            UE_LOG(ThriveLog, Error, TEXT("Error starting PortAudio audio stream: %s"),
                ANSI_TO_TCHAR(Pa_GetErrorText(err)));

            bIsPlayingAudio = true;
            
        } else {

            bIsPlayingAudio = true;

            LOG_LOG("Audio playback started");
        }
    }

    // Only decode if there isn't a frame ready
    while(!NextFrameReady){

        // Decode a packet if none are in queue
        if(ReadOnePacket(EDecodePriority::Video) == EPacketReadResult::Ended){

            // There are no more frames, end the playback
            OnStreamEndReached();
            return;
        }

        NextFrameReady = DecodeVideoFrame();
    }

    if(PassedTimeSeconds >= CurrentlyDecodedTimeStamp){

        UpdateTexture();
        NextFrameReady = false;
    }
}

// ------------------------------------ //
bool AThriveVideoPlayer::PlayVideo(const FString &NewVideoFile){

    // Make sure ffmpeg is loaded //
    LoadFFMPEG();

    // Make sure port audio is loaded //
    if(!bIsPortAudioInitialized){

        auto err = Pa_Initialize();
        
        if(err != paNoError){

            UE_LOG(ThriveLog, Error, TEXT("Error starting PortAudio: %s"),
                ANSI_TO_TCHAR(Pa_GetErrorText(err)));
            
            return false;
        }

        bIsPortAudioInitialized = true;
    }
    
    if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*NewVideoFile))
        return false;

    // Create a texture for the video output //

    if(!BasePlayerMaterial){

        LOG_ERROR("VideoPlayer failed to find the player material");
        return false;
    }

    VideoFile = NewVideoFile;

    VideoFileReader = std::unique_ptr<FFileReadHelper>(new FFileReadHelper(VideoFile));

    if(!VideoFileReader || !VideoFileReader->IsValid()){

        Close();
        return false;
    }

    // Parse stream data to know how big our textures need to be //
    if(!FFMPEGLoadFile()){

        LOG_ERROR("VideoPlayer ffmpeg failed to parse / setup playback for the file");
        Close();
        return false;
    }

    if(!OnVideoDataLoaded()){

        VideoOutputTexture = nullptr;
        LOG_ERROR("VideoPlayer ue4 texture / material setup failed");
        return false;
    }

    // Make tick run
    bIsPlaying = true;
    return true;
}

void AThriveVideoPlayer::Close(){

    // Close all ffmpeg resources //
    bStreamValid = false;

    // Stop audio playing first //
    if(bIsPlayingAudio){
        bIsPlayingAudio = false;
    }

    if(AudioStream){

        // Immediately close the stream
        auto err = Pa_AbortStream(AudioStream);

        if(err != paNoError){

            UE_LOG(ThriveLog, Error, TEXT("Error aborting PortAudio stream: %s"),
                ANSI_TO_TCHAR(Pa_GetErrorText(err)));
        }
        
        err = Pa_CloseStream(AudioStream);

        if(err != paNoError){

            UE_LOG(ThriveLog, Error, TEXT("Error closing PortAudio stream: %s"),
                ANSI_TO_TCHAR(Pa_GetErrorText(err)));
        }
        
        AudioStream = nullptr;
    }

    // Unhook the sound wave from us
    // if(PlayingSource){

    //     // This needs investigating whether this can cause crashes or not
    //     PlayingSource->ClearSource();
    // }
    
    // Dump remaining packet data frames //
    {
        std::lock_guard<std::mutex> Lock(ReadPacketMutex);

        WaitingVideoPackets.clear();
        WaitingAudioPackets.clear();
    }

    // Close down audio portion //
    {
        std::lock_guard<std::mutex> Lock(AudioMutex);

        //PlayingSource = nullptr;
        ReadAudioDataBuffer.clear();
    }

    // Video and Audio codecs are released by Context, but we still free them here?
    if(VideoCodec)
        avcodec_free_context(&VideoCodec);
    if(AudioCodec)
        avcodec_free_context(&AudioCodec);
        
    VideoCodec = nullptr;
    AudioCodec = nullptr;

    if(ImageConverter){

        sws_freeContext(ImageConverter);
        ImageConverter = nullptr;
    }

    if(AudioConverter)
        swr_free(&AudioConverter);

    // These are set to null automatically //
    if(DecodedFrame)
        av_frame_free(&DecodedFrame);
    if(DecodedAudio)
        av_frame_free(&DecodedAudio);
    if(ConvertedFrameBuffer)
        av_freep(&ConvertedFrameBuffer);
    if(ConvertedFrame)
        av_frame_free(&ConvertedFrame);

    if(ResourceReader){

        if(ResourceReader->buffer){

            av_free(ResourceReader->buffer);
            ResourceReader->buffer = nullptr;
        }

        av_free(ResourceReader);
        ResourceReader = nullptr;
    }

    if(FormatContext){
        // The doc says this is the right method to close it after
        // avformat_open_input has succeeded
        
        //avformat_free_context(FormatContext);
        avformat_close_input(&FormatContext);
        FormatContext = nullptr;
    }
    
    // if(VideoParser){
    //     av_parser_close(VideoParser);
    //     VideoParser = nullptr;
    // }

    // if(AudioParser){
    //     av_parser_close(AudioParser);
    //     AudioParser = nullptr;
    // }


    // Let go of our textures and things //
    VideoFile = "";
    VideoOutputTexture = nullptr;
    VideoOutput = nullptr;
    

    // Reset other resources //
    
    VideoFileReader.reset();
    bIsPlaying = false;
}
// ------------------------------------ //
bool AThriveVideoPlayer::HasAudio() const{

    return AudioCodec;
}

float AThriveVideoPlayer::GetCurrentTime() const{

    return CurrentlyDecodedTimeStamp;
}

bool AThriveVideoPlayer::IsStreamValid() const{

    return bStreamValid && VideoCodec && ConvertedFrameBuffer;
}

int AThriveVideoPlayer::GetAudioChannelCount() const{

    return ChannelCount;
}

int AThriveVideoPlayer::GetAudioSampleRate() const{

    return SampleRate;
}

int32_t AThriveVideoPlayer::GetVideoWidth() const{

    return FrameWidth;
}

int32_t AThriveVideoPlayer::GetVideoHeight() const{

    return FrameHeight;
}
// ------------------------------------ //
bool AThriveVideoPlayer::OnVideoDataLoaded(){

    VideoOutput = UMaterialInstanceDynamic::Create(BasePlayerMaterial, this);

    if(!VideoOutput){
        
        LOG_ERROR("Failed to create video output material");
        return false;
    }

    VideoOutputTexture = UTexture2D::CreateTransient(FrameWidth, FrameHeight, UE4_SIDE_FORMAT);

    UE_LOG(ThriveLog, Log, TEXT("Created video texture: %dx%d"), FrameWidth, FrameHeight);
    
    if(!VideoOutputTexture){

        LOG_ERROR("Failed to create video output texture");
        return false;
    }

    //Make sure it won't be compressed
    VideoOutputTexture->CompressionSettings =
        TextureCompressionSettings::TC_VectorDisplacementmap;

    // Gamma correction MUST be on for the video to look right!
    // //Turn off Gamma-correction
    // //VideoOutputTexture->SRGB = 0;

    // Allocate the texture RHI resource things
    VideoOutputTexture->UpdateResource();

    VideoOutput->SetTextureParameterValue("Texture", VideoOutputTexture);

    return true;
}

bool AThriveVideoPlayer::FFMPEGLoadFile(){

    unsigned char* ReadBuffer = reinterpret_cast<unsigned char*>(
        av_malloc(DEFAULT_READ_BUFFER));

    if(!ReadBuffer){
        LOG_ERROR("Alloc ReadBuffer failed");
        return false;
    }

    ResourceReader = avio_alloc_context(ReadBuffer, DEFAULT_READ_BUFFER, 0,
        VideoFileReader.get(),
        ReadHelper_Read,
        ReadHelper_Write,
        ReadHelper_Seek);

    if(!ResourceReader){
        LOG_ERROR("avio_alloc_context failed");
        return false;
    }

    FormatContext = avformat_alloc_context();
    if(!FormatContext){
        LOG_ERROR("avformat_alloc_context failed");
        return false;
    }

    FormatContext->pb = ResourceReader;

    // We use a custom io object so we probably can pass null to the url parameter
    // instead of TCHAR_TO_ANSI(*VideoFile)
    if(avformat_open_input(&FormatContext, TCHAR_TO_ANSI(*VideoFile), nullptr, nullptr) < 0){

        // Context was freed automatically
        FormatContext = nullptr;
        LOG_ERROR("FFMPEG failed to open video stream file resource");
        return false;
    }

    if(avformat_find_stream_info(FormatContext, nullptr) < 0){

        LOG_ERROR("Failed to read video stream info");
        return false;
    }

    // Find audio and video streams //
    unsigned int FoundVideoStreamIndex = std::numeric_limits<unsigned int>::max();
    unsigned int FoundAudioStreamIndex = std::numeric_limits<unsigned int>::max();

    for(unsigned int i = 0; i < FormatContext->nb_streams; ++i){

        if(FormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){

            FoundVideoStreamIndex = i;
            continue;
        }

        if(FormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){

            FoundAudioStreamIndex = i;
            continue;
        }
    }

    // Fail if didn't find a stream //
    if(FoundVideoStreamIndex >= FormatContext->nb_streams){

        LOG_WARNING("Video didn't have a video stream");
        return false;
    }


    if(FoundVideoStreamIndex < FormatContext->nb_streams){

        // Found a video stream, play it
        if(!OpenStream(FoundVideoStreamIndex, true)){

            LOG_ERROR("Failed to open video stream");
            return false;
        }
    }

    if(FoundAudioStreamIndex < FormatContext->nb_streams){

        // Found an audio stream, play it
        if(!OpenStream(FoundAudioStreamIndex, false)){

            LOG_WARNING("Failed to open audio stream, playing without audio");
        }
    }

    DecodedFrame = av_frame_alloc();
    ConvertedFrame = av_frame_alloc();
    DecodedAudio = av_frame_alloc();

    if(!DecodedFrame || !ConvertedFrame || !DecodedAudio){
        LOG_ERROR("av_frame_alloc failed");
        return false;
    }

    FrameWidth = FormatContext->streams[FoundVideoStreamIndex]->codecpar->width;
    FrameHeight = FormatContext->streams[FoundVideoStreamIndex]->codecpar->height;

    // Calculate required size for the converted frame
    ConvertedBufferSize = av_image_get_buffer_size(FFMPEG_DECODE_TARGET,
        FrameWidth, FrameHeight, 1);

    ConvertedFrameBuffer = reinterpret_cast<uint8_t*>(av_malloc(
            ConvertedBufferSize * sizeof(uint8_t)));

    if(!ConvertedFrameBuffer){
        LOG_ERROR("av_malloc failed for ConvertedFrameBuffer");
        return false;
    }

    if(ConvertedBufferSize != FrameWidth * FrameHeight * 4){

        LOG_ERROR("FFMPEG and UE4 image data sizes don't match! Check selected formats");
        return false;
    }

    if(av_image_fill_arrays(ConvertedFrame->data, ConvertedFrame->linesize,
            ConvertedFrameBuffer, FFMPEG_DECODE_TARGET,
            FrameWidth, FrameHeight, 1) < 0)
    {
        LOG_ERROR("av_image_fill_arrays failed");
        return false;
    }

    // Converting images to be ue4 compatible is done by this
    // TODO: allow controlling how good conversion is done
    // SWS_FAST_BILINEAR is the fastest
    ImageConverter = sws_getContext(FrameWidth, FrameHeight,
        static_cast<AVPixelFormat>(FormatContext->streams[VideoIndex]->codecpar->format),
        FrameWidth, FrameHeight, FFMPEG_DECODE_TARGET, SWS_BICUBIC,
        nullptr, nullptr, nullptr);

    if(!ImageConverter){

        LOG_ERROR("sws_getContext failed");
        return false;
    }

    if(AudioCodec){

        // Setup audio playing //
        SampleRate = AudioCodec->sample_rate;
        ChannelCount = AudioCodec->channels;

        // This may or may not be a limitation anymore
        if(ChannelCount <= 0 || ChannelCount > 2){
            
            LOG_ERROR("Unsupported audio channel count, "
                "only 1 or 2 are supported");
            return false;
        }

        // cAudio expects AV_SAMPLE_FMT_S16
        //AudioCodec->sample_fmt;
        if(av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) != 2){

            LOG_ERROR("AV_SAMPLE_FMT_S16 size has changed");
            return false;
        }

        AudioConverter = swr_alloc();

        if(!AudioConverter){

            LOG_ERROR("swr_alloc failed");
            return false;
        }

        const auto ChannelLayout = AudioCodec->channel_layout != 0 ?
            AudioCodec->channel_layout :
            // Guess
            av_get_default_channel_layout(AudioCodec->channels);

        // ue4 always consumes signed 16 bit sound samples
        AudioConverter = swr_alloc_set_opts(AudioConverter, ChannelLayout,
            AV_SAMPLE_FMT_S16, AudioCodec->sample_rate,
            ChannelLayout, AudioCodec->sample_fmt, AudioCodec->sample_rate,
            0, nullptr);

        if(swr_init(AudioConverter) < 0){

            LOG_ERROR("Failed to initialize audio converter for stream");
            return false;
        }

        // Create sound //
        auto err = Pa_OpenDefaultStream(&AudioStream,
            // No input
            0,
            // Output
            ChannelCount, paInt16, SampleRate,
            // paFramesPerBufferUnspecified can be used to automatically choose this
            // 1024 (bytes: 4096) seems to be the amount ffmpeg dumps at once
            1024, 
            PaCallbackHelper, this);
        
        if(err != paNoError){

            UE_LOG(ThriveLog, Error, TEXT("Error opening PortAudio playback stream: %s"),
                ANSI_TO_TCHAR(Pa_GetErrorText(err)));

            AudioStream = nullptr;
        }
    }

    DumpInfo();
    ResetClock();

    PassedTimeSeconds = 0.f;
    NextFrameReady = false;
    CurrentlyDecodedTimeStamp = 0.f;

    bStreamValid = true;

    LOG_LOG("VideoPlayer successfully opened all the ffmpeg streams for video file");

    return true;
}

bool AThriveVideoPlayer::OpenStream(unsigned int Index, bool Video){

    auto* ThisStreamCodec = avcodec_find_decoder(
        FormatContext->streams[Index]->codecpar->codec_id);

    if(!ThisStreamCodec){

        LOG_ERROR("unsupported codec used in video file");
        return false;
    }

    auto* ThisCodecContext = avcodec_alloc_context3(ThisStreamCodec);

    if(!ThisCodecContext){

        LOG_ERROR("failed to allocate codec context");
        return false;
    }

    // Try copying parameters //
    if(avcodec_parameters_to_context(ThisCodecContext,
            FormatContext->streams[Index]->codecpar) < 0)
    {
        avcodec_free_context(&ThisCodecContext);
        LOG_ERROR("failed to copy parameters to codec context");
        return false;
    }

    // Open the codec this is important to avoid segfaulting //
    // FFMPEG documentation warns that this is not thread safe
    const auto CodecOpenResult = avcodec_open2(ThisCodecContext, ThisStreamCodec, nullptr);

    if(CodecOpenResult < 0){

        std::string ErrorMessage;
        ErrorMessage.resize(40);
        memset(&ErrorMessage[0], ' ', ErrorMessage.size());
        av_strerror(CodecOpenResult, &ErrorMessage[0], ErrorMessage.size());

        UE_LOG(ThriveLog, Error, TEXT("Error opening codec context: %d (%s)"), CodecOpenResult,
            ANSI_TO_TCHAR(ErrorMessage.c_str()));

        avcodec_free_context(&ThisCodecContext);
        LOG_ERROR("codec failed to open");
        return false;
    }

    // This should probably be done by the caller of this method...
    if(Video){

        //VideoParser = ThisCodecParser;
        VideoCodec = ThisCodecContext;
        VideoIndex = static_cast<int>(Index);
        VideoTimeBase = static_cast<float>(FormatContext->streams[Index]->time_base.num) /
            static_cast<float>(FormatContext->streams[Index]->time_base.den);
        // VideoTimeBase = static_cast<float>(VideoCodec->time_base.num) /
        //     static_cast<float>(VideoCodec->time_base.den);

    } else {

        //AudioParser = ThisCodecParser;
        AudioCodec = ThisCodecContext;
        AudioIndex = static_cast<int>(Index);
    }

    return true;
}

// ------------------------------------ //
bool AThriveVideoPlayer::DecodeVideoFrame(){

    const auto Result = avcodec_receive_frame(VideoCodec, DecodedFrame);

    if(Result >= 0){

        // Worked //
            
        // Convert the image from its native format to RGB
        if(sws_scale(ImageConverter, DecodedFrame->data, DecodedFrame->linesize,
                0, FrameHeight,
                ConvertedFrame->data, ConvertedFrame->linesize) < 0)
        {
            // Failed to convert frame //
            LOG_ERROR("Converting video frame failed");
            return false;
        }

        // Seems like DecodedFrame->pts contains garbage
        // and packet.pts is the timestamp in VideoCodec->time_base
        // so we access that through pkt_pts
        //CurrentlyDecodedTimeStamp = DecodedFrame->pkt_pts * VideoTimeBase;
        //VideoTimeBase = VideoCodec->time_base.num / VideoCodec->time_base.den;
        //CurrentlyDecodedTimeStamp = DecodedFrame->pkt_pts * VideoTimeBase;

        // Seems that the latest FFMPEG version has fixed this.
        // I would put this in a #IF macro bLock if ffmpeg provided a way to check the
        // version at compile time
        CurrentlyDecodedTimeStamp = DecodedFrame->pts * VideoTimeBase;
        return true;
    }

    if(Result == AVERROR(EAGAIN)){

        // Waiting for data //
        return false;
    }

    UE_LOG(ThriveLog, Error, TEXT("Video frame receive failed, error: %d"), Result);
    return false;
}

AThriveVideoPlayer::EPacketReadResult AThriveVideoPlayer::ReadOnePacket(
    EDecodePriority Priority)
{
    if(!FormatContext || !bStreamValid)
        return EPacketReadResult::Ended;

    std::lock_guard<std::mutex> Lock(ReadPacketMutex);

    // Decode queued packets first
    if(Priority == EDecodePriority::Video && !WaitingVideoPackets.empty()){

        // Try to send it //
        const auto Result = avcodec_send_packet(VideoCodec,
            &WaitingVideoPackets.front()->packet);
            
        if(Result == AVERROR(EAGAIN)){

            // Still wailing to send //
            return EPacketReadResult::QueueFull;
        }

        if(Result < 0){

            // An error occured //
            LOG_ERROR("Video stream send error from queue, stopping playback");
            bStreamValid = false;
            return EPacketReadResult::Ended;
        }

        // Successfully sent the first in the queue //
        WaitingVideoPackets.pop_front();
        return EPacketReadResult::Ok;
    }
    
    if(Priority == EDecodePriority::Audio && !WaitingAudioPackets.empty()){

        // Try to send it //
        const auto Result = avcodec_send_packet(AudioCodec,
            &WaitingAudioPackets.front()->packet);
            
        if(Result == AVERROR(EAGAIN)){

            // Still wailing to send //
            return EPacketReadResult::QueueFull;
        } 

        if(Result < 0){

            // An error occured //
            LOG_ERROR("Audio stream send error from queue, stopping playback");
            bStreamValid = false;
            return EPacketReadResult::Ended;
        }

        // Successfully sent the first in the queue //
        WaitingAudioPackets.pop_front();
        return EPacketReadResult::Ok;
    }

    // If we had nothing in the right queue try to read more frames //

    AVPacket Packet;
    //av_init_packet(&packet);

    if(av_read_frame(FormatContext, &Packet) < 0){

        // Stream ended //
        //av_packet_unref(&packet);
        return EPacketReadResult::Ended;
    }

    if(!bStreamValid){

        av_packet_unref(&Packet);
        return EPacketReadResult::Ended;
    }

    // Is this a packet from the video stream?
    if(Packet.stream_index == VideoIndex) {

        // If not wanting this stream don't send it //
        if(Priority != EDecodePriority::Video){

            WaitingVideoPackets.push_back(std::unique_ptr<ReadPacket>(
                    new ReadPacket(&Packet)));
            
            return EPacketReadResult::Ok;
        }

        // Send it to the decoder //
        const auto Result = avcodec_send_packet(VideoCodec, &Packet);

        if(Result == AVERROR(EAGAIN)){

            // Add to queue //
            WaitingVideoPackets.push_back(std::unique_ptr<ReadPacket>(
                    new ReadPacket(&Packet)));
            return EPacketReadResult::QueueFull;
        }

        av_packet_unref(&Packet);
        
        if(Result < 0){

            LOG_ERROR("Video stream send error, stopping playback");
            bStreamValid = false;
            return EPacketReadResult::Ended;
        }

        return EPacketReadResult::Ok;

    } else if(Packet.stream_index == AudioIndex && AudioCodec){
            
        // If audio codec is null audio playback is disabled //
            
        // If not wanting this stream don't send it //
        if(Priority != EDecodePriority::Audio){

            WaitingAudioPackets.push_back(std::unique_ptr<ReadPacket>(
                    new ReadPacket(&Packet)));
            return EPacketReadResult::Ok;
        }
            
        const auto Result = avcodec_send_packet(AudioCodec, &Packet);

        if(Result == AVERROR(EAGAIN)){

            // Add to queue //
            WaitingAudioPackets.push_back(std::unique_ptr<ReadPacket>(
                    new ReadPacket(&Packet)));
            return EPacketReadResult::QueueFull;
        }

        av_packet_unref(&Packet);

        if(Result < 0){

            LOG_ERROR("Audio stream send error, stopping audio playback");
            bStreamValid = false;
            return EPacketReadResult::Ended;
        }

        // This is probably not needed? and was an error before
        //av_packet_unref(&Packet);
        return EPacketReadResult::Ok;
    }

    // Unknown stream, ignore
    av_packet_unref(&Packet);
    return EPacketReadResult::Ok;
}

void AThriveVideoPlayer::UpdateTexture(){

    // This is bytes per pixel instead of bits
    const auto BytesPerPixel = FORMAT_BPP / 8;
    const auto Stride = FrameWidth * BytesPerPixel;

    FTextureHelper::UpdateTextureRegions(VideoOutputTexture, 0, 1,
        new FUpdateTextureRegion2D(0, 0, 0, 0, FrameWidth, FrameHeight), 
        Stride, BytesPerPixel, ConvertedFrameBuffer, false);
}
// ------------------------------------ //
size_t AThriveVideoPlayer::ReadAudioData(uint8_t* Output, size_t Amount){

    std::lock_guard<std::mutex> Lock(AudioMutex);
    
    if(Amount < 1 || !AudioCodec || !bStreamValid)
        return 0;

    // Receive audio packet //
    while(true){

        // First return from queue //
        if(!ReadAudioDataBuffer.empty()){

            // Try to read from the queue //
            const auto ReadAmount = ReadDataFromAudioQueue(Lock, Output, Amount);

            if(ReadAmount == 0){

                // Queue is invalid... //
                LOG_ERROR("Invalid audio queue, emptying the queue");
                ReadAudioDataBuffer.clear();
                continue;
            }

            return ReadAmount;
        }

        const auto ReadResult = avcodec_receive_frame(AudioCodec, DecodedAudio);
            
        if(ReadResult == AVERROR(EAGAIN)){

            if(this->ReadOnePacket(EDecodePriority::Audio) == EPacketReadResult::Ended){

                // Stream ended //
                return 0;
            }

            continue;
        }

        if(ReadResult < 0){

            // Some error //
            LOG_ERROR("Failed receiving audio packet, stopping audio playback");
            bStreamValid = false;
            return 0;
        }

        // Received audio data //

        // This is verified in open when setting up converting
        // av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) could also be used here
        const auto BytesPerSample = 2;

        const auto TotalSize = BytesPerSample * (DecodedAudio->nb_samples
            * ChannelCount);

        if(Amount >= static_cast<size_t>(TotalSize)){
                
            // Lets try to directly feed the converted data to the requester //
            if(swr_convert(AudioConverter, &Output, TotalSize,
                    const_cast<const uint8_t**>(DecodedAudio->data),
                    DecodedAudio->nb_samples) < 0)
            {
                LOG_ERROR("Invalid audio stream, converting to audio read buffer failed");
                bStreamValid = false;
                return 0;
            }

            return TotalSize;
        }
            
        // We need a temporary buffer //
        auto NewBuffer = std::unique_ptr<ReadAudioPacket>(new ReadAudioPacket());

        NewBuffer->DecodedData.SetNum(TotalSize);

        uint8_t* DecodeOutput = &NewBuffer->DecodedData[0];

        // Convert into the output data
        if(swr_convert(AudioConverter, &DecodeOutput, TotalSize,
                const_cast<const uint8_t**>(DecodedAudio->data),
                DecodedAudio->nb_samples) < 0)
        {
            LOG_ERROR("Invalid audio stream, converting failed");
            bStreamValid = false;
            return 0;
        }
                    
        ReadAudioDataBuffer.push_back(std::move(NewBuffer));
        continue;
    }

    // Execution never reaches here
}

size_t AThriveVideoPlayer::ReadDataFromAudioQueue(std::lock_guard<std::mutex> &AudioLocked,
    uint8_t* Output, size_t Amount)
{
    if(ReadAudioDataBuffer.empty())
        return 0;
    
    auto& DataVector = ReadAudioDataBuffer.front()->DecodedData;

    if(Amount >= DataVector.Num()){

        // Can move an entire packet //
        const auto MovedDataCount = DataVector.Num();

        memcpy(Output, &DataVector[0], MovedDataCount);

        ReadAudioDataBuffer.pop_front();

        return MovedDataCount;
    }

    // Need to return a partial packet //
    const auto MovedDataCount = Amount;
    const auto LeftSize = DataVector.Num() - MovedDataCount;

    memcpy(Output, &DataVector[0], MovedDataCount);

    TArray<uint8_t> NewData;
    NewData.SetNum(DataVector.Num() - LeftSize);

    check(NewData.Num() == LeftSize + MovedDataCount);

    FMemory::Memcpy(DataVector.GetData() + DataVector.Num() - LeftSize, NewData.GetData(),
        NewData.Num());

    
    DataVector = NewData;
    return MovedDataCount;
}

int AThriveVideoPlayer::PortAudioDataCallback(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags)
{

    uint8_t* OutputBuffer = reinterpret_cast<uint8_t*>(output);

    bool bReadAny = false;

    uint64_t BytesNeeded = frameCount * ChannelCount * sizeof(int16_t);

    while(BytesNeeded > 0){

        const auto ReadAmount = ReadAudioData(OutputBuffer, BytesNeeded);

        if(!bReadAny && ReadAmount == 0)
            return 1;

        if(ReadAmount == 0)
            break;

        bReadAny = true;
        OutputBuffer += ReadAmount;
        BytesNeeded -= ReadAmount;
    }
    
    return 0;
}
// ------------------------------------ //
void AThriveVideoPlayer::ResetClock(){

    LastUpdateTime = ClockType::now();
}

void AThriveVideoPlayer::OnStreamEndReached(){

    const auto OldVideo = VideoFile;
    Close();
    OnPlayBackEnded.Broadcast(OldVideo);
}

void AThriveVideoPlayer::SeekVideo(float Time){

    if(Time < 0)
        Time = 0;

    const auto SeekPos = static_cast<uint64_t>(Time * AV_TIME_BASE);

    const auto TimeStamp = av_rescale_q(SeekPos, AV_TIME_BASE_Q,
        FormatContext->streams[VideoIndex]->time_base);

    av_seek_frame(FormatContext, VideoIndex, TimeStamp, AVSEEK_FLAG_BACKWARD);
}

// ------------------------------------ //
void AThriveVideoPlayer::DumpInfo() const{

    if(FormatContext)
        // Passing VideoFile here passes the name onto output, it's not needed
        // but it differentiates the output by file name
        av_dump_format(FormatContext, 0, TCHAR_TO_ANSI(*VideoFile), 0);
}
// ------------------------------------ //


// ------------------------------------ //
// FFileReadHelper
FFileReadHelper::FFileReadHelper(const FString &ReadFile){

    File = FPlatformFileManager::Get().GetPlatformFile().OpenRead(*ReadFile, false);
}

FFileReadHelper::~FFileReadHelper(){

    if(File)
        delete File;
    File = nullptr;
}

bool FFileReadHelper::IsValid() const{

    return File;
}

int FFileReadHelper::Read(uint8_t* Buffer, int BufferSize){

    if(!IsValid())
        return 0;

    // Because the unreal public api doesn't tell us how much we read
    // we need to clamp the size
    const auto ShouldRead = FMath::Min<int64_t>(BufferSize, File->Size() - File->Tell());

    if(!File->Read(Buffer, ShouldRead))
        return 0;

    return ShouldRead;
}

int FFileReadHelper::Write(uint8_t *Buffer, int BufferSize){

    if(!IsValid())
        return 0;

    return 0;
}

int64_t FFileReadHelper::Seek(int64_t Offset, int Whence){

    if(!IsValid())
        return -1;
    
    Whence &= ~AVSEEK_FORCE;
    
    if(Whence == AVSEEK_SIZE)
        return File->Size();
    if(Whence == SEEK_SET)
        File->Seek(Offset);
    else if(Whence == SEEK_CUR)
        File->Seek(File->Tell() + Offset);
    else if(Whence == SEEK_END)
        File->Seek(File->Size() + Offset);
    else
        return -1;

    return File->Tell();
}

// ------------------------------------ //
// Read helpers
int ReadHelper_Read(void *user_data, uint8_t *buf, int buf_size){
    
    auto* Stream = static_cast<FFileReadHelper*>(user_data);
    return Stream->Read(buf, buf_size);
}

int ReadHelper_Write(void *user_data, uint8_t *buf, int buf_size){
    
    auto* Stream = static_cast<FFileReadHelper*>(user_data);
    return Stream->Write(buf, buf_size);
}

int64_t ReadHelper_Seek(void *user_data, int64_t offset, int whence){
    
    auto* Stream = static_cast<FFileReadHelper*>(user_data);
    return Stream->Seek(offset, whence);
}


static int PaCallbackHelper(const void *input, void *output, unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void *userData)
{
    return reinterpret_cast<AThriveVideoPlayer*>(userData)
        ->PortAudioDataCallback(input, output, frameCount, timeInfo, statusFlags);
}



static bool FFMPEGLoadedAlready = false;

void AThriveVideoPlayer::LoadFFMPEG(){

    if(FFMPEGLoadedAlready)
        return;

    FFMPEGLoadedAlready = true;

    avcodec_register_all();
    av_register_all();
}
