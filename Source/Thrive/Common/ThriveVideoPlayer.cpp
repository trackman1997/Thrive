// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "ThriveVideoPlayer.h"

#include <string>

constexpr auto DEFAULT_READ_BUFFER = 32000;

constexpr auto INPUT_BUFFER_SIZE = 4096;

constexpr auto UE4_SIDE_FORMAT = PF_B8G8R8A8;
// This must match the above definition
constexpr AVPixelFormat FFMPEG_DECODE_TARGET = AV_PIX_FMT_BGRA;

constexpr auto FORMAT_BPP = 32;


int ReadHelper_Read(void *user_data, uint8_t *buf, int buf_size);

int ReadHelper_Write(void *user_data, uint8_t *buf, int buf_size);

int64_t ReadHelper_Seek(void *user_data, int64_t offset, int whence);



// Sets default values
AThriveVideoPlayer::AThriveVideoPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasePlayerMaterialFinder(
        TEXT("/Game/Common/LitTextureBase"));

    if(BasePlayerMaterialFinder.Object)
        BasePlayerMaterial = BasePlayerMaterialFinder.Object;
    
}

AThriveVideoPlayer::~AThriveVideoPlayer(){

    // Ensure all FFMPEG resources are closed
    Close();
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

    
}

// ------------------------------------ //
bool AThriveVideoPlayer::PlayVideo(const FString &NewVideoFile){

    // Make sure ffmpeg is loaded //
    LoadFFMPEG();
    
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
    
    return true;
}

void AThriveVideoPlayer::Close(){

    // Close all ffmpeg resources //

    StreamValid = false;

    // // Dump remaining video frames //
    // {
    //     boost::lock_guard<boost::mutex> lock(ReadPacketMutex);

    //     WaitingVideoPackets.clear();
    //     WaitingAudioPackets.clear();
    // }

    // unhookAudio();

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

    if(Context){
        avcodec_free_context(&Context);
        Context = nullptr;
    }
    
    if(VideoParser){
        av_parser_close(VideoParser);
        VideoParser = nullptr;
    }

    if(AudioParser){
        av_parser_close(AudioParser);
        AudioParser = nullptr;
    }


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

    return false;
}


float AThriveVideoPlayer::GetCurrentTime() const{

    return 0.f;
}

