//==============================================================================
//
//  Transcode
//
//  Created by Kwon Keuk Han
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#include "stream.h"
#include <random>
#include "application.h"

#define OV_LOG_TAG "Stream"

using namespace common;

namespace info
{
	Stream::Stream(const info::Application &app_info, StreamSourceType source)
	{
		_app_info = std::make_shared<info::Application>(app_info);

		// ID RANDOM 생성
		SetId(ov::Random::GenerateUInt32() - 1);

		_created_time = std::chrono::system_clock::now();
		_source_type = source;
	}

	Stream::Stream(const info::Application &app_info, info::stream_id_t stream_id, StreamSourceType source)
	{
		_app_info = std::make_shared<info::Application>(app_info);

		_id = stream_id;
		_created_time = std::chrono::system_clock::now();
		_source_type = source;
	}

	Stream::Stream(const Stream &stream)
	{
		_id = stream._id;
		_name = stream._name;
		_source_type = stream._source_type;
		_created_time = stream._created_time;
		_app_info = std::make_shared<info::Application>(stream.GetApplicationInfo());

		for (auto &track : stream._tracks)
		{
			auto new_track = std::make_shared<MediaTrack>(*(track.second.get()));
			AddTrack(new_track);
		}
	}

	Stream::~Stream()
	{
	}

	void Stream::SetId(info::stream_id_t id)
	{
		_id = id;
	}

	info::stream_id_t Stream::GetId() const
	{
		return _id;
	}

	ov::String Stream::GetName() const 
	{
		return _name;
	}

	void Stream::SetName(ov::String name)
	{
		_name = name;
	}

	void Stream::SetOriginStream(const std::shared_ptr<Stream> &stream)
	{
		_origin_stream = stream;
	}

	const std::shared_ptr<Stream> Stream::GetOriginStream()
	{
		return _origin_stream;
	}

	std::chrono::system_clock::time_point Stream::GetCreatedTime() const
	{
		return _created_time;
	}

	StreamSourceType Stream::GetSourceType() const
	{
		return _source_type;
	}

	bool Stream::AddTrack(std::shared_ptr<MediaTrack> track)
	{
		return _tracks.insert(std::make_pair(track->GetId(), track)).second;
	}

	const std::shared_ptr<MediaTrack> Stream::GetTrack(int32_t id) const
	{
		auto item = _tracks.find(id);

		if (item == _tracks.end())
		{
			return nullptr;
		}

		return item->second;
	}

	const std::map<int32_t, std::shared_ptr<MediaTrack>> &Stream::GetTracks() const
	{
		return _tracks;
	}

	void Stream::ShowInfo()
	{
		ov::String out_str = ov::String::FormatString("Stream Information / id(%u), name(%s)", GetId(), GetName().CStr());

		for (auto it = _tracks.begin(); it != _tracks.end(); ++it)
		{
			auto track = it->second;

			// TODO. CODEC_ID를 문자열로 변환하는 공통 함수를 만들어야함.
			ov::String codec_name;

			switch (track->GetCodecId())
			{
				case MediaCodecId::H264:
					codec_name = "avc";
					break;

				case MediaCodecId::Vp8:
					codec_name = "vp8";
					break;

				case MediaCodecId::Vp9:
					codec_name = "vp0";
					break;

				case MediaCodecId::Flv:
					codec_name = "flv";
					break;

				case MediaCodecId::Aac:
					codec_name = "aac";
					break;

				case MediaCodecId::Mp3:
					codec_name = "mp3";
					break;

				case MediaCodecId::Opus:
					codec_name = "opus";
					break;

				case MediaCodecId::None:
				default:
					codec_name = "unknown";
					break;
			}

			switch (track->GetMediaType())
			{
				case MediaType::Video:
					out_str.AppendFormat(
						"\n\tVideo Track #%d: "
						"Bypass(%s) "
						"Bitrate(%d) "
						"codec(%d, %s) "
						"resolution(%dx%d) "
						"framerate(%.2ffps) ",
						track->GetId(),
						track->IsBypass() ? "true" : "false",
						track->GetBitrate(),
						track->GetCodecId(), codec_name.CStr(),
						track->GetWidth(), track->GetHeight(),
						track->GetFrameRate());
					break;

				case MediaType::Audio:
					out_str.AppendFormat(
						"\n\tAudio Track #%d: "
						"Bypass(%s) "
						"Bitrate(%d) "
						"codec(%d, %s) "
						"samplerate(%d) "
						"format(%s, %d) "
						"channel(%s, %d) ",
						track->GetId(),
						track->IsBypass() ? "true" : "false",
						track->GetBitrate(),
						track->GetCodecId(), codec_name.CStr(),
						track->GetSampleRate(),
						track->GetSample().GetName(), track->GetSample().GetSampleSize() * 8,
						track->GetChannel().GetName(), track->GetChannel().GetCounts());
					break;

				default:
					break;
			}

			out_str.AppendFormat("timebase(%s)", track->GetTimeBase().ToString().CStr());
		}

		logti("%s", out_str.CStr());
	}
}  // namespace info