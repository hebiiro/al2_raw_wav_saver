#include <windows.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")
#include "output2.h"
#include <fstream>

namespace apn
{
	//
	// ���̊֐��͎w�肳�ꂽ�f�[�^���o�̓X�g���[���ɏ������݂܂��B
	//
	inline void write(auto& stream, const void* data, size_t size)
	{
		stream.write((const char*)data, size);
	}

	//
	// ���̊֐��͎w�肳�ꂽ�f�[�^���o�̓X�g���[���ɏ������݂܂��B
	//
	inline void write(auto& stream, const auto& data)
	{
		return write(stream, &data, sizeof(data));
	}

	//
	// wav�t�@�C�����o�͂��܂��B
	//
	bool func_output(OUTPUT_INFO* oip)
	{
		// �o�̓X�g���[�����J���܂��B
		std::ofstream file(oip->savefile, std::ios::binary);

		// wav�t�H�[�}�b�g�ł��B
		WAVEFORMATEX wf {};
		wf.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		wf.nChannels = oip->audio_ch;
		wf.nSamplesPerSec = oip->audio_rate;
		wf.wBitsPerSample = sizeof(float) * 8;
		wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

		// �����M���̑��o�C�g���ł��B
		auto nb_total_bytes = wf.nBlockAlign * oip->audio_n;

		// wav�w�b�_�[���������݂܂��B
		write(file, "RIFF", 4);
		write(file, (int32_t)(36 + nb_total_bytes));
		write(file, "WAVE", 4);

		// fmt�`�����N���������݂܂��B
		write(file, "fmt ", 4);
		write(file, (int32_t)sizeof(PCMWAVEFORMAT));
		write(file, wf.wFormatTag);
		write(file, wf.nChannels);
		write(file, wf.nSamplesPerSec);
		write(file, wf.nAvgBytesPerSec);
		write(file, wf.nBlockAlign);
		write(file, wf.wBitsPerSample);

		// data�`�����N���������݂܂��B
		write(file, "data", 4);
		write(file, (int32_t)nb_total_bytes);

		// �f�[�^�擾�o�b�t�@����ύX���܂��B
		oip->func_set_buffer_size(5, 10);

		// �����M���𑖍����܂��B�������A1�b���ɂ܂Ƃ߂ď������܂��B
		for (int i = 0; i < oip->audio_n; i += oip->audio_rate)
		{
			// �c�莞�Ԃ�\�����܂��B
			oip->func_rest_time_disp(i, oip->audio_n);

			// ���f���邩�ǂ����m�F���܂��B
			if (oip->func_is_abort()) break;

			// 1�b���̉����M�����擾���܂��B
			int readed = 0;
			auto data = oip->func_get_audio(i, oip->audio_rate, &readed, WAVE_FORMAT_IEEE_FLOAT);

			// �ǂݍ��߂Ȃ������ꍇ�͉������܂���B(�T���v���R�[�h�ʂ�)
			if (readed == 0) continue;

			// 1�b���̉����M�����������݂܂��B
			write(file, data, readed * wf.nBlockAlign);
		}

		return true;
	}

	//
	// �o�̓v���O�C���̍\���̂ł��B
	//
	OUTPUT_PLUGIN_TABLE output_plugin_table =
	{
		OUTPUT_PLUGIN_TABLE::FLAG_AUDIO,			// �t���O
		L"WAV�t�@�C���o�� (32bit float�̂�)",		// �v���O�C���̖��O
		L"wav files (*.wav)\0*.wav\0",				// �o�̓t�@�C���̃t�B���^
		L"WAV�t�@�C���o�� (32bit float�̂�) r1",	// �v���O�C���̏��
		func_output,								// �o�͎��ɌĂ΂��֐��ւ̃|�C���^
	};

	//
	// �o�̓v���O�C���̍\���̂�Ԃ��܂��B
	//
	EXTERN_C __declspec(dllexport) OUTPUT_PLUGIN_TABLE* GetOutputPluginTable()
	{
		return &output_plugin_table;
	}
}
