#include "../common//al2_wav_saver.hpp"

namespace apn
{
	//
	// wavファイルを出力します。
	//
	bool func_output(OUTPUT_INFO* oip)
	{
		return output_wav(oip, WAVE_FORMAT_PCM, sizeof(short));
	}

	//
	// 出力プラグインの名前です。
	//
	const auto plugin_name = L"WAVファイル出力 (16bit short)";

	//
	// 出力プラグインの構造体です。
	//
	OUTPUT_PLUGIN_TABLE output_plugin_table =
	{
		OUTPUT_PLUGIN_TABLE::FLAG_AUDIO,	// フラグ
		plugin_name,						// プラグインの名前
		L"wav files (*.wav)\0*.wav\0",		// 出力ファイルのフィルタ
		plugin_name,						// プラグインの情報
		func_output,						// 出力時に呼ばれる関数へのポインタ
	};

	//
	// 出力プラグインの構造体を返します。
	//
	EXTERN_C __declspec(dllexport) OUTPUT_PLUGIN_TABLE* GetOutputPluginTable()
	{
		return &output_plugin_table;
	}

	//
	// エントリポイントです。
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
		case DLL_PROCESS_ATTACH:
			{
				::DisableThreadLibraryCalls(instance);

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				break;
			}
		}

		return TRUE;
	}
}
