#include <windows.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")
#include "output2.h"
#include <fstream>

namespace apn
{
	//
	// この関数は指定されたデータを出力ストリームに書き込みます。
	//
	inline void write(auto& stream, const void* data, size_t size)
	{
		stream.write((const char*)data, size);
	}

	//
	// この関数は指定されたデータを出力ストリームに書き込みます。
	//
	inline void write(auto& stream, const auto& data)
	{
		return write(stream, &data, sizeof(data));
	}

	//
	// wavファイルを出力します。
	//
	bool output_wav(OUTPUT_INFO* oip, WORD format_tag, WORD bytes_per_sample)
	{
		// 出力ストリームを開きます。
		std::ofstream file(oip->savefile, std::ios::binary);

		// wavフォーマットです。
		WAVEFORMATEX wf {};
		wf.wFormatTag = format_tag;
		wf.nChannels = oip->audio_ch;
		wf.nSamplesPerSec = oip->audio_rate;
		wf.wBitsPerSample = bytes_per_sample * 8;
		wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

		// 音声信号の総バイト数です。
		auto nb_total_bytes = wf.nBlockAlign * oip->audio_n;

		// wavヘッダーを書き込みます。
		write(file, "RIFF", 4);
		write(file, (int32_t)(36 + nb_total_bytes));
		write(file, "WAVE", 4);

		// fmtチャンクを書き込みます。
		write(file, "fmt ", 4);
		write(file, (int32_t)sizeof(PCMWAVEFORMAT));
		write(file, wf.wFormatTag);
		write(file, wf.nChannels);
		write(file, wf.nSamplesPerSec);
		write(file, wf.nAvgBytesPerSec);
		write(file, wf.nBlockAlign);
		write(file, wf.wBitsPerSample);

		// dataチャンクを書き込みます。
		write(file, "data", 4);
		write(file, (int32_t)nb_total_bytes);

		// データ取得バッファ数を変更します。
		oip->func_set_buffer_size(5, 10);

		// 音声信号を走査します。ただし、1秒毎にまとめて処理します。
		for (int i = 0; i < oip->audio_n; i += oip->audio_rate)
		{
			// 残り時間を表示します。
			oip->func_rest_time_disp(i, oip->audio_n);

			// 中断するかどうか確認します。
			if (oip->func_is_abort()) break;

			// 1秒分の音声信号を取得します。
			int readed = 0;
			auto data = oip->func_get_audio(i, oip->audio_rate, &readed, format_tag);

			// 読み込めなかった場合は何もしません。(サンプルコード通り)
			if (readed == 0) continue;

			// 1秒分の音声信号を書き込みます。
			write(file, data, readed * wf.nBlockAlign);
		}

		return true;
	}
}