bool AThriveVideoPlayer::IsStreamValid() const{

    return StreamValid && VideoCodec && ConvertedFrameBuffer;
}
// ------------------------------------ //
bool AThriveVideoPlayer::OnVideoDataLoaded(){

    VideoOutput = UMaterialInstanceDynamic::Create(BasePlayerMaterial, this);

    if(!VideoOutput){
        
        LOG_ERROR("Failed to create video output material");
        return false;
    }

    VideoOutputTexture = UTexture2D::CreateTransient(FrameWidth, FrameHeight, UE4_SIDE_FORMAT);

    if(!VideoOutputTexture){

        LOG_ERROR("Failed to create video output texture");
        return false;
    }

    // TODO: in the old code there was a check here to make sure that VideoOutput memory size
    // matches the size of ConvertedBufferSize

    //Make sure it won't be compressed
    VideoOutputTexture->CompressionSettings =
        TextureCompressionSettings::TC_VectorDisplacementmap;
    
    //Turn off Gamma-correction
    VideoOutputTexture->SRGB = 0;

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
    unsigned int videoStream = std::numeric_limits<unsigned int>::max();
    unsigned int audioStream = std::numeric_limits<unsigned int>::max();

    for(unsigned int i = 0; i < FormatContext->nb_streams; ++i){

        if(FormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){

            videoStream = i;
            continue;
        }

        if(FormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){

            audioStream = i;
            continue;
        }
    }

    // Fail if didn't find a stream //
    if(videoStream >= FormatContext->nb_streams){

        LOG_WARNING("Video didn't have a video stream");
        return false;
    }


    if(videoStream < FormatContext->nb_streams){

        // Found a video stream, play it
        if(!OpenStream(videoStream, true)){

            LOG_ERROR("Failed to open video stream");
            return false;
        }
    }

    if(audioStream < FormatContext->nb_streams){

        // Found an audio stream, play it
        if(!OpenStream(audioStream, false)){

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

    FrameWidth = FormatContext->streams[videoStream]->codecpar->width;
    FrameHeight = FormatContext->streams[videoStream]->codecpar->height;

    // Calculate required size for the converted frame
    ConvertedBufferSize = av_image_get_buffer_size(FFMPEG_DECODE_TARGET,
        FrameWidth, FrameHeight, 1);

    ConvertedFrameBuffer = reinterpret_cast<uint8_t*>(av_malloc(
            ConvertedBufferSize * sizeof(uint8_t)));

    if(!ConvertedFrameBuffer){
        LOG_ERROR("av_malloc failed for ConvertedFrameBuffer");
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

        const auto channelLayout = AudioCodec->channel_layout != 0 ?
            AudioCodec->channel_layout :
            // Guess
            av_get_default_channel_layout(AudioCodec->channels);


        AudioConverter = swr_alloc_set_opts(AudioConverter, channelLayout,
            AV_SAMPLE_FMT_S16, AudioCodec->sample_rate,
            channelLayout, AudioCodec->sample_fmt, AudioCodec->sample_rate,
            0, nullptr);

        if(swr_init(AudioConverter) < 0){

            LOG_ERROR("Failed to initialize audio converter for stream");
            return false;
        }

        // Create sound //

		// Create streaming wave object
		PlayingSource = NewObject<UVideoPlayerSoundWave>();
		PlayingSource->SampleRate = SampleRate;
		PlayingSource->NumChannels = ChannelCount;
		PlayingSource->Duration = INDEFINITELY_LOOPING_DURATION;
		PlayingSource->bLooping = false;

		PlayingSource->SoundSource = this;
    }

    DumpInfo();
    ResetClock();

    PassedTimeSeconds = 0.f;
    NextFrameReady = false;
    CurrentlyDecodedTimeStamp = 0.f;

    StreamValid = true;

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

    auto* ThisCodecParser = av_parser_init(ThisStreamCodec->id);

    if(!ThisCodecParser){
        
        LOG_ERROR("failed to allocate codec parser");
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

        VideoParser = ThisCodecParser;
        VideoCodec = ThisCodecContext;
        VideoIndex = static_cast<int>(Index);
        VideoTimeBase = static_cast<float>(FormatContext->streams[Index]->time_base.num) /
            static_cast<float>(FormatContext->streams[Index]->time_base.den);
        // VideoTimeBase = static_cast<float>(VideoCodec->time_base.num) /
        //     static_cast<float>(VideoCodec->time_base.den);

    } else {

        AudioParser = ThisCodecParser;
        AudioCodec = ThisCodecContext;
        AudioIndex = static_cast<int>(Index);
    }

    return true;
}

// ------------------------------------ //
void AThriveVideoPlayer::ResetClock(){

    
}

void AThriveVideoPlayer::OnStreamEndReached(){

    const auto OldVideo = VideoFile;
    Close();
    OnPlayBackEnded.Broadcast(OldVideo);
}

// ------------------------------------ //
void AThriveVideoPlayer::DumpInfo() const{

    if(FormatContext)
        // Passing VideoFile here passes the name onto output, it's not needed
        // but it differentiates the output by file name
        av_dump_format(FormatContext, 0, TCHAR_TO_ANSI(*VideoFile), 0);
}

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


static bool FFMPEGLoadedAlready = false;

void AThriveVideoPlayer::LoadFFMPEG(){

    if(FFMPEGLoadedAlready)
        return;

    FFMPEGLoadedAlready = true;

    avcodec_register_all();
    av_register_all();
}
